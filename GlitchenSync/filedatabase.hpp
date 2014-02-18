

#pragma once


#include <string>
#include "sqlite\sqlite3x.hpp"
#include <windows.h>
#include "..\libcc\libcc\stringutil.hpp"
#include "md5.hpp"
#include "utility.hpp"



class FileDatabase
{
	friend class Iterator;

public:
	FileDatabase(const std::wstring& dbFileName, const std::wstring& schemaSQL);
	~FileDatabase();

	struct FileIterator
	{
		FileIterator(FileDatabase& container);
		FileIterator(FileDatabase& container, const std::wstring& fileQuery);
		~FileIterator();
		bool Read();

		std::wstring RelativePath();
		bool IsDirectory();
		bool MD5IsValid();
		MD5Result MD5();
		FILETIME ModifiedDate();
		UINT64 Size();

	private:
		bool m_isReady;
		sqlite3x::sqlite3_reader m_reader;
		sqlite3x::sqlite3_command* m_cmd;
		sqlite3x::sqlite3_connection m_conn;
	};

private:
	std::wstring m_schemaVersion;
	std::wstring GetDatabaseSchemaVersion();

	std::wstring m_dbFileName;
	std::wstring m_schemaSQL;

	bool OpenDatabase(sqlite3x::sqlite3_connection&);

public:
	std::wstring GetFileName() const { return m_dbFileName; }
	bool UpdateFile(const std::wstring& relativePath, bool isDirectory, bool md5IsValid, const MD5Result& md5, const FILETIME& modifiedDate, UINT64 size);
	bool DeleteFile_(const std::wstring& relativePath);

	bool DeleteUnexpectedFile(const std::wstring& subscriberID, const std::wstring& relativePath);
	bool AddUnexpectedFile(const std::wstring& subscriberID, const std::wstring& relativePath);
	bool UnexpectedFileExists(const std::wstring& subscriberID, const std::wstring& relativePath);
};


inline std::wstring QuoteSql(const std::wstring& in)
{
	std::wstring ret;
	ret.reserve(in.length());
	for(std::wstring::const_iterator it = in.begin(); it != in.end(); ++ it)
	{
		if(*it == L'\'')
		{
			ret.push_back(L'\'');
			ret.push_back(L'\'');
		}
		else
		{
			ret.push_back(*it);
		}
	}
	return ret;
}
