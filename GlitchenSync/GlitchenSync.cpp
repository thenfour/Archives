/*

BUGS:
- actually use a real thread pool for collecting file info
- need to make sure repositories are all completely separate from eachother.
- need to make sure we ignore the glitchensync file database file.
- make sure empty directories are copied
x if a dir cannot be read, fatal error.

optimization strategy:
- say you have a NEW file and a DELETED file in the same dir. they are the same filesize, so go ahead and do the hash check. determine if they're just renamed.
- sort master copies by file date always.

*/
#include "stdafx.h"
#include "md5.hpp"
#include "base64.hpp"
#include "configuration.h"
#include "threadpool.h"
#include "resource.h"
#include <list>
#include "..\libcc\libcc/Log.hpp"
#include "..\libcc\libcc/Timer.hpp"
#include "filedatabase.hpp"
#include "filecollection.hpp"
#include "syncplan.hpp"
#include "Path.hpp"
#include <iostream>

#pragma warning(disable:4996)


extern HINSTANCE g_hInstance;

extern LibCC::Log* gslog;

struct ThreadParams
{
	Subscriber* subscriber;
	class FileDatabase* db;
	RepositoryFileCollection* fc;
};
unsigned __stdcall CollectSourceFileInfoThreadProc(void* params)
{
	Subscriber& s = *((ThreadParams*)params)->subscriber;
	FileDatabase& db = *((ThreadParams*)params)->db;
	RepositoryFileCollection& fc = *((ThreadParams*)params)->fc;
	delete params;

	// we need to collect all information about this subscriber's current file situation.

	bool hasEllipses = false;
	for(std::vector<Subscriber::Filter>::iterator it = s.sourceFilters.begin(); it != s.sourceFilters.end(); ++ it)
	{
		if(it->criteria.SpansSubDirectories())
			hasEllipses = true;
	}

	LibCC::FileIteratorW fi;
	LibCC::RecursiveFileIteratorW rfi;
	LibCC::FileIteratorBaseW& it = hasEllipses ? (LibCC::FileIteratorBaseW&)rfi : (LibCC::FileIteratorBaseW&)fi;

	if(!it.Reset(s.root))
	{
		gslog->Message(LibCC::Format(L"Error initializing recursive iterator for root: %")(s.root));
		return 0;
	}
	LibCC::StdOutPrint(LibCC::Format(L"%Examining %, ").s(gslog->GetIndentStringW()).qs(s.root).Str());
	InitStdOutDisappearingText();
	std::wstring fullPath;
	WIN32_FIND_DATAW fileData;
	int count = 0;
	while(it.Next(fullPath, fileData))
	{
		std::wstring relativePath = s.GetRelativePath(fullPath);
		if(s.PassesSourceFilterCriteria(relativePath))
		{
			count ++;
			if(count % 137 == 0)
				SetStdOutDisappearingText(LibCC::FormatA("% files found")(count).Str());

			fc.GetFile(relativePath).SetBasicFilesystemData(&s, fileData.ftLastWriteTime,
				(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0, MakeUINT64(fileData.nFileSizeLow, fileData.nFileSizeHigh));
		}
	}
	SetStdOutDisappearingText(LibCC::FormatA("% files found|")(count).Str());
	return 1;
}

bool CollectDirectoryInformation(RepositoryFileCollection& fc, FileDatabase& db, SyncScript& script)
{
	ThreadPool pool;

	// fill the file collection with data from the file database.
	FileDatabase::FileIterator it(db);
	while(it.Read())
	{
		MD5Result md5;
		bool md5IsValid = it.MD5IsValid();
		if(md5IsValid)
			md5 = it.MD5();

		FILETIME modifiedDate = it.ModifiedDate();

		fc.GetFile(it.RelativePath()).SetDatabaseInfo(modifiedDate, md5IsValid, md5, it.IsDirectory(), it.Size());
	}

	// now fill the file collection with data from the live filesystem
	for(std::vector<Subscriber>::iterator it = script.subscribers.begin(); it != script.subscribers.end(); ++ it)
	{
		if(it->isSyncSource)
		{
			ThreadParams* params = new ThreadParams();
			params->db = &db;
			params->subscriber = &*it;
			params->fc = &fc;
			pool.AddTask(CollectSourceFileInfoThreadProc, params, LibCC::Format(L"Analyzing: %")(it->root).Str());
		}
	}

	pool.WaitUntilCompletion();

	// check for failure
	for(std::vector<int>::iterator it = pool.m_results.begin(); it != pool.m_results.end(); ++ it)
	{
		if(*it == 0)
		{
			// failure; we're quitting.
			return false;
		}
	}

	return true;
}

bool AnalyzeHistory(RepositoryFileCollection& fc, SyncScript& script, FileDatabase& db, bool verbose)
{
	LibCC::StdOutPrint(LibCC::Format(L"%Analyzing history ").s(gslog->GetIndentStringW()).Str());
	InitStdOutDisappearingText();

	int count = 0;
	// the goal here is to determine which instances of the files have been modified / deleted / added.
	for(RepositoryFileCollection::iterator itFile = fc.begin(); itFile != fc.end(); ++ itFile)
	{
		count ++;
		if((count % 137) == 0)
		{
			SetStdOutDisappearingText(LibCC::FormatA("(%^%)")
				((int)(100.0 * count / fc.GetFileCount()))
				.Str());
		}

		if(fc.CanIgnore(*itFile))
		{
			if(verbose) gslog->Message(LibCC::Format(L"Ignore entirely: File is a subfile of a directory that will be deleted: %").s(itFile->relativePath));
			continue;
		}

		SubscriberData* history = itFile->GetData(0);
		for(std::vector<Subscriber>::iterator itSubscriber = script.subscribers.begin(); itSubscriber != script.subscribers.end(); ++ itSubscriber)
		{
			if(!itSubscriber->isSyncSource)
				continue;

			std::wstring fullPath = itFile->GetFullPath(*itSubscriber);
			if(!itSubscriber->PassesSourceFilterCriteria(itFile->relativePath))
			{
				if(verbose) gslog->Message(LibCC::Format(L"Skipping filtered file: %").s(fullPath));
				continue;
			}

			SubscriberData* fcdata = itFile->GetData(&*itSubscriber);
			// fcdata could be null if the file exists only in other subscribers (on fs or db), and it doesnt exist in either the filesystem or the db for this subscriber.
			// in that case, it doesn't exist in either place, and there is no action to be taken right now. later it would be determined if something needs to be copied.
			if(!history->Exists() && !fcdata->Exists())
			{
				if(verbose) gslog->Message(LibCC::Format(L"Does not exist in either db or fc: %").s(fullPath));
				continue;
			}

			// was it deleted?
			if(history->Exists() && !fcdata->Exists())
			{
				if(!itSubscriber->isSyncDestination)
				{
					// do NOT consider this file deleted if it's not a destination and thus this file was previously NOT copied here.
					if(db.UnexpectedFileExists(itSubscriber->id, itFile->relativePath))
					{
						// no action taken.
						if(verbose) gslog->Message(LibCC::Format(L"File doesn't exist but is in history. However, this subscriber is not a sync destination so it's to be expected; no action to take for: %").s(fullPath));
						continue;
					}
				}

				if(history->IsDirectory())
				{
					fc.AddDeletedDirectoryObserved(&*itFile);
				}
				if(verbose) gslog->Message(LibCC::Format(L"File was deleted (exists in db but not in fs): %").s(fullPath));
				itFile->AddDeletedSource(fcdata);
				continue;
			}

			// is it new?
			if(!history->Exists() && fcdata->Exists())
			{
				if(verbose) gslog->Message(LibCC::Format(L"File is new; considered modified (doesn't exist in db but does on fs): %").s(fullPath));
				itFile->AddModifiedSource(fcdata);
				continue;
			}

			// deduce file / directory conflicts.
			if(!history->IsDirectory() && fcdata->IsDirectory())
			{
				// the file changed to a directory. it will get deleted, and replaced by a dir and all its files.
				if(verbose) gslog->Message(LibCC::Format(L"Changed from a file to a directory: %").s(fullPath));
				fc.AddFileToDeleteFirstObserved(&*itFile);
				itFile->AddChangedToDirectorySource(fcdata);
				continue;
			}
			if(history->IsDirectory() && !fcdata->IsDirectory())
			{
				if(verbose) gslog->Message(LibCC::Format(L"Changed from a directory to a file: %").s(fullPath));
				fc.AddDeletedDirectoryObserved(&*itFile);
				itFile->AddChangedToFileSource(fcdata);
				continue;
			}

			// directories cannot be "modified"
			if(fcdata->isDirectory)
				continue;

			// was it modified?
			// do hash / date comparison
			if(script.comparisonType == SyncScript::MD5)
			{
				if(history->size != fcdata->size)
				{
					if(verbose) gslog->Message(LibCC::Format(L"File size differs between database & filesystem. Marking as modified and skipping hash check: %").s(fullPath));
					itFile->AddModifiedSource(fcdata);
					continue;
				}
				if(!history->MD5IsValid)
				{
					// there is no previus md5. there is no way of knowing then if it's been modified. assume yes.
					if(verbose) gslog->Message(LibCC::Format(L"Database MD5 is missing; file is considered modified: %").s(fullPath));
					itFile->AddModifiedSource(fcdata);
					continue;
				}
				if(!fcdata->MD5IsValid)
				{
					LibCC::Result r = itFile->PopulateMD5(*fcdata, fullPath);
					if(r.Failed())
					{
						if(verbose) gslog->Message(LibCC::Format(L"MD5 hash error; file is considered modified: %").s(fullPath));
						itFile->AddModifiedSource(fcdata);
						continue;
					}
				}
				// we have both md5s - compare them.
				if(!fcdata->MD5.Equals(history->MD5))
				{
					if(verbose) gslog->Message(LibCC::Format(L"File is modified according to MD5: %").s(fullPath));
					itFile->AddModifiedSource(fcdata);
					continue;
				}

				if(verbose) gslog->Message(LibCC::Format(L"File has not changed (did hash check): %").s(fullPath));
				continue;
			}
			if(script.comparisonType == SyncScript::ModifiedDateUTC)
			{
				if(!Equals(history->modifiedDate, fcdata->modifiedDate))
				{
					if(verbose) gslog->Message(LibCC::Format(L"File is modified according to modified date: %").s(fullPath));
					itFile->AddModifiedSource(fcdata);
					continue;
				}
				if(verbose) gslog->Message(LibCC::Format(L"File has not changed (did date check): %").s(fullPath));
				continue;
			}
		}
	}

	SetStdOutDisappearingText(LibCC::FormatA("(%^%)|")(100).Str());
	return true;
}

// the master copy says DELETE! so go through destinations and decide whether they need to be deleted.
void GenerateSyncPlanDeletedFile(RepositoryFileCollection& fc, SyncScript& script, RepositoryFileCollection::iterator& itFile, bool verbose)
{
	for(std::vector<Subscriber>::iterator itDestination = script.subscribers.begin(); itDestination != script.subscribers.end(); ++ itDestination)
	{
		if(itFile->GetMasterCopy()->subscriber == &*itDestination)// do not delete the file if it's the master copy. in the case of a "changed to dir" situation we don't want to delete the directory that we should later copy.
			continue;
		if(!itDestination->isSyncDestination)
			continue;
		if(!itDestination->PassesDestinationFilterCriteria(itFile->relativePath))
			continue;

		std::wstring destFullPath = itFile->GetFullPath(*itDestination);
		SubscriberData* data = itFile->GetData(&*itDestination);// destination info is not necessarily loaded yet.

		itFile->PopulateBasicFilesystemData(*data, destFullPath);
		if(!data->Exists())
			continue;// doesn't exist on the filesystem. fine; this file is deleted anyway.

		// it exists on the filesystem. add an action to delete it.
		if(verbose) gslog->Message(LibCC::Format(L"Action:Delete - %").s(destFullPath));
		fc.AddSyncPlanActionAtTheBeginning(SyncPlanAction::Delete, data);
	}
}

// take a file that EXISTS (modified or not), and create sync plan entries for the destinations that need it
void GenerateSyncPlanModifiedFile(RepositoryFileCollection& fc, SyncScript& script, RepositoryFileCollection::iterator& itFile, const std::wstring& masterFullPath, bool verbose)
{
	for(std::vector<Subscriber>::iterator itDestination = script.subscribers.begin(); itDestination != script.subscribers.end(); ++ itDestination)
	{
		if(!itDestination->isSyncDestination)
			continue;
		if(&*itDestination == itFile->GetMasterCopy()->subscriber)// the master doesn't need action.
			continue;
		if(!itDestination->PassesDestinationFilterCriteria(itFile->relativePath))
			continue;

		std::wstring destFullPath = itFile->GetFullPath(*itDestination);
		SubscriberData* data = itFile->GetData(&*itDestination);

		itFile->PopulateBasicFilesystemData(*data, destFullPath);

		if(!data->Exists())
		{
			if(verbose) gslog->Message(LibCC::Format(L"Action:Restoring missing file from % to %").qs(masterFullPath).qs(destFullPath));
			fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
			continue;
		}

		// it exists in the destination. decide whether it needs to be updated.
		if(script.comparisonType == SyncScript::MD5)
		{
			if(!itFile->PopulateBasicFilesystemData(*data, destFullPath))
			{
				if(verbose) gslog->Message(LibCC::Format(L"** ERROR getting information about %").qs(destFullPath));
				continue;
			}
			if(data->size != itFile->GetMasterCopy()->size)
			{
				if(verbose) gslog->Message(LibCC::Format(L"Size differs; the dest file needs to be clobbered & copied (from % to %)").qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			LibCC::Result r = itFile->PopulateMD5(*itFile->GetMasterCopy(), masterFullPath);
			if(r.Failed())
			{
				if(verbose) gslog->Message(LibCC::Format(L"** Failed to MD5 hash; assuming the file needs to be copied (from % to %)").s(r.str()).qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			r = itFile->PopulateMD5(*data, destFullPath);
			if(r.Failed())
			{
				if(verbose) gslog->Message(LibCC::Format(L"** Failed to MD5 hash; assuming the file needs to be copied (from % to %)").s(r.str()).qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			if(!data->MD5.Equals(itFile->GetMasterCopy()->MD5))
			{
				if(verbose) gslog->Message(LibCC::Format(L"MD5 hash is different. copying from % to %").qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			if(verbose) gslog->Message(LibCC::Format(L"MD5 hash is equal. No action needed for %").qs(destFullPath));
			continue;
		}
		else if(script.comparisonType == SyncScript::ModifiedDateUTC)
		{
			if(itFile->PopulateBasicFilesystemData(*data, destFullPath).Failed())
			{
				if(verbose) gslog->Message(LibCC::Format(L"Get modified date failed - thus, assuming different and copying from % to %").qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			if(!Equals(data->modifiedDate, itFile->GetMasterCopy()->modifiedDate))
			{
				if(verbose) gslog->Message(LibCC::Format(L"Modified dates are different. copying from % to %").qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			// for "changed to file" date check is not enough. one more check is necessary:
			if(Path(masterFullPath).IsDirectory() != Path(destFullPath).IsDirectory())
			{
				if(verbose) gslog->Message(LibCC::Format(L"Need to change from dir to file. copying from % to %").qs(masterFullPath).qs(destFullPath));
				fc.AddSyncPlanAction(SyncPlanAction::Copy, itFile->GetMasterCopy(), data);
				continue;
			}
			if(verbose) gslog->Message(LibCC::Format(L"Modified dates are equal. No action needed for %").qs(destFullPath));
			continue;
		}

	}// itDestination
}

// for the most part, the inputs are the .modifiedSources / .deletedSources / .changedToFile / .changedToDirectory arrays,
// and the output is a sync plan.
bool GenerateSyncPlan(RepositoryFileCollection& fc, SyncScript& script, bool verbose)
{
	LibCC::StdOutPrint(LibCC::Format(L"%Generating a sync plan ").s(gslog->GetIndentStringW()).Str());
	InitStdOutDisappearingText();

	int count = 0;
	for(RepositoryFileCollection::iterator itFile = fc.begin(); itFile != fc.end(); ++ itFile)
	{
		count ++;
		if((count % 137) == 0)
		{
			SetStdOutDisappearingText(LibCC::FormatA("(%^%)")
				((int)(100.0 * count / fc.GetFileCount()))
				.Str());
		}

		if(fc.CanIgnore(*itFile))
		{
			if(verbose) gslog->Message(LibCC::Format(L"Ignore entirely: File is a subfile of a directory that will be deleted: %").s(itFile->relativePath));
			continue;
		}

		if(itFile->changedToDirectory.size() == 0 && itFile->deletedSources.size() == 0 && itFile->GetMasterCopy() != 0 && itFile->GetMasterCopy()->subscriber == 0)
		{
			// the master copy is the history instance and the file has not been deleted. nothing has been modified.
			continue;
		}

		// check for simple conflicts
		if(itFile->deletedSources.size() > 0 && itFile->modifiedSources.size() > 0)
		{
			if(verbose) gslog->Message(LibCC::Format(L"Conflict: File has both deleted & modified states: %").s(itFile->relativePath));
			fc.AddSyncPlanActionAtTheBeginning(SyncPlanAction::Conflicted, itFile->relativePath);
			continue;
		}
		if(itFile->modifiedSources.size() > 0 && itFile->changedToDirectory.size() > 0)
		{
			if(verbose) gslog->Message(LibCC::Format(L"Conflict: File has both changed-to-directory & modified states: %").s(itFile->relativePath));
			fc.AddSyncPlanActionAtTheBeginning(SyncPlanAction::Conflicted, itFile->relativePath);
			continue;
		}

		// if it's deleted, then just make sure all destinations get it deleted too.
		if(itFile->deletedSources.size() > 0)
		{
			GenerateSyncPlanDeletedFile(fc, script, itFile, verbose);
			continue;
		}

		// check for file contents conflicts between multiple sources
		_ASSERT(0 != itFile->GetMasterCopy());
		std::wstring masterFullPath = itFile->GetFullPath(*itFile->GetMasterCopy()->subscriber);

		if(itFile->modifiedSources.size() > 0 && !itFile->GetMasterCopy()->IsDirectory())
		{
			bool hasDifferent = false;
			for(std::vector<SubscriberData* >::iterator it = itFile->modifiedSources.begin(); it != itFile->modifiedSources.end(); ++ it)
			{
				SubscriberData* data = *it;
				if(itFile->GetMasterCopy() == data)
					continue;

				if(script.comparisonType == SyncScript::MD5)
				{
					if(data->size != itFile->GetMasterCopy()->size)
					{
						if(verbose) gslog->Message(LibCC::Format(L"Conflict: File has different file sizes (no need for hash check): %").s(itFile->relativePath));
						hasDifferent = true;
						break;
					}

					if(itFile->PopulateMD5(*data, itFile->GetFullPath(*data->subscriber)).Failed())
						continue;

					if(!data->MD5.Equals(itFile->GetMasterCopy()->MD5))
					{
						if(verbose) gslog->Message(LibCC::Format(L"Conflict: File has different hashes: %").s(itFile->relativePath));
						hasDifferent = true;
						break;
					}
				}
				else if(script.comparisonType == SyncScript::ModifiedDateUTC)// Do the same for modified date case.
				{
					if(itFile->PopulateBasicFilesystemData(*data, itFile->GetFullPath(*data->subscriber)).Failed())
						continue;
					if(Equals(data->modifiedDate, itFile->GetMasterCopy()->modifiedDate))
					{
						if(data->size != itFile->GetMasterCopy()->size)
						{
							// rare case
							if(verbose) gslog->Message(LibCC::Format(L"Conflict: File has different file sizes but the same modified date. %").s(itFile->relativePath));
							hasDifferent = true;
							break;
						}
						break;
					}
				}
			}
			if(hasDifferent)
			{
				fc.AddSyncPlanActionAtTheBeginning(SyncPlanAction::Conflicted, itFile->relativePath);
				continue;
			}
		}

		// for changedToDirectory and changedToFile, there are two operations: a delete operation (which must be placed BEFORE anything), and an addition.
		if(itFile->changedToFile.size() > 0 || itFile->changedToDirectory.size() > 0)
		{
			GenerateSyncPlanDeletedFile(fc, script, itFile, verbose);// delete the existing directory
			GenerateSyncPlanModifiedFile(fc, script, itFile, masterFullPath, verbose);// and add the new file.
			continue;
		}

		// OK we have a single existing master file (modified or not). check if the file needs to be copied to destinations
		GenerateSyncPlanModifiedFile(fc, script, itFile, masterFullPath, verbose);

	}// itFile

	SetStdOutDisappearingText(LibCC::FormatA("(%^%)|")(100).Str());
	return true;
}


bool ExecuteSyncPlan(RepositoryFileCollection& fc, bool verbose)
{
	bool ret = true;
	LibCC::LogScopeMessage l(L"ExecuteSyncPlan", gslog);
	for(std::list<SyncPlanAction>::iterator itAction = fc.syncPlan.begin(); itAction != fc.syncPlan.end(); ++ itAction)
	{
		switch(itAction->action)
		{
		default:
			gslog->Message(L"ERROR: *** Unknown sync action; no action taken.");
			ret = false;
			itAction->result.Fail();
			break;
		case SyncPlanAction::Conflicted:
			gslog->Message(L"ERROR: *** Conflicted file - cannot take action.");
			ret = false;
			itAction->result.Fail();
			break;
		case SyncPlanAction::Ignore:
			break;
		case SyncPlanAction::Copy:
			{
				std::wstring left = itAction->from->owner->GetFullPath(*itAction->from->subscriber);
				std::wstring right = itAction->to->owner->GetFullPath(*itAction->to->subscriber);
				if(!Path(left).CopyTo(Path(right)))
				{
					itAction->result.Fail();
					gslog->Message(LibCC::Format(L"ERROR copying from % to %").qs(left).qs(right));
					ret = false;
				}
				else
				{
					itAction->result.Succeed();
					fc.action_Copied ++;
					gslog->Message(LibCC::Format(L"Copied from % to %").qs(left).qs(right));
				}
				break;
			}
		case SyncPlanAction::Delete:
			{
				std::wstring fullPath = itAction->from->owner->GetFullPath(*itAction->from->subscriber);
				if(!Path(fullPath).Delete())
				{
					itAction->result.Fail();
					gslog->Message(LibCC::Format(L"ERROR deleting file %").qs(fullPath));
					ret = false;
				}
				else
				{
					itAction->result.Succeed();
					fc.action_Deleted ++;
					gslog->Message(LibCC::Format(L"Deleted file %").qs(fullPath));
				}
				break;
			}
		}
	}
	return true;
}


bool UpdateFileDatabase(RepositoryFileCollection& fc, SyncScript& script, FileDatabase& db, bool verbose)
{
	// if we made it this far, then we want to update the database with what should be. ASSUME THERE ARE NO REMAINING CONFLICTS.
	// iterate through all the files and for each one that is modified, make the appropriate change in the db.
	LibCC::StdOutPrint(LibCC::Format(L"%Updating history database ").s(gslog->GetIndentStringW()).Str());
	InitStdOutDisappearingText();

	int count = 0;
	for(RepositoryFileCollection::iterator itFile = fc.begin(); itFile != fc.end(); ++ itFile)
	{
		count ++;
		if((count % 137) == 0)
		{
			SetStdOutDisappearingText(LibCC::FormatA("(%^%)")
				((int)(100.0 * count / fc.GetFileCount()))
				.Str());
		}

		if(itFile->deletedSources.size() > 0)
		{
			db.DeleteFile_(itFile->relativePath);
			continue;
		}

		SubscriberData* masterCopy = itFile->GetMasterCopy();

		if(masterCopy->subscriber == 0)
		{
			// ignoring; we're looking at the original history which means it's unmodified from how it already is in the database.
			continue;
		}

		if(verbose) gslog->Message(LibCC::Format(L"Updating file db: path:% isdir:% md5valid:% md5:% date:% size:%")
			.qs(itFile->relativePath)
			.s(masterCopy->IsDirectory() ? L"1" : L"0")
			.s(masterCopy->MD5IsValid ? L"1" : L"0")
			.s(masterCopy->MD5.ToString())
			.ul(masterCopy->modifiedDate.dwLowDateTime)
			(masterCopy->size));


		if(!db.UpdateFile(itFile->relativePath, masterCopy->IsDirectory(), masterCopy->MD5IsValid, masterCopy->MD5, masterCopy->modifiedDate, masterCopy->size))
		{
			gslog->Message(LibCC::Format(L"Error updating database file: %").qs(itFile->relativePath));
		}
	}

	for(std::vector<Subscriber>::iterator itSubscriber = script.subscribers.begin(); itSubscriber != script.subscribers.end(); ++ itSubscriber)
	{
		for(RepositoryFileCollection::iterator itFile = fc.begin(); itFile != fc.end(); ++ itFile)
		{
			SubscriberData* fcdata = itFile->GetData(&*itSubscriber);

			// if it exists on the filesystem, make sure that it's not in UnexpectedFiles database table for ANY subscribers.
			if(fcdata->Exists())
			{
				if(verbose) gslog->Message(LibCC::Format(L"Removing unexpected file for subscriber: %, path: %").qs(itSubscriber->id).qs(itFile->relativePath));
				db.DeleteUnexpectedFile(itSubscriber->id, itFile->relativePath);
				continue;
			}

			// for sources that are not destinations, if it did not exist on the fs before the sync, then don't expect it there now
			// or any time in the future.
			if(!fcdata->Exists() && !itSubscriber->isSyncDestination && itSubscriber->isSyncSource)
			{
				if(verbose) gslog->Message(LibCC::Format(L"Adding unexpected file for subscriber: %, path: %").qs(itSubscriber->id).qs(itFile->relativePath));
				db.AddUnexpectedFile(itSubscriber->id, itFile->relativePath);
				continue;
			}
		}
	}

	SetStdOutDisappearingText(LibCC::FormatA("(%^%)|")(100).Str());
	return true;
}



bool RunSyncScript(ConfigFile& configFile, SyncScript& script, RepositoryFileCollection& fc, bool verbose)
{
	LibCC::LogScopeMessageTimer l(L"RunSyncScript", gslog);

	if(script.comparisonType == SyncScript::SHA1)
	{
		gslog->Message(L"ERROR: *** SHA1 NOT SUPPORTED");
		return false;
	}

	// check for:
	// - correct formatting of root names (just because it's convenient to do now, and would be a waste to continue)
	// - if we're going to be checking modified date, then make sure we're on a filesystem that will be accurate about it (NTFS only)
	// - existant and unique subscriber names
	for(std::vector<Subscriber>::iterator itSubscriber = script.subscribers.begin(); itSubscriber != script.subscribers.end(); ++ itSubscriber)
	{
		if(itSubscriber->id.empty())
		{
			gslog->Message(LibCC::Format(L"All subscribers need an ID. The subscriber for root path % did not.")
				.qs(itSubscriber->root));
			return false;
		}

		for(std::vector<Subscriber>::iterator it2 = script.subscribers.begin(); it2 != script.subscribers.end(); ++ it2)
		{
			if(&*it2 == &*itSubscriber)// don't care about the same one.
				continue;
			if(it2->id == itSubscriber->id)
			{
				gslog->Message(LibCC::Format(L"All subscribers need a unique ID. The subscriber % for root paths % and % are not.")
					.qs(itSubscriber->id)
					.qs(itSubscriber->root)
					.qs(it2->root)
					);
				return false;
			}
		}

		if(TRUE == PathIsRelative(itSubscriber->root.c_str()))
		{
			gslog->Message(LibCC::Format(L"The subscriber % needs to be an absolute path, not relative.").qs(itSubscriber->id));
			return false;
		}

		bool isNTFS = Path(itSubscriber->root).IsNTFS();
		itSubscriber->modifiedDatesAreAccurate = isNTFS;
		if(script.comparisonType == SyncScript::ModifiedDateUTC && !isNTFS)
		{
			gslog->Message(LibCC::Format(L"Only NTFS filesystems are supported for comparing file dates. The subscriber % is not.")
				.qs(itSubscriber->id));
			return false;
		}
	}

	// open the file database
	std::wstring databaseFilename = script.fileDB;
	if(databaseFilename.empty())
	{
		databaseFilename = configFile.fileName;
		databaseFilename.append(L".db3");
	}
	else
	{
		if(PathIsRelative(script.fileDB.c_str()))
		{
			databaseFilename = LibCC::PathRemoveFilename(configFile.fileName);
			databaseFilename = LibCC::PathJoin(databaseFilename, script.fileDB);
		}
	}
	std::wstring schemaSQL = LoadTextFileResource(g_hInstance, MAKEINTRESOURCE(IDR_TEXT1), L"TEXT");
	FileDatabase db(databaseFilename, schemaSQL);

	// gather basic information about all relevant files
	if(!CollectDirectoryInformation(fc, db, script))
		return false;

	// fill in file history to get a first look at what happened to each file (for each file, populate DeletedSources / ModifiedSources / etc)
	AnalyzeHistory(fc, script, db, verbose);

	if(fc.afc_deletedSources > 0)
		gslog->Message(LibCC::Format(L"  % file instances deleted")(fc.afc_deletedSources));
	if(fc.afc_modifiedSources > 0)
		gslog->Message(LibCC::Format(L"  % file instances modified")(fc.afc_modifiedSources));
	if(fc.afc_changedToFile > 0)
		gslog->Message(LibCC::Format(L"  % file instances changed from directory to file")(fc.afc_changedToFile));
	if(fc.afc_changedToDirectory > 0)
		gslog->Message(LibCC::Format(L"  % file instances changed from file to directory")(fc.afc_changedToDirectory));

	// convert file status to an action plan
	if(!GenerateSyncPlan(fc, script, verbose))
		return false;

	// TODO: user verify & modify sync plan

	int conflicts = 0;
	for(std::list<SyncPlanAction>::iterator itAction = fc.syncPlan.begin(); itAction != fc.syncPlan.end(); ++ itAction)
	{
		if(itAction->action == SyncPlanAction::Conflicted)
		{
			gslog->Message(LibCC::Format("Conflicted file: ").qs(itAction->path));
			conflicts ++;
		}
	}

	// TODO: conflict resolution hinting

	//if(conflicts > 0)
	//{
	//	gslog->Message(LibCC::Format("% conflicts found. canceling synchronization.")(conflicts));
	//	return false;
	//}

	if(!ExecuteSyncPlan(fc, verbose))
		return false;

	if(!UpdateFileDatabase(fc, script, db, verbose))
		return false;

	for(std::list<SyncPlanAction>::iterator itAction = fc.syncPlan.begin(); itAction != fc.syncPlan.end(); ++ itAction)
	{
		if(itAction->action == SyncPlanAction::Conflicted)
		{
			gslog->Message(LibCC::Format("No action taken for conflicted file: ").qs(itAction->path));
		}
	}
	return true;
}


bool RunSyncScript(ConfigFile& configFile, SyncScript& script, bool verbose)
{
	RepositoryFileCollection fc;
	return RunSyncScript(configFile, script, fc, verbose);
}

