

#pragma once

#include "testframework.hpp"
#include "path.hpp"
#include "GlitchenSync.hpp"
#include "automatedTests.hpp"


inline bool operator ==(const FILETIME& lhs, const FILETIME& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(lhs)) == 0;
}

void GlitchenSyncTest(const std::wstring& _ntfsSandbox, const std::wstring& _fatSandbox, bool verbose);

