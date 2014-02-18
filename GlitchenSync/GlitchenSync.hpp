

#pragma once


#include "configuration.h"
#include "filecollection.hpp"


bool RunSyncScript(ConfigFile& configFile, SyncScript& script, bool verbose);
bool RunSyncScript(ConfigFile& configFile, SyncScript& script, RepositoryFileCollection& fc, bool verbose);

