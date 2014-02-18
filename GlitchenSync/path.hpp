/*
provides a minimal syntax for doing filesystem operations. for example,
Path("c:\something")["temp"]["somedir"]["file.txt"].CreateTextFile("contents");
Path("c:\something")["temp"]["somedir"].GetFiles("...");
Path("c:\something")["temp"]["somedir"].DeleteAllChildren();

*/

#pragma once

#include "..\libcc\libcc\winapi.hpp"
#include "..\libcc\libcc\PathMatchSpec.hpp"
#include "..\libcc\libcc\blob.hpp"
#include "md5.hpp"
#include "utility.hpp"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

#pragma warning(push)
#pragma warning(disable:4996)



// can represent both files or directories.
class Path
{
	std::wstring m_base;

	bool m_basicInfoValid;
	bool m_isDirectory;// basic info
	bool m_isFile;// basic info
	bool m_exists;// basic info
	UINT64 m_size;// basic info
	FILETIME m_modifiedDate;// basic info

	template<typename Char>
	static LibCC::Blob<Char> DoubleNullTerminate(const std::basic_string<Char>& s)
	{
		LibCC::Blob<Char> temp(s.size() + 2);
		LibCC::XLastDitchStringCopy(s, temp.GetBuffer());
		temp.GetBuffer()[s.length()+1] = 0;
		return temp;
	}

	bool PopulateBasicInfo()
	{
		//if(m_basicInfoValid)
		//	return true;

		m_basicInfoValid = true;
		m_exists = false;
		m_isDirectory = false;
		m_isFile = false;
		m_size = 0;

		WIN32_FILE_ATTRIBUTE_DATA fileData;
		if(0 == GetFileAttributesExW(m_base.c_str(), GetFileExInfoStandard, &fileData))
			return true;

		m_exists = true;
		m_isDirectory = 0 != (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		m_isFile = !m_isDirectory;
		m_size = MakeUINT64(fileData.nFileSizeLow, fileData.nFileSizeHigh);
		m_modifiedDate = fileData.ftLastWriteTime;
		return true;
	}

public:
	Path() :
		m_basicInfoValid(false)
	{
	}

	Path(const std::wstring& base) :
		m_base(base),
		m_basicInfoValid(false)
	{
	}

	Path(const std::string& base) :
		m_basicInfoValid(false)
	{
		m_base = LibCC::ToUTF16(base);
	}

	Path(const Path& p) :
		m_base(p.m_base),
		m_basicInfoValid(p.m_basicInfoValid),
		m_isDirectory(p.m_isDirectory),
		m_isFile(p.m_isFile),
		m_exists(p.m_exists),
		m_size(p.m_size),
		m_modifiedDate(p.m_modifiedDate)
	{
	}

	Path operator[] (const std::string& sub)
	{
		return (*this)[LibCC::ToUTF16(sub)];
	}

	Path operator[] (const std::wstring& sub)
	{
		return Path(LibCC::PathJoin(m_base, sub));
	}

	std::string GetPathA() const
	{
		return LibCC::ToANSI(m_base);
	}

	std::wstring GetPathW() const
	{
		return m_base;
	}

	std::vector<Path> GetFiles(const std::wstring& spec) const
	{
		std::vector<Path> ret;
		LibCC::RecursiveFileIteratorW x;
		if(!x.Reset(m_base))
			return ret;

		std::wstring fullPath;
		WIN32_FIND_DATAW fd;
		LibCC::PathMatchSpecP4W p4;
		p4.SetCriteria(LibCC::PathAppendX(m_base, spec));

		while(x.Next(fullPath, fd))
		{
			if(p4.Match(fullPath))
				ret.push_back(fullPath);
		}
		return ret;
	}
	std::vector<Path> GetFiles(const std::string& spec) const
	{
		return GetFiles(LibCC::ToUTF16(spec));
	}

	FILETIME GetModifiedDate()
	{
		PopulateBasicInfo();
		return m_modifiedDate;
	}

	UINT64 GetSize()
	{
		PopulateBasicInfo();
		return m_size;
	}

	bool IsDirectory()
	{
		PopulateBasicInfo();
		return m_isDirectory;
	}

	bool IsFile()
	{
		PopulateBasicInfo();
		return m_isFile;
	}

	bool Exists()
	{
		PopulateBasicInfo();
		return m_exists;
	}

	bool Delete()
	{
		PopulateBasicInfo();
		if(!m_exists)
			return true;
		SHFILEOPSTRUCTW sh = {0};
		sh.hwnd = GetDesktopWindow();
		sh.wFunc = FO_DELETE;
		LibCC::Blob<wchar_t> pFrom = DoubleNullTerminate(m_base);
		sh.pFrom = pFrom.GetBuffer();// SHFileOperation needs 2 nulls.
		sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		if(0 != SHFileOperationW(&sh))
			return false;
		m_basicInfoValid = false;
		return true;
	}

	bool CopyTo(const Path& rhs)
	{
		PopulateBasicInfo();
		if(!m_exists)
			return false;

		// if the dest dir doesn't exist already, then SHFileOperation needs confirmation to create the directories. we don't need that!
		if(!rhs.GetParent().Exists())
		{
			if(!rhs.GetParent().CreateDirectory_())
				return false;
		}

		SHFILEOPSTRUCTW sh = {0};
		sh.hwnd = GetDesktopWindow();
		sh.wFunc = FO_COPY;
		LibCC::Blob<wchar_t> pFrom = DoubleNullTerminate(m_base);
		LibCC::Blob<wchar_t> pTo = DoubleNullTerminate(rhs.m_base);
		sh.pFrom = pFrom.GetBuffer();
		sh.pTo = pTo.GetBuffer();// SHFileOperation needs 2 nulls.
		sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;
		//sh.fFlags = 0;
		int r = SHFileOperationW(&sh);
		if(0 != r)
			return false;
		m_basicInfoValid = false;
		return true;
	}

	bool CreateDirectory_()
	{
		PopulateBasicInfo();
		if(m_exists && m_isDirectory)
			return true;
		if(m_exists && m_isFile)
			return false;

		std::wstring wbase = LibCC::ToUTF16(m_base);
		int res = SHCreateDirectory(0, wbase.c_str());
		m_basicInfoValid = false;
		return ERROR_SUCCESS == res;
	}

	Path GetParent() const
	{
		LibCC::Blob<wchar_t> copy(m_base.size());
		wcscpy(copy.GetBuffer(), m_base.c_str());
		PathRemoveBackslashW(copy.GetBuffer());
		PathRemoveFileSpecW(copy.GetBuffer());
		return Path(copy.GetBuffer());
	}

	bool CreateTextFile(const std::string& contents)
	{
		// force overwriting
		if(!Delete())
			return false;
		// make sure the parent directory exists
		if(!GetParent().CreateDirectory_())
			return false;

		HANDLE h = CreateFileW(m_base.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_NEW, 0, 0);
		if(LibCC::IsBadHandle(h))
			return false;

		DWORD bw = 0;
		WriteFile(h, contents.c_str(), contents.size(), &bw, 0);

		CloseHandle(h);

		m_basicInfoValid = false;
		return bw == contents.size();
	}

	inline std::wstring GetFilesystemNameW() const
	{
		std::wstring path = m_base;
		if(path.empty()) return L"";
		if(*path.rbegin() != '\\')
			path.push_back('\\');
		LibCC::Blob<wchar_t> temp(path.size() + 1);
		wcscpy(temp.GetBuffer(), path.c_str());
		*PathSkipRootW(temp.GetBuffer()) = 0;

		wchar_t fileSystemName[101] = {0};
		DWORD maxlen, flags;
		GetVolumeInformationW(temp.GetBuffer(), 0, 0, 0, &maxlen, &flags, fileSystemName, 100);
		return fileSystemName;
	}
	inline std::string GetFilesystemNameA() const
	{
		return LibCC::ToANSI(GetFilesystemNameW());
	}

	bool IsNTFS()
	{
		return stricmp("NTFS", GetFilesystemNameA().c_str()) == 0;
	}

	bool IsFAT()
	{
		return LibCC::StringToLower(GetFilesystemNameA()).find("fat") == 0;
	}

	bool DeleteAllChildren()
	{
		std::vector<Path> files = GetFiles(L"*");
		for(std::vector<Path>::iterator it = files.begin(); it != files.end(); ++ it)
		{
			if(!it->Delete())
				return false;
		}
		return true;
	}

	// check modifieddate
	// check size
	// check contents
	bool EqualsExactly(Path& rhs)
	{
		PopulateBasicInfo();
		rhs.PopulateBasicInfo();
		if(!m_exists && !rhs.m_exists)
			return true;
		if(m_exists && !rhs.m_exists)
			return false;
		if(m_isDirectory != rhs.m_isDirectory)
			return false;
		if(m_isFile != rhs.m_isFile)
			return false;
		if(m_size != rhs.m_size)
			return false;
		if(!Equals(m_modifiedDate, rhs.m_modifiedDate))
			return false;
		MD5Result left, right;
		MD5EncodeFile(left, GetPathW());
		MD5EncodeFile(right, rhs.GetPathW());
		return left.Equals(right);
	}

	std::wstring ReadUTF8TextFile()
	{
		HANDLE h = CreateFileW(m_base.c_str(), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if(LibCC::IsBadHandle(h))
			return L"";

		DWORD br = 0;
		LibCC::Blob<char> buf;
		DWORD size = min(100000000, (DWORD)GetSize());
		buf.Alloc((int)size);
		ReadFile(h, buf.GetBuffer(), size, &br, 0);
		CloseHandle(h);

		if(br != size)
			return L"";

		return LibCC::ToUTF16(std::string(buf.GetBuffer(), size), CP_UTF8);
	}

	void SetModifiedDate(const FILETIME& ft)
	{
		HANDLE h = CreateFileW(m_base.c_str(), GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
		if(LibCC::IsBadHandle(h))
			return;

		SetFileTime(h, NULL, NULL, &ft);

		CloseHandle(h);
	}

};


#pragma warning(pop)
