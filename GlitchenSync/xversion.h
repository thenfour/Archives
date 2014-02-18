

#pragma once

#pragma comment(lib, "version.lib")

class Version
{
public:
  Version() :
    m_fileVerA(0),
    m_fileVerB(0),
    m_fileVerC(0),
    m_fileVerD(0),
		m_prodVerA(0),
		m_prodVerB(0),
		m_prodVerC(0),
		m_prodVerD(0)
  {
  }

	void FromFile(const std::wstring& file)
  {
    DWORD wtf;
    DWORD size;
		size = GetFileVersionInfoSize(file.c_str(), &wtf);
    if(!size) return;

    LibCC::Blob<BYTE> data;
    if(!data.Alloc(size+1)) return;// why +1 ?  just for fun i guess.... no good reason.
    if(!GetFileVersionInfo(file.c_str(), 0, size, data.GetBuffer())) return;

    // fixed info (main shit)
    VS_FIXEDFILEINFO* ffi = 0;
    UINT ffilen = 0;
    if(!VerQueryValue(data.GetBuffer(), L"\\", (void**)&ffi, &ffilen)) return;
    m_fileVerA = HIWORD(ffi->dwFileVersionMS);
    m_fileVerB = LOWORD(ffi->dwFileVersionMS);
		m_fileVerC = HIWORD(ffi->dwFileVersionLS);
    m_fileVerD = LOWORD(ffi->dwFileVersionLS);

		m_prodVerA = HIWORD(ffi->dwProductVersionMS);
    m_prodVerB = LOWORD(ffi->dwProductVersionMS);
		m_prodVerC = HIWORD(ffi->dwProductVersionLS);
    m_prodVerD = LOWORD(ffi->dwProductVersionLS);

    // registrant
    PCSTR pStr;
    if(0 != VerQueryValue(data.GetBuffer(), L"\\StringFileInfo\\040904b0\\RegisteredTo", (void**)&pStr, &ffilen))
	    m_registrant = (const wchar_t*)pStr;

    // copyright
    if(0 != VerQueryValue(data.GetBuffer(), L"\\StringFileInfo\\040904b0\\LegalCopyright", (void**)&pStr, &ffilen))
	    m_copyright = (const wchar_t*)pStr;

    // file description
    if(0 != VerQueryValue(data.GetBuffer(), L"\\StringFileInfo\\040904b0\\FileDescription", (void**)&pStr, &ffilen))
		  m_fileDescription = (const wchar_t*)pStr;

    // product name
    if(0 != VerQueryValue(data.GetBuffer(), L"\\StringFileInfo\\040904b0\\ProductName", (void**)&pStr, &ffilen))
			m_productName = (const wchar_t*)pStr;
  }

	std::wstring GetRegistrant() const { return m_registrant; }
  std::wstring GetCopyright() const { return m_copyright; }
	std::wstring GetFileDescription() const { return m_fileDescription; }
	std::wstring GetProductName() const { return m_productName; }
	std::wstring GetFileVersionString() const
	{
		return LibCC::FormatW(L"%.%.%.%")(m_fileVerA)(m_fileVerB)(m_fileVerC)(m_fileVerD).Str();
	}
	std::wstring GetProductVersionString() const
	{
		return LibCC::FormatW(L"%.%.%.%")(m_prodVerA)(m_prodVerB)(m_prodVerC)(m_prodVerD).Str();
	}


private:
  std::wstring m_registrant;
  std::wstring m_copyright;
	std::wstring m_fileDescription;
	std::wstring m_productName;

  WORD m_fileVerA;
  WORD m_fileVerB;
  WORD m_fileVerC;
  WORD m_fileVerD;

  WORD m_prodVerA;
  WORD m_prodVerB;
  WORD m_prodVerC;
  WORD m_prodVerD;
};

