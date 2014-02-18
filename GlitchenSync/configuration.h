/*

need to test:
when root changes from directory to file.

*/

#pragma once

#include <vector>
#include "serialization.h"
#include "..\libcc\libcc/log.hpp"
#include "..\libcc\libcc/PathMatchSpec.hpp"


class Subscriber
{
public:
	class Filter
	{
	public:
		enum Type
		{
			Include,
			Ignore
		};
		static std::wstring TypeToString(Type t)
		{
			switch(t)
			{
			case Include:
				return L"Include";
			case Ignore:
				return L"Ignore";
			}
			return L"(unknown type)";
		}
		static Type StringToType(const std::wstring& s)
		{
			std::wstring l = LibCC::StringToLower(s);
			if(l == L"include")
				return Include;
			return Ignore;
		}

		Type type;
		LibCC::PathMatchSpecP4 criteria;// criteria is NOT an absolute path. it is verbatim what is in the xml file.

		Filter() :
			type(Include)
		{
		}

		Filter(Type _type, const std::wstring& _criteria) :
			type(Include)
		{
			criteria.SetCriteria(_criteria);
		}

		void Serialize(Xml::Element parent) const
		{
			Xml::SerializeAttribute(parent, L"type", TypeToString(type));
			Xml::SerializeAttribute(parent, L"criteria", criteria.GetCriteria());
		}

		void Deserialize(Xml::Element parent)
		{
			std::wstring temp;
			Xml::DeserializeAttribute(parent, L"criteria", temp);
			criteria.SetCriteria(temp);

			Xml::DeserializeAttribute(parent, L"type", temp);
			type = StringToType(temp);
		}
	};

	Subscriber() :
		modifiedDatesAreAccurate(false)
	{
	}
	
	Subscriber(const std::wstring& _root, bool isSource, bool isDest) :
		modifiedDatesAreAccurate(false),
		root(_root),
		isSyncSource(isSource),
		isSyncDestination(isDest)
	{
	}

	std::vector<Filter> sourceFilters;// and'd together. a file needs to pass all tests to make it into sync. otherwise it won't be touched (deleted, etc)
	std::vector<Filter> destinationFilters;

	std::wstring id;
	std::wstring root;// must be an absolute path, and end in a backslash
	bool isSyncSource;
	bool isSyncDestination;

	// used during the sync.
	bool modifiedDatesAreAccurate;// basically, FAT32 doesn't support accurate filetimes. they are all local times, which can change based on daylight savings, and are not compatible with NTFS dates. so this flag is an indication whether dates from this subscriber can be trusted.

	// basically, turn C:\blah\omg\lol.txt, with root of c:\blah into omg\lol.txt
	std::wstring GetRelativePath(const std::wstring& fullPath)
	{
		_ASSERT(LibCC::StringToLower(fullPath).find(LibCC::StringToLower(root)) == 0);

		std::wstring::size_type pos = root.size();
		_ASSERT(LibCC::StringContains(L"\\/", fullPath[pos]));
		//if(fullPath[pos] == '\\' || fullPath[pos] == '/')
		//	pos ++;

		return fullPath.substr(pos);
	}

	bool PassesSourceFilterCriteria(const std::wstring& relativePath) const
	{
		return PassesFilterCriteria(sourceFilters, relativePath);
	}
	bool PassesDestinationFilterCriteria(const std::wstring& relativePath) const
	{
		return PassesFilterCriteria(destinationFilters, relativePath);
	}
	static bool PassesFilterCriteria(const std::vector<Filter>& filters, const std::wstring& relativePath)
	{
		bool includesPass = false;
		bool excludesPass = true;
		for(std::vector<Filter>::const_iterator itFilter = filters.begin(); itFilter != filters.end(); ++ itFilter)
		{
			if(itFilter->criteria.Match(relativePath))
			{
				switch(itFilter->type)
				{
				case Subscriber::Filter::Ignore:
					excludesPass = false;
					break;
				case Subscriber::Filter::Include:
					includesPass = true;
					break;
				}
			}
		}
		return includesPass && excludesPass;
	}

	void Serialize(Xml::Element parent) const
	{
		Xml::SerializeCollection(parent, L"", L"sourceFilter", sourceFilters);
		Xml::SerializeCollection(parent, L"", L"destinationFilter", destinationFilters);
		Xml::SerializeAttribute(parent, L"id", id);
		Xml::SerializeAttribute(parent, L"root", root);
		Xml::SerializeAttribute(parent, L"isSyncSource", isSyncSource);
		Xml::SerializeAttribute(parent, L"isSyncDestination", isSyncDestination);
	}

	void Deserialize(Xml::Element parent)
	{
		Xml::DeserializeCollection(parent, L"sourceFilter", sourceFilters);
		Xml::DeserializeCollection(parent, L"destinationFilter", destinationFilters);
		Xml::DeserializeAttribute(parent, L"id", id);// REQUIRED
		id = LibCC::StringTrim(id, L" \t\r\n");
		id = LibCC::StringToLower(id);
		Xml::DeserializeAttribute(parent, L"root", root);
		if(*root.rbegin() != L'\\')
			root.push_back(L'\\');
		Xml::DeserializeAttribute(parent, L"isSyncSource", isSyncSource);
		Xml::DeserializeAttribute(parent, L"isSyncDestination", isSyncDestination);
	}
};


class SyncScript
{
public:
	///////////////////////////////
	enum ComparisonType
	{
		UnknownComparisonType = 0,
		ModifiedDateUTC = 1,// only available on NTFS because of how unreliable fat32 dates are.
		Adler32 = 2,// TODO
		CRC32 = 3,// TODO
		MD5 = 4,
		SHA1 = 5,// probably not necessary. this would be less efficient than md5, and probably the extra reliability is not necessary
	};
	static std::wstring ComparisonTypeToString(ComparisonType t)
	{
		switch(t)
		{
		case ModifiedDateUTC:
			return L"ModifiedDateUTC";
		case Adler32:
			return L"Adler32";
		case CRC32:
			return L"CRC32";
		case MD5:
			return L"MD5";
		case SHA1:
			return L"SHA1";
		}
		return L"(unknown comparison type)";
	}
	static ComparisonType StringToComparisonType(const std::wstring& s)
	{
		std::wstring l = LibCC::StringToLower(s);
		if(l == L"modifieddateutc")
			return UnknownComparisonType;
		if(l == L"adler32")
			return Adler32;
		if(l == L"crc32")
			return CRC32;
		if(l == L"md5")
			return MD5;
		if(l == L"sha1")
			return SHA1;
		return UnknownComparisonType;
	}
	///////////////////////////////
	SyncScript() { }
	SyncScript(ComparisonType _comparisonType, const std::wstring& _fileDB, const std::wstring& _name) :
		comparisonType(_comparisonType),
		fileDB(_fileDB),
		name(_name)
	{
	}

	std::vector<Subscriber> subscribers;
	ComparisonType comparisonType;
	std::wstring fileDB;
	std::wstring name;

	Subscriber* FindSubscriber(const std::wstring& root)
	{
		for(std::vector<Subscriber>::iterator it = subscribers.begin(); it != subscribers.end(); ++ it)
		{
			if(LibCC::StringToLower(it->root) == LibCC::StringToLower(root))
				return &*it;
		}
		return 0;
	}

	void Serialize(Xml::Element parent) const
	{
		Xml::SerializeCollection(parent, L"", L"subscriber", subscribers);
		Xml::SerializeAttribute(parent, L"comparisonType", ComparisonTypeToString(comparisonType));
		Xml::SerializeAttribute(parent, L"fileDB", fileDB);
		Xml::SerializeAttribute(parent, L"name", name);
	}

	void Deserialize(Xml::Element parent)
	{
		Xml::DeserializeCollection(parent, L"subscriber", subscribers);
		std::wstring temp;
		Xml::DeserializeAttribute(parent, L"comparisonType", temp);
		comparisonType = StringToComparisonType(temp);
		Xml::DeserializeAttribute(parent, L"fileDB", fileDB);
		Xml::DeserializeAttribute(parent, L"name", name);
	}
};

class ConfigFile
{
public:
	ConfigFile() { }

	std::vector<SyncScript> scripts;
	std::wstring fileName;

	void Save(const std::wstring& fileName)
	{
		Xml::SerializeDocument(*this, fileName, L"GlitchenSync");
		this->fileName = fileName;
	}
	LibCC::Result Load(const std::wstring& fileName)
	{
		this->fileName = fileName;
		//log->Message(LibCC::Format(L"Loading configuration file from %")(fileName));
		return Xml::DeserializeDocument(*this, fileName, L"GlitchenSync");
	}

	void Serialize(Xml::Element parent) const
	{
		Xml::SerializeCollection(parent, L"", L"script", scripts);
	}
	void Deserialize(Xml::Element parent)
	{
		Xml::DeserializeCollection(parent, L"script", scripts);
	}
};

