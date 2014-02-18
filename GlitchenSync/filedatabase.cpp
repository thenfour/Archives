#include "StdAfx.h"
#include "filedatabase.hpp"
#include "resource.h"
#include "base64.hpp"
#include "md5.hpp"
#include "..\libcc\libcc\log.hpp"
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

extern LibCC::Log* gslog;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileDatabase::FileIterator::FileIterator(FileDatabase& container) :
	m_cmd(0),
	m_isReady(false)
{
	try
	{
		if(!container.OpenDatabase(m_conn))
			return;

		m_cmd = new sqlite3x::sqlite3_command(m_conn, "select [relativePath], [MD5], [modifiedDate], [isDirectory], [size] from Files");
		m_reader = sqlite3x::sqlite3_reader(m_cmd->executereader());
		m_isReady = true;
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileIterator::FileIterator(). %").qs(er.what()));
	}
}

FileDatabase::FileIterator::FileIterator(FileDatabase& container, const std::wstring& fileQuery) :
	m_cmd(0),
	m_isReady(false)
{
	try
	{
		if(!container.OpenDatabase(m_conn))
			return;

		m_cmd = new sqlite3x::sqlite3_command(m_conn, "select [relativePath], [MD5], [modifiedDate], [isDirectory], [size] from Files where relativePath like ?");
		m_cmd->bind(1, fileQuery);
		m_reader = sqlite3x::sqlite3_reader(m_cmd->executereader());
		m_isReady = true;
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileIterator::FileIterator(). %").qs(er.what()));
	}
}

FileDatabase::FileIterator::~FileIterator()
{
	m_reader.close();
	delete m_cmd;
	m_conn.close();
}

// returns true if there is a value now ready to read with GetCurrentValue().
// you have to call this BEFORE GetCurrentValue.
bool FileDatabase::FileIterator::Read()
{
	if(!m_isReady)
		return false;

	if(m_reader.read())
		return true;

	m_isReady = false;
	return false;
}

std::wstring FileDatabase::FileIterator::RelativePath()
{
	if(!m_isReady)
		return L"";
	return m_reader.getstring16(0);
}

bool FileDatabase::FileIterator::MD5IsValid()
{
	if(!m_isReady)
		return false;
	return !m_reader.isNull(1);
}

MD5Result FileDatabase::FileIterator::MD5()
{
	MD5Result ret;
	if(!m_isReady)
		return ret;
	std::string temp = m_reader.getblob(1);
	if(temp.size() != 16)
		return ret;
	memcpy(&ret.checksum, temp.c_str(), 16);
	return ret;
}

FILETIME FileDatabase::FileIterator::ModifiedDate()
{
	FILETIME ret = {0};
	if(!m_isReady)
		return ret;
	std::string temp = m_reader.getblob(2);
	memcpy(&ret, temp.c_str(), temp.size());
	return ret;
}

bool FileDatabase::FileIterator::IsDirectory()
{
	if(!m_isReady)
		return false;
	return 0 != m_reader.getint(3);
}

UINT64 FileDatabase::FileIterator::Size()
{
	UINT64 ret = 0;
	if(!m_isReady)
		return ret;
	std::string temp = m_reader.getblob(4);
	memcpy(&ret, temp.c_str(), temp.size());
	return ret;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FileDatabase::FileDatabase(const std::wstring& dbFileName, const std::wstring& schemaSQL) :
	m_dbFileName(dbFileName),
	m_schemaSQL(schemaSQL)
{
}

FileDatabase::~FileDatabase()
{
}

std::wstring FileDatabase::GetDatabaseSchemaVersion()
{
	if(!m_schemaVersion.empty())
		return m_schemaVersion;

	// the schema version is just a MD5 of the original schema create script. so when the schema changes,
	// the database knows to re-create.
	std::string schemaTextA = LibCC::ToANSI(m_schemaSQL);
	MD5Result md5 = MD5Encode(schemaTextA.c_str(), schemaTextA.length());

	return Base64Encode<wchar_t>(md5.checksum, sizeof(md5.checksum));
}


bool FileDatabase::UpdateFile(const std::wstring& relativePath, bool isDirectory, bool md5IsValid, const MD5Result& md5, const FILETIME& modifiedDate, UINT64 size)
{
	try
	{
		sqlite3x::sqlite3_connection conn;
		if(!OpenDatabase(conn))
			return false;

		{
			sqlite3x::sqlite3_command cmd(conn, L"delete from Files where relativePath like ?");
			cmd.bind(1, LibCC::Format(L"%^%")(relativePath).Str());
			cmd.executenonquery();
		}

		{
			sqlite3x::sqlite3_command cmd(conn, LibCC::Format(L"insert into Files ([relativePath], [MD5], [modifiedDate], [isDirectory], [size]) "
				L"values (?, ?, ?, %, ?)")(isDirectory ? 1 : 0).Str());
			cmd.bind(1, relativePath);
			if(!md5IsValid)
			{
				cmd.bind(2);
			}
			else
			{
				cmd.bind(2, (const void*)md5.checksum, 16);
			}
			cmd.bind(3, (const void*)&modifiedDate, sizeof(modifiedDate));
			cmd.bind(4, &size, sizeof(size));
			cmd.executenonquery();
		}

		conn.close();
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileIterator::FileIterator(). %").qs(er.what()));
	}
	return true;
}

bool FileDatabase::DeleteFile_(const std::wstring& relativePath)
{
	try
	{
		sqlite3x::sqlite3_connection conn;
		if(!OpenDatabase(conn))
			return false;

		{
			sqlite3x::sqlite3_command cmd(conn, L"delete from Files where relativePath = ?");
			cmd.bind(1, relativePath);
			cmd.executenonquery();
		}

		conn.close();
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileIterator::FileIterator(). %").qs(er.what()));
	}
	return true;
}

bool FileDatabase::DeleteUnexpectedFile(const std::wstring& subscriberID, const std::wstring& relativePath)
{
	try
	{
		sqlite3x::sqlite3_connection conn;
		if(!OpenDatabase(conn))
			return false;

		{
			sqlite3x::sqlite3_command cmd(conn, L"delete from UnexpectedFiles where [subscriberID] = ? and [relativePath] = ?");
			cmd.bind(1, subscriberID);
			cmd.bind(2, relativePath);
			cmd.executenonquery();
		}

		conn.close();
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileDatabase::DeleteUnexpectedFile(). %").qs(er.what()));
	}
	return true;
}

bool FileDatabase::AddUnexpectedFile(const std::wstring& subscriberID, const std::wstring& relativePath)
{
	try
	{
		sqlite3x::sqlite3_connection conn;
		if(!OpenDatabase(conn))
			return false;

		{
			if(!DeleteUnexpectedFile(subscriberID, relativePath))
				return false;
		}

		{
			sqlite3x::sqlite3_command cmd(conn, L"insert into UnexpectedFiles ([subscriberID], [relativePath]) values (?, ?)");
			cmd.bind(1, subscriberID);
			cmd.bind(2, relativePath);
			cmd.executenonquery();
		}

		conn.close();
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileDatabase::AddUnexpectedFile(). %").qs(er.what()));
	}
	return true;
}


bool FileDatabase::UnexpectedFileExists(const std::wstring& subscriberID, const std::wstring& relativePath)
{
	bool r = false;
	try
	{
		sqlite3x::sqlite3_connection conn;
		if(!OpenDatabase(conn))
			return false;

		{
			sqlite3x::sqlite3_command cmd(conn, "select 0 from UnexpectedFiles where [subscriberID] = ? and [relativePath] = ?");
			cmd.bind(1, subscriberID);
			cmd.bind(2, relativePath);
			sqlite3x::sqlite3_reader reader(cmd.executereader());
			r = reader.read();
			reader.close();
		}
		conn.close();
	}
	catch(sqlite3x::database_error& er)
	{
		gslog->Message(LibCC::Format("Database exception in FileDatabase::UnexpectedFileExists(). %").qs(er.what()));
	}
	return r;
}



bool FileDatabase::OpenDatabase(sqlite3x::sqlite3_connection& out)
{
	bool openSuccessful = false;
	bool wrongVersion = false;

	if(PathFileExists(m_dbFileName.c_str()))
	{
		try
		{
			out.open(m_dbFileName.c_str());
			std::wstring cur;

			{// necessary for cmd to cleanup properly
				sqlite3x::sqlite3_command cmd(out, "select [stringValue] from Settings where [Name] like 'SchemaVersion'");
				cur = cmd.executestring16();
			}

			if(cur == GetDatabaseSchemaVersion())
			{
				openSuccessful = true;
			}
			else
			{
				out.close();
				if(IDCANCEL == MessageBox(0, L"The file database format has changed since the last time you ran this program. Click OK to delete the existing database and re-examine directories.", L"Screenie", MB_ICONINFORMATION | MB_OKCANCEL))
				{
					return false;
				}
				gslog->Message(LibCC::Format("Database % is the wrong version (it's %, but I expected %)").qs(m_dbFileName)(cur)(GetDatabaseSchemaVersion()));
				wrongVersion = true;
			}
		}
		catch(sqlite3x::database_error& er)
		{
			// no big deal; the database probable doesn't exist.
			gslog->Message(LibCC::Format("Database exception while opening %: %; trying to open a database.").qs(m_dbFileName).qs(er.what()));
		}
	}

	if(!openSuccessful)
	{
		try
		{
			out.close();
			if(0 == DeleteFile(m_dbFileName.c_str()))
			{
				if(wrongVersion && (GetLastError() == ERROR_SHARING_VIOLATION))
				{
					while(true)
					{
						if(IDCANCEL == MessageBox(0, LibCC::Format("Cannot delete the database at % because it is in use by another program. Please close any program that is using the file and click Retry.")(m_dbFileName).CStr(),L"Screenie", MB_ICONERROR | MB_RETRYCANCEL))
						{
							return false;
						}
						if(0 != DeleteFile(m_dbFileName.c_str()))
							break;// success.					
					}
				}
			}
			// create schema
			std::vector<std::wstring> commands;
			LibCC::StringSplitByString(m_schemaSQL, L";", std::back_inserter(commands));

			out.open(m_dbFileName.c_str());
			for(std::vector<std::wstring>::iterator it = commands.begin(); it != commands.end(); ++ it)
			{
				std::wstring q = LibCC::StringTrim(*it, L"\r\n \t");
				if(q.length())
					out.executenonquery(q);
			}

			out.executenonquery(LibCC::Format("insert into Settings ([Name], [stringValue]) values ('SchemaVersion', '%')")(GetDatabaseSchemaVersion()).Str());
			openSuccessful = true;
		}
		catch(sqlite3x::database_error& er)
		{
			// no big deal; the database probably doesn't exist.
			gslog->Message(LibCC::Format("Error while setting up new database (%).").qs(er.what()));
		}
	}

	if(!openSuccessful)
	{
		return false;
	}

	// no idea why this is here.
	sqlite3_enable_shared_cache(1);
	return true;
}


