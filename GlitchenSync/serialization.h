/*
	Originally written for Screenie.
	There are versions where i mistyped the date as May 2 2008 or something. Well it's bogus. This is the later one, 

	April 7, 2008.

	TODO: Support a different (cross platform?) xml parser.

	BASIC USAGE:

		class SomeClass
		{
			void Serialize(Xml::Element parent) const
			{
				Xml::SerializeAttribute(parent, L"ElementName", m_anything);
				// or SerializeElement or SerializeCollection etc.
			}
			void Deserialize(Xml::Element parent)
			{
				Xml::Deserialize(parent, L"ElementName", m_anything);
			}
		};


	And to actually start the serialization / deserialization process, call
	Xml::Serialize(ref_to_class, file_name);

	or the same for Deserialize().

*/


#pragma once

#include <map>
#include <vector>
#include "..\libcc\libcc/StringUtil.hpp"
#include "..\libcc\libcc/Result.hpp"
#include "..\libcc\libcc/blob.hpp"
#import "msxml3.dll"

//extern "C" size_t Curl_base64_encode(const char *input, size_t size, char **str);
//extern "C" size_t Curl_base64_decode(const char *source, unsigned char **outptr);



inline _bstr_t _FormatXML(MSXML2::IXMLDOMNodePtr ptrNode, int iTabLevel)
{
 _bstr_t bstrNode;

 switch (ptrNode->GetnodeType())
 {
 case MSXML2::NODE_DOCUMENT:
 case MSXML2::NODE_DOCUMENT_FRAGMENT:
  {
   // All children of document/document fragment are at same level.
   for (long i=0; i<ptrNode->GetchildNodes()->Getlength(); i++)
    bstrNode += _FormatXML(ptrNode->GetchildNodes()->Getitem(i), iTabLevel);
  }
  break;

 case MSXML2::NODE_TEXT:
  // Format like IE5 renders.
  {
   bool bPartOfMixedContent =
    ptrNode->GetparentNode()->GetchildNodes()->Getlength() > 1;
   if (bPartOfMixedContent)
    // Indent if part of mixed content.
		bstrNode = std::wstring(iTabLevel, '\t').c_str();

   // Call Getxml() here, otherwise entities (&gt;) get expanded.
   // Trim leading and trailing whitespace to mimic Gettext().
	 std::wstring strXML = LibCC::StringTrim((PWSTR)ptrNode->Getxml(), L" \t\r\n");
	 bstrNode += strXML.c_str();

   if (bPartOfMixedContent)
    // Add carriage-return/line feed if part of mixed content.
    bstrNode += L"\r\n";
  }
  break;

 case MSXML2::NODE_ELEMENT:
  {
   // Indent.
	bstrNode = std::wstring(iTabLevel, '\t').c_str();

   // Open the start tag.
   bstrNode += L"<" + ptrNode->GetnodeName();

   // Add the attributes.
   for (long i=0; i<ptrNode->Getattributes()->Getlength(); i++)
    bstrNode +=
     L" " + ptrNode->Getattributes()->Getitem(i)->Getxml();

   // Determine how to close start tag with help of following bools.
   bool bHasChildNodes = ptrNode->hasChildNodes() == VARIANT_TRUE;
   bool bHasOnlyATextChildNode =
    bHasChildNodes &&
    ptrNode->GetchildNodes()->Getlength() == 1 &&
    ptrNode->GetchildNodes()->Getitem(0)->GetnodeType() ==
    MSXML2::NODE_TEXT;

   if (!bHasChildNodes)
    // It is an empty element; close it.
    bstrNode += L"/>\r\n";
   else if (bHasOnlyATextChildNode)
    // It has a single text node; don't add carriage return.
    bstrNode += L">";
   else
    // It has more than one text child node; add carriage return.
    bstrNode += L">\r\n";

   // Recurse if it has children.
   if (bHasChildNodes)
    for (int i=0; i<ptrNode->GetchildNodes()->Getlength(); i++)
     bstrNode +=
      _FormatXML
      (
       ptrNode->GetchildNodes()->Getitem(i),
       iTabLevel+1
      );

   // Indent properly and add end tag.
   if (!bHasOnlyATextChildNode && bHasChildNodes)
		 bstrNode += std::wstring(iTabLevel, '\t').c_str();
   if (bHasChildNodes)
    bstrNode += L"</" + ptrNode->GetnodeName() + L">\r\n";
  }
  break;

 case MSXML2::NODE_ENTITY:
 case MSXML2::NODE_ATTRIBUTE:
  // These cases should never occur, but don't do anything if they do.
  break;

 default:
  // All other node types should return their XML (properly indented).
  bstrNode = std::wstring(iTabLevel, '\t').c_str() + ptrNode->Getxml() + L"\r\n";
  break;
 } 

 return bstrNode;
}

namespace Xml
{
	typedef MSXML2::IXMLDOMDocumentPtr Document;
	typedef MSXML2::IXMLDOMElementPtr Element;
	typedef MSXML2::IXMLDOMNodePtr Node;

	// Fundamental conversion functions -----------------------------------------------------------------------
	inline void ToString(const std::wstring& in, std::wstring& out)
	{
		out = in;
	}
	inline void FromString(const std::wstring& in, std::wstring& out)
	{
		out = in;
	}

	inline void ToString(const bool& in, std::wstring& out)
	{
		out = in ? L"yes" : L"no";
	}
	inline void FromString(const std::wstring& in, bool& out)
	{
		out = (in == L"yes");
	}

	template<typename T>
	inline void ToStringViaVariant(const T& in, std::wstring& out)
	{
		_variant_t temp(in);
		temp.ChangeType(VT_BSTR);
		out = temp.bstrVal;
	}
	inline _variant_t ToVariant(const std::wstring& in, VARTYPE vt)
	{
		_variant_t temp(in.c_str());
		temp.ChangeType(vt);
		return temp;
	}

	inline void ToString(const float& in, std::wstring& out)
	{
		ToStringViaVariant(in, out);
	}
	inline void FromString(const std::wstring& in, float& out)
	{
		out = ToVariant(in, VT_R4).fltVal;
	}
	inline void ToString(const int& in, std::wstring& out)
	{
		ToStringViaVariant(in, out);
	}
	inline void FromString(const std::wstring& in, int& out)
	{
		out = ToVariant(in, VT_I4).intVal;
	}
	inline void ToString(const unsigned short& in, std::wstring& out)
	{
		ToStringViaVariant((int)in, out);
	}
	inline void FromString(const std::wstring& in, unsigned short& out)
	{
		out = (unsigned short)(ToVariant(in, VT_I4).intVal);
	}

	//class BinaryData
	//{
	//public:
	//	BinaryData() :
	//		p(0),
	//		size(0)
	//	{
	//	}
	//	template<typename T>
	//	BinaryData(const T* x, size_t elements = 1) :
	//		p(x),
	//		size(elements * sizeof(T))
	//	{
	//	}
	//	const void* p;
	//	size_t size;
	//};
	//inline void ToString(const BinaryData& in, std::wstring& out)
	//{
	//	char* outA;
	//	size_t size = Curl_base64_encode((const char*)in.p, in.size, &outA);
	//	LibCC::StringConvert(std::string(outA, size), out);
	//	free(outA);
	//}
	//inline void FromString(const std::wstring& in, LibCC::Blob<BYTE>& out)
	//{
	//	std::string a;
	//	LibCC::StringConvert(in, a);
	//	unsigned char* buf;
	//	size_t size = Curl_base64_decode(a.c_str(), &buf);
	//	out.Alloc(size);
	//	memcpy(out.GetBuffer(), buf, size);
	//	free(buf);
	//}

	// for serializing wstring
	template<typename T>
	inline void SerializeAttribute(Element parent, const std::wstring& name, const T& val)
	{
		try
		{
			std::wstring str;
			ToString(val, str);
			parent->setAttribute(name.c_str(), str.c_str());
		}
		catch(_com_error&)
		{
		}
	}

	template<typename T>
	inline bool DeserializeAttribute(Element parent, const std::wstring& name, T& val)
	{
		try
		{
			_variant_t var = parent->getAttribute(name.c_str());
			if (var.vt == VT_EMPTY || var.vt == VT_NULL)
			{
				return false;
			}
			FromString(var.bstrVal, val);
			return true;
		}
		catch(_com_error&)
		{
		}
		return false;
	}

	// ----------------------------------

	// catch-all for custom types.
	template<typename T>
	inline void SerializeElement(Element el, const T& val)
	{
		try
		{
			val.Serialize(el);
		}
		catch(_com_error&)
		{
		}
	}
	template<typename T>
	inline void SerializeElement(Element parent, const std::wstring& tagName, const T& val)
	{
		try
		{
			Element el = parent->ownerDocument->createElement(tagName.c_str());
			parent->appendChild(el);
			SerializeElement(el, val);
		}
		catch(_com_error&)
		{
		}
	}

	//inline void SerializeAttribute(Element parent, const std::wstring& name, const std::wstring& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}
	//inline void SerializeAttribute(Element parent, const std::wstring& name, const bool& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}
	//inline void SerializeAttribute(Element parent, const std::wstring& name, const float& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}
	//inline void SerializeAttribute(Element parent, const std::wstring& name, const BinaryData& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}
	//inline void SerializeAttribute(Element parent, const std::wstring& name, const int& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}
	inline void SerializeElement(Element parent, const std::wstring& name, const int& val)
	{
		Element el = parent->ownerDocument->createElement(name.c_str());
		parent->appendChild(el);
		std::wstring str;
		ToString(val, str);
		el->text = str.c_str();
	}
	//inline void SerializeAttribute(Element parent, const std::wstring& name, const unsigned short& val)
	//{
	//	SerializeAttribute(parent, name, val);
	//}

	template<typename T>
	inline void SerializeCollection(Element parent, const std::wstring& collectionElementNameOrEmpty, const std::wstring& itemElementName, const std::vector<T>& src)
	{
		try
		{
			// create a single element to contain the elements.
			Element elCollection = parent;
			if(!collectionElementNameOrEmpty.empty())
			{
				elCollection = parent->ownerDocument->createElement(collectionElementNameOrEmpty.c_str());
				parent->appendChild(elCollection);
			}

			for(std::vector<T>::const_iterator it = src.begin(); it != src.end(); ++ it)
			{
				// create an element for the item itself
				SerializeElement(elCollection, itemElementName, *it);
			}
		}
		catch(_com_error&)
		{
		}
	}

	template<typename T>
	inline void SerializeDocument(const T& o, const std::wstring& fileName, const std::wstring& rootElementName)
	{
		CoInitialize(NULL);
		{
			try
			{
				Document doc;
				doc.CreateInstance(L"Msxml2.DOMDocument");
				if(doc != 0)
				{
					// create the root element.
					Element root;
					root = doc->createElement(rootElementName.c_str());
					doc->appendChild(root);

					// serialize everything
					SerializeElement(root, o);

					// save
					_bstr_t newXml = _FormatXML(doc, 0);
					Document n;
					n.CreateInstance(L"Msxml2.DOMDocument");
					n->loadXML(newXml);
					n->save(fileName.c_str());
				}
			}
			catch(_com_error&)
			{
			}
		}
		CoUninitialize();
	}

	/////////////////////
	// catch-all for custom types.
	template<typename T>
	inline bool DeserializeElement(Element el, T& val)
	{
		try
		{
			val.Deserialize(el);
			return true;
		}
		catch(_com_error&)
		{
		}
		return false;
	}

	template<typename T>
	inline bool DeserializeElement(Element parent, const std::wstring& name, T& val)
	{
		try
		{
			Element el = parent->selectSingleNode(name.c_str());
			if(el == 0)
			{
				return false;
			}
			return Deserialize(el, val);
		}
		catch(_com_error&)
		{
		}
		return false;
	}

	//inline bool DeserializeAttribute(Element parent, const std::wstring& name, std::wstring& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}
	//inline bool Deserialize(Element parent, const std::wstring& name, bool& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}
	//inline bool DeserializeAttribute(Element parent, const std::wstring& name, float& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}
	//inline bool DeserializeAttribute(Element parent, const std::wstring& name, LibCC::Blob<BYTE>& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}
	//inline bool DeserializeAttribute(Element parent, const std::wstring& name, int& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}
	//inline bool DeserializeAttribute(Element parent, const std::wstring& name, unsigned short& val)
	//{
	//	return DeserializeAttribute(parent, name, val);
	//}

	template<typename T>
	inline bool DeserializeCollection(Element parent, const std::wstring& itemXPath, std::vector<T>& src)
	{
		try
		{
			MSXML2::IXMLDOMNodeListPtr x = parent->selectNodes(itemXPath.c_str());
			if(x == 0)
			{
				return true;// no items; that's fine.
			}

			long l = x->length;
			for(long i = 0; i < l; ++ i)
			{
				Element item = x->item[i];
				src.push_back(T());
				if(!DeserializeElement(item, src.back()))
				{
					return false;// this may cut deserialization short.
				}
			}
		}
		catch(_com_error&)
		{
		}
		return false;
	}

	struct AutoCoInitialize
	{
		AutoCoInitialize()
		{
			CoInitialize(NULL);
		}
		~AutoCoInitialize()
		{
			CoUninitialize();
		}
	};

	template<typename T>
	inline LibCC::Result DeserializeDocument(T& o, const std::wstring& fileName, const std::wstring& rootElementName)
	{
		AutoCoInitialize aci;
		Document doc;
		LibCC::Result x;
		x.Assign(doc.CreateInstance(L"Msxml2.DOMDocument"));
		if(x.Failed())
			return x.Prepend(L"Error loading file - could not initialize MSXML");

		if(VARIANT_TRUE != doc->load(fileName.c_str()))
			return LibCC::Result::Failure(LibCC::Format(L"Error parsing file %").qs(fileName).Str());

		if(!DeserializeElement(doc->selectSingleNode(rootElementName.c_str()), o))
			return LibCC::Result::Failure(L"Error deserializing file");
		return LibCC::Result::Success();
	}
}
