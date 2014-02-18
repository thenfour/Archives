
#pragma once


#include "..\libcc\libcc/Log.hpp"
#include "md5.hpp"
#include "utility.hpp"
#include <set>

struct SubscriberData
{
	explicit SubscriberData(Subscriber* s) :
		subscriber(s),
		basicInfoIsValid(false),
		isDirectory(false),
		fileExists(false),
		MD5IsValid(false),
		size(0),
		owner(0)
	{
	}

	bool operator < (const SubscriberData& rhs) const
	{
		return subscriber < rhs.subscriber;
	}

	Subscriber* subscriber;// this is 0 if this is info from the database
	struct RepositoryFile* owner;

	bool basicInfoIsValid:1;
	bool isDirectory:1;// basic info
	bool fileExists:1;// basic info
	bool MD5IsValid:1;

	UINT64 size;// basic info
	FILETIME modifiedDate;// basic info
	MD5Result MD5;

	bool Exists() const
	{
		return basicInfoIsValid && fileExists;
	}

	bool IsDirectory() const
	{
		return basicInfoIsValid && isDirectory;
	}
};

struct AnalysisFileCounts
{
	AnalysisFileCounts() :
		afc_modifiedSources(0),
		afc_deletedSources(0),
		afc_changedToFile(0),
		afc_changedToDirectory(0),
		action_Copied(0),
		action_Deleted(0)
	{
	}

	int afc_modifiedSources;
	int afc_deletedSources;
	int afc_changedToFile;
	int afc_changedToDirectory;
	int action_Copied;
	int action_Deleted;

	// possible other metrics to gather:
	// - bytes read
	// - bytes written
	// - database rows written
	// - database rows read
	// - files touched
	// - hashes performed
};

struct RepositoryFile
{
private:
	std::set<SubscriberData> data;

	SubscriberData* m_masterCopy;// master copy is a copy that we can copy from.

public:
	std::wstring relativePath;
	bool operator < (const RepositoryFile& rhs) const
	{
		return relativePath < rhs.relativePath;
	}

	std::vector<SubscriberData* > modifiedSources;// list of subscribers that modified this file (populated during phase 2)
	std::vector<SubscriberData* > deletedSources;// list of subscribers that deleted this file (populated during phase 3)
	std::vector<SubscriberData* > changedToFile;// list of subscribers that deleted the directory and added a file in its place
	std::vector<SubscriberData* > changedToDirectory;// list of subscribers that deleted the file and added a directory in its place

	AnalysisFileCounts* counts;

	// only valid if history analysis is done.
	SubscriberData* GetMasterCopy()
	{
		if(0 != m_masterCopy)
			return m_masterCopy;
		if(modifiedSources.size() > 0)
		{
			// master copy should be the latest modified one.
			m_masterCopy = modifiedSources.front();
			for(std::vector<SubscriberData* >::iterator it = modifiedSources.begin(); it != modifiedSources.end(); ++ it)
			{
				SubscriberData* p = *it;
				if(p->basicInfoIsValid)
				{
					if(!m_masterCopy->basicInfoIsValid)
						m_masterCopy = p;
					else
					{
						if(LessThan(m_masterCopy->modifiedDate, p->modifiedDate))
						{
							m_masterCopy = p;
						}
					}
				}
			}
		}
		else if(changedToFile.size() > 0)
		{
			m_masterCopy = changedToFile.front();
		}
		else if(changedToFile.size() > 0)
		{
			m_masterCopy = changedToFile.front();
		}
		else if(changedToDirectory.size() > 0)
		{
			m_masterCopy = changedToDirectory.front();
		}
		else if(data.size())// nothing changed; use anything.
		{
			m_masterCopy = &*data.begin();
		}
		return m_masterCopy;
	}

	typedef std::set<SubscriberData>::iterator iterator;
	iterator begin() { return data.begin(); }
	iterator end() { return data.end(); }

	explicit RepositoryFile(const std::wstring& fileName) :
		relativePath(fileName),
		m_masterCopy(0),
		counts(0)
	{
	}

	void AddDeletedSource(SubscriberData* data)
	{
		counts->afc_deletedSources ++;
		deletedSources.push_back(data);
	}

	void AddModifiedSource(SubscriberData* data)
	{
		counts->afc_modifiedSources ++;
		modifiedSources.push_back(data);
	}

	void AddChangedToFileSource(SubscriberData* data)
	{
		counts->afc_changedToFile ++;
		changedToFile.push_back(data);
	}

	void AddChangedToDirectorySource(SubscriberData* data)
	{
		counts->afc_changedToDirectory ++;
		changedToDirectory.push_back(data);
	}

	SubscriberData* GetData(Subscriber* key)
	{
		SubscriberData* ret = &(*data.insert(SubscriberData(key)).first);
		ret->owner = this;
		_ASSERT(ret != 0);
		return ret;
	}

	std::wstring GetFullPath(const Subscriber& s)
	{
		return LibCC::PathAppendX(s.root, relativePath);
	}

	LibCC::Result PopulateBasicFilesystemData(SubscriberData& data, const std::wstring& fullPath)
	{
		if(data.basicInfoIsValid)
			return LibCC::Result::Success();

		data.basicInfoIsValid = true;
		data.fileExists = false;

		WIN32_FILE_ATTRIBUTE_DATA fileData;
		if(0 == GetFileAttributesEx(fullPath.c_str(), GetFileExInfoStandard, &fileData))
		{
			// file probably just doesn't exist
			return LibCC::Result::Failure();
		}

		data.fileExists = true;
		data.isDirectory = 0 != (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		data.size = MakeUINT64(fileData.nFileSizeLow, fileData.nFileSizeHigh);
		data.modifiedDate = fileData.ftLastWriteTime;

		return LibCC::Result::Success();
	}

	LibCC::Result PopulateMD5(SubscriberData& data, const std::wstring& fullPath)
	{
		if(data.MD5IsValid)
			return LibCC::Result::Success();

		PopulateBasicFilesystemData(data, fullPath);

		if(!data.fileExists)
			return LibCC::Result::Success();
		if(data.IsDirectory())
			return LibCC::Result::Failure(LibCC::Format(L"Cannot hash directory: ").s(fullPath).Str());

		//log->Message(LibCC::Format(L"Hashing: %").s(fullPath));
		LibCC::Result r = MD5EncodeFile(data.MD5, fullPath);
		data.MD5IsValid = r.Succeeded();
		if(r.Failed())
		{
			//log->Message(LibCC::Format(L"** Failed to MD5 hash: ").s(r.str()));
		}
		else
		{
			//log->Message(LibCC::Format(L"Hash result: ").s(r.str()));
		}
		return r;
	}

	void SetBasicFilesystemData(Subscriber* s, const FILETIME& modifiedDate, bool isDirectory, UINT64 size)
	{
		_ASSERT(data.find(SubscriberData(s)) == data.end());
		SubscriberData* d = GetData(s);

		d->basicInfoIsValid = true;
		d->fileExists = true;
		d->isDirectory = isDirectory;
		d->modifiedDate = modifiedDate;
		d->size = size;
	}

	void SetDatabaseInfo(const FILETIME& modifiedDate, bool md5IsValid, const MD5Result& md5, bool isDirectory, UINT64 size)
	{
		SubscriberData* d = GetData(0);
		d->basicInfoIsValid = true;
		d->fileExists = true;
		d->isDirectory = isDirectory;
		d->modifiedDate = modifiedDate;
		d->MD5IsValid = md5IsValid;
		d->MD5 = md5;
		d->size = size;
	}
};

struct SyncPlanAction
{
	enum Action
	{
		Copy,// from / to are used
		Delete,// from is used
		Conflicted,// from is used
		Ignore
	};
	Action action;
	SubscriberData* from;
	SubscriberData* to;
	std::wstring path;
	LibCC::Result result;
};

// keyed collection of all files known in the repository. the reason for this is mostly to keep a single list of
// all the files relevant to developing a sync plan. it should be able to contain file info for each subscriber.
class RepositoryFileCollection :
	public AnalysisFileCounts
{
private:
	std::set<RepositoryFile> m_items;// keyed by lowercase filename, relative to roots.

public:
	typedef std::set<RepositoryFile>::iterator iterator;
	typedef std::set<RepositoryFile>::const_iterator const_iterator;
	iterator begin() { return m_items.begin(); }
	iterator end() { return m_items.end(); }
	size_t GetFileCount() const { return m_items.size(); }

	// populated by AnalyzeHistory to keep track of directories that get clobbered entirely. helps
	// determine where to skip over things.
	// NOTE that if we let users resolve conflicts then this skipping might bite us in the ass.
	struct DeletedDirectoryInfo
	{
		explicit DeletedDirectoryInfo(RepositoryFile* s) :
			m_file(s)
		{
			m_p4.SetCriteria(LibCC::PathAppendX(s->relativePath, L"..."));
		}
		
		bool operator <(const DeletedDirectoryInfo& s) const { return m_file < s.m_file; }
		
		bool IsASubFile(RepositoryFile* rhs)
		{
			return m_p4.Match(rhs->relativePath);
		}
		LibCC::PathMatchSpecP4 m_p4;
		RepositoryFile* m_file;
	};

	std::set<DeletedDirectoryInfo> deletedDirectoriesObserved;
	void AddDeletedDirectoryObserved(RepositoryFile* f)
	{
		deletedDirectoriesObserved.insert(DeletedDirectoryInfo(f));

		// retroactively ignore stuff.
		for(iterator it = begin(); it != end(); ++ it)
		{
			if(CanIgnore(*it))
			{
				// remove all history observations
				it->changedToDirectory.clear();
				it->changedToFile.clear();
				it->deletedSources.clear();
				it->modifiedSources.clear();
			}
		}
	}
	std::set<std::wstring> filesToDeleteFirstObserved;
	void AddFileToDeleteFirstObserved(RepositoryFile* f)
	{
		filesToDeleteFirstObserved.insert(f->relativePath);
	}

	// returns true if this file can be flat out ignored because it will be deleted due to deletedDirectoriesObserved
	bool CanIgnore(RepositoryFile& rhs)
	{
		for(std::set<DeletedDirectoryInfo>::iterator it = deletedDirectoriesObserved.begin(); it != deletedDirectoriesObserved.end(); ++ it)
		{
			if(it->IsASubFile(&rhs))
				return true;
		}
		return false;
	}

	std::list<SyncPlanAction> syncPlan;// list of subscribers that deleted this file (populated during phase 3)
	void AddSyncPlanAction(SyncPlanAction::Action action, SubscriberData* from = 0, SubscriberData* to = 0)
	{
		SyncPlanAction n;
		n.action = action;
		n.from = from;
		n.to = to;
		syncPlan.push_back(n);
	}
	void AddSyncPlanActionAtTheBeginning(SyncPlanAction::Action action, const std::wstring& path)
	{
		SyncPlanAction n;
		n.action = action;
		n.from = 0;
		n.to = 0;
		n.path = path;
		syncPlan.push_front(n);
	}

	void AddSyncPlanActionAtTheBeginning(SyncPlanAction::Action action, SubscriberData* from)
	{
		SyncPlanAction n;
		n.action = action;
		n.from = from;
		n.to = 0;
		syncPlan.push_front(n);
	}

	bool FileExists(const std::wstring& relativePath)
	{
		std::set<RepositoryFile>::iterator it = m_items.find(RepositoryFile(LibCC::StringToLower(relativePath)));
		return it != m_items.end();
	}

	RepositoryFile& GetFile(const std::wstring& relativePath)
	{
		std::wstring lower = LibCC::StringToLower(relativePath);
		std::pair<std::set<RepositoryFile>::iterator, bool> res = m_items.insert(RepositoryFile(lower));
		res.first->counts = this;
		return *res.first;
	}
};

