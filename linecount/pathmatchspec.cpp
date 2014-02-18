


#include "pathmatchspec.h"
#include <iostream>



CP4PathMatchSpec::CP4PathMatchSpec()
{
}



CP4PathMatchSpec::~CP4PathMatchSpec()
{
}


bool CP4PathMatchSpec::Match(const std::string& s)
{
    bool r = false;
    CriteriaListList_T::iterator it;
    CriteriaList_T* pCriteriaItem = 0;

    for(it=m_CriteriaLists.begin();it!=m_CriteriaLists.end();it++)
    {
        pCriteriaItem = &(*it);
        if(!pCriteriaItem->empty())// empty criteria == no match.
        {
            CCriteriaTypeBase::CriteriaList_T::iterator itCriteria = pCriteriaItem->begin();
            r = pCriteriaItem->front()->Match(*pCriteriaItem, itCriteria, s);
        }

        if(r) break;// if we find a match, no reason to keep searching.
    }
    return r;
}


void CP4PathMatchSpec::SetCriteria(const std::string& s)
{
    ClearCriteria();

    // find the first segment (separated by semicolons)
    std::string::size_type nCurrentSemicolon = 0;
    std::string::size_type nPreviousSemicolon = 0;

    /*
        check out this criteria:   c:\\*code\\*.h;*.cpp
        here, I have to extract c:\\*code from the first segment
        and append that to every non-absolute segment thereafter.
    */
    std::string sDirectory;
    std::string sThisSegment;

    while(1)
    {
        if(nCurrentSemicolon)
        {
            nPreviousSemicolon = nCurrentSemicolon+1;// only advance if it's actually on a semicolon, not if it's just at position 0
        }
        else
        {
            nPreviousSemicolon = nCurrentSemicolon;
        }

        nCurrentSemicolon = s.find(';', nPreviousSemicolon);// find the next semicolon.
        if(nCurrentSemicolon != std::string::npos)
        {
            sThisSegment = s.substr(nPreviousSemicolon, nCurrentSemicolon - nPreviousSemicolon);

            if(sDirectory.empty())
            {
                if(PathIsAbsolute(sThisSegment))
                {
                    // this thing is a full path and we don't have  adirectory; create it
                    sDirectory = PathRemoveFilename(sThisSegment);
                }
            }
            else
            {
                if(!PathIsAbsolute(sThisSegment))
                {
                    sThisSegment = PathJoin(sDirectory, sThisSegment);
                }
            }

            SetCriteria(GetNewCriteriaList(), sThisSegment);
        }
        else
        {
            // this is the last segment; no semicolon was found.
            sThisSegment = s.substr(nPreviousSemicolon);

            if(!sDirectory.empty())
            {
                if(!PathIsAbsolute(sThisSegment))
                {
                    sThisSegment = PathJoin(sDirectory, sThisSegment);
                }
            }

            SetCriteria(GetNewCriteriaList(), sThisSegment);

            break;
        }
    }

    return;
}


CP4PathMatchSpec::CriteriaList_T* CP4PathMatchSpec::GetNewCriteriaList()
{
    m_CriteriaLists.push_back(CriteriaList_T());
    return &m_CriteriaLists.back();
}


void CP4PathMatchSpec::SetCriteria(CriteriaList_T* pCriteria, const std::string& s)
{
    std::string::const_iterator it;
    std::string::value_type c;

    for(it=s.begin();it!=s.end();it++)
    {
        c = *it;

        if(c == '*')
        {
            pCriteria->push_back(new CStarType);
        }
        else if(c == '?')
        {
            pCriteria->push_back(new CQuestionType);
        }
        else if(IteratorPointsToEllipses(it, s))
        {
            pCriteria->push_back(new CEllipsesType);
        }
        else
        {
            CLiteralType* pCurrentLiteral = 0;

            // see if we need to push_Back a new literal engine.
            if(pCriteria->empty() || (pCriteria->back()->m_Type != CT_Literal))
            {
                pCurrentLiteral = new CLiteralType;
                pCriteria->push_back(pCurrentLiteral);
            }
            else
            {
                CCriteriaTypeBase* p = (pCriteria->back());
                pCurrentLiteral = (CLiteralType*)p;
            }

            pCurrentLiteral->m_s.push_back(c);
        }
    }

    pCriteria->push_back(new CEndOfLineType);

    return;
}


/*
    This thing will return true if you need to search sub-directories
    in order to match everything.  In other words, take this criteria:
    c:\\*.*

    There is no way anything will match it unless it's in the root directory.

    c:\\*\\*.txt
    - Return true if there is something other than a literal BEFORE the last path separator
    - If there are ellipses at all.
*/
bool CP4PathMatchSpec::SpansSubDirectories()
{
    bool r = false;
    CriteriaListList_T::iterator it;
    for(it=m_CriteriaLists.begin();it!=m_CriteriaLists.end();it++)
    {
        r = SpansSubDirectories(&(*it));
        if(r) break;
    }
    return r;
}


bool CP4PathMatchSpec::SpansSubDirectories(CriteriaList_T* pCriteria)
{
    bool r = false;
    CriteriaList_T::reverse_iterator it;

    bool bHaveHitPathSep = false;

    for(it=pCriteria->rbegin();it!=pCriteria->rend();it++)
    {
        CCriteriaTypeBase* pBase = *it;
        switch(pBase->m_Type)
        {
        case CT_Ellipses:
            r = true;
            break;
        case CT_Star:
            if(bHaveHitPathSep) r = true;
            break;
        case CT_Question:
            if(bHaveHitPathSep) r = true;
            break;
        case CT_Literal:
            {
                CLiteralType* plt = (CLiteralType*)pBase;
                // search for path separator.
                if(std::string::npos != plt->m_s.find_first_of("\\/"))
                {
                    bHaveHitPathSep = true;
                }
                break;
            }
        }

        // if we satisfy the need-sub-dirs criteria, then exit immediately.
        if(r) break;
    }

    return r;
}


bool CP4PathMatchSpec::GetRootDir(std::string& s)
{
    bool r = false;
    CriteriaList_T* pFirstList = &(m_CriteriaLists.front());
    CCriteriaTypeBase* pFirstCriteria = pFirstList->front();

    if(pFirstCriteria->m_Type == CT_Literal)
    {
        CLiteralType* pLiteral = (CLiteralType*)pFirstCriteria;
        s = PathRemoveFilename(pLiteral->m_s);
    }
    r = true;

    return r;
}


bool CP4PathMatchSpec::IsPathSeparator(const std::string::const_iterator& it, const std::string& s)
{
    bool r = false;
    if(it != s.end())
    {
        r = IsPathSeparator(*it);
    }
    return r;
}


bool CP4PathMatchSpec::IsPathSeparator(const std::string::iterator& it, const std::string& s)
{
    bool r = false;
    if(it != s.end())
    {
        r = IsPathSeparator(*it);
    }
    return r;
}


bool CP4PathMatchSpec::IsPathSeparator(const std::string::value_type& c)
{
    switch(c)
    {
    case '\\': return true;
    case '/': return true;
    }

    return false;
}


std::string CP4PathMatchSpec::GetCurrentDir()
{
    std::string r;
    char temp[2] = {0};
    int nRequiredSize = GetCurrentDirectory(2, temp);
    char* s = new char[nRequiredSize+2];
    GetCurrentDirectory(nRequiredSize+1, s);
    r = s;
    delete [] s;
    return r;
}


std::string CP4PathMatchSpec::PathRemoveFilename(const std::string& path)
{
    std::string::size_type nLastSlash = 0;
    nLastSlash = path.find_last_of("\\/");
    if(nLastSlash == std::string::npos) return path;

    return path.substr(0, nLastSlash);
}


// must contain :/ or :\\ at position 2.  so the string must also be at least 3 chars long
bool CP4PathMatchSpec::PathIsAbsolute(const std::string& path)
{
    bool r = false;
    if(path.length() >= 3)
    {
        std::string sSearch = path.substr(1, 2);
        if((sSearch == ":/") || (sSearch == ":\\"))
        {
            r = true;
        }
    }
    return r;
}


std::string CP4PathMatchSpec::PathJoin(const std::string& dir, const std::string& file)
{
    if(dir.empty()) return file;
    std::string::value_type c = *(dir.rbegin());

    switch(c)
    {
    case '\\':
    case '/':
        return dir + file;
        break;
    }

    return dir + "\\" + file;
}


// if the thing does point to ellipses, it will end up at the last character of the ellipses.
bool CP4PathMatchSpec::IteratorPointsToEllipses(std::string::const_iterator& it, const std::string& s)
{
    bool r = false;

    if(it != s.end())
    {
        if((*it) == '.')
        {
            // we have the first one.
            it++;
            if(it != s.end())
            {
                if((*it) == '.')
                {
                    // we have the second one.
                    it++;
                    if(it != s.end())
                    {
                        if((*it) == '.')
                        {
                            // we have the third one.
                            r = true;
                        }
                        else
                        {
                            it -= 2;
                        }
                    }
                    else
                    {
                        it -= 2;
                    }
                }
                else
                {
                    it --;
                }
            }
            else
            {
                it --;
            }
        }
    }

    return r;
}


void CP4PathMatchSpec::ClearCriteria()
{
    CriteriaListList_T::iterator itLists;
    CCriteriaTypeBase::CriteriaList_T::iterator it;
    CCriteriaTypeBase::CriteriaList_T* pThisList = 0;


    for(itLists=m_CriteriaLists.begin();itLists!=m_CriteriaLists.end();itLists++)
    {
        pThisList = &(*itLists);
        for(it=pThisList->begin();it!=pThisList->end();it++)
        {
            delete (*it);
        }

        pThisList->clear();
    }

    m_CriteriaLists.clear();

    return;
}


void CP4PathMatchSpec::DumpCriteria()
{
    std::cout << "Criteria list" << std::endl;

    CriteriaListList_T::iterator itLists;
    CCriteriaTypeBase::CriteriaList_T::iterator it;
    CCriteriaTypeBase::CriteriaList_T* pThisList = 0;

    for(itLists=m_CriteriaLists.begin();itLists!=m_CriteriaLists.end();itLists++)
    {
        pThisList = &(*itLists);
        for(it=pThisList->begin();it!=pThisList->end();it++)
        {
            switch((*it)->m_Type)
            {
            case CT_Star:
                std::cout << "    Star" << std::endl;
                break;
            case CT_Question:
                std::cout << "    Question" << std::endl;
                break;
            case CT_Ellipses:
                std::cout << "    Ellipse" << std::endl;
                break;
            case CT_EndOfLine:
                std::cout << "    EndOfLine" << std::endl;
                break;
            case CT_Literal:
                std::cout << "    Literal('" << ((CLiteralType*)*it)->m_s << "')" << std::endl;
                break;
            }
        }
    }

    return;
}


// here, we will start at the current position and just perform the rest of the criteria.
// if we encounter a path separator in our quest, no match.
bool CStarType::Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s)
{
    bool r = false;
    itCriteria++;
    std::string::const_iterator itsCriteria = s.begin();
    std::string::size_type pos = 0;
    std::string sTheRest;

    while(1)
    {
        // generate a string that represents the rest of the string
        sTheRest = s.substr(pos);

        if((*itCriteria)->Match(cl, itCriteria, sTheRest))
        {
            // this is the only way to cause a match.
            r = true;
            break;
        }

        // make sure we didn't encounter a path separator.
        if(CP4PathMatchSpec::IsPathSeparator(itsCriteria, s)) break;

        if(itsCriteria == s.end()) break;// no match.

        itsCriteria ++;
        pos ++;
    }

    return r;
}


bool CEllipsesType::Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s)
{
    bool r = false;
    itCriteria++;
    std::string::const_iterator itsCriteria = s.begin();
    std::string::size_type pos = 0;
    std::string sTheRest;

    while(1)
    {
        // generate a string that represents the rest of the string
        sTheRest = s.substr(pos);

        if((*itCriteria)->Match(cl, itCriteria, sTheRest))
        {
            // this is the only way to cause a match.
            r = true;
            break;
        }

        if(itsCriteria == s.end()) break;// no match.

        itsCriteria ++;
        pos ++;
    }

    return r;
}


bool CQuestionType::Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s)
{
    bool r = false;

    if(!s.empty())
    {
        std::string sTheRest = s.substr(1);
        itCriteria++;
        CCriteriaTypeBase* pType = *itCriteria;

        r = pType->Match(cl, itCriteria, sTheRest);
    }

    return r;
}


bool CEndOfLineType::Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s)
{
    return (s.empty());
}


// if our literal string matches the beginning of the given string, we're set, and move on to the
// next criteria items in the list.  Otherwise, just return false.
// special attention must also be paid to path separators.
// if criteria is "test/blarg" and the string is "test\\blarg" then a match should be made
bool CLiteralType::Match(CriteriaList_T& cl, CriteriaList_T::iterator _itCriteria, const std::string& s)
{
    bool r = false;

    if(s.length() >= m_s.length())
    {
        bool bMatch = false;
        std::string::const_iterator itCriteria;
        std::string::const_iterator itComparison;
        std::string::value_type cCriteria;
        std::string::value_type cComparison;

        itCriteria = m_s.begin();
        itComparison = s.begin();

        while(1)
        {
            if(itCriteria == m_s.end())
            {
                // If we hit the end of the criteria string that means everything matched
                // so far; MATCH and BREAK
                bMatch = true;
                break;
            }

            if(itComparison == s.end())
            {
                // if we hit the end of the comparison string, no match.
                break;
            }

            cCriteria = tolower(*itCriteria);
            cComparison = tolower(*itComparison);

            if(CP4PathMatchSpec::IsPathSeparator(cComparison))
            {
                // both must be path separators
                if(!CP4PathMatchSpec::IsPathSeparator(cCriteria)) break;
            }
            else
            {
                // we hit a place where they are different.
                if(cCriteria != cComparison) break;
            }

            itCriteria ++;
            itComparison ++;
        }

        if(bMatch)
        {
            _itCriteria++;
            std::string sTheRest;
            sTheRest = s.substr(m_s.length());
            r = (*_itCriteria)->Match(cl, _itCriteria, sTheRest);
        }
    }
    return r;
}


CFileIterator::CFileIterator() :
    m_hFind(0)
{
}


CFileIterator::~CFileIterator()
{
    _Free();
}


void CFileIterator::_Free()
{
    if(P4_IS_VALID_HANDLE(m_hFind)) FindClose(m_hFind);
    m_hFind = 0;
}


bool CFileIterator::Reset(const std::string& sDir)
{
    bool r = false;

    _Free();

    std::string sQuery;
    sQuery = CP4PathMatchSpec::PathJoin(sDir, "*");

    m_BaseDir = sDir;
    m_hFind = FindFirstFile(sQuery.c_str(), &m_fd);

    if(P4_IS_BAD_HANDLE(m_hFind))
    {
        // reset back to invalid.
        m_hFind = 0;
        m_BaseDir.clear();
    }
    else
    {
        r = true;
    }

    return r;
}


bool CFileIterator::Next(std::string& sFullPath, DWORD* pdwAttributes)
{
    bool r = false;

    if(P4_IS_VALID_HANDLE(m_hFind))
    {
        while(1)
        {
            if(IsUsableFileName(&m_fd))
            {
                // use it!
                sFullPath = CP4PathMatchSpec::PathJoin(m_BaseDir, m_fd.cFileName);
                if(pdwAttributes) *pdwAttributes = m_fd.dwFileAttributes;
                r = true;

                // advance to the next one so we're queued up next call.
                if(0 == FindNextFile(m_hFind, &m_fd))
                {
                    FindClose(m_hFind);
                    m_hFind = 0;
                }
                break;
            }

            // otherwise, proceed to the next one.
            if(0 == FindNextFile(m_hFind, &m_fd))
            {
                // failure; make sure we clean everything up and return false.
                FindClose(m_hFind);
                m_hFind = 0;
                break;
            }
        }
    }

    return r;
}


// only unused filenames are "." and ".."
bool CFileIterator::IsUsableFileName(const WIN32_FIND_DATA* pfd)
{
    bool r = true;
    if(pfd->cFileName[0])
    {
        if(pfd->cFileName[0] == '.')
        {
            if(pfd->cFileName[1] == 0)
            {
                // it equals "."
                r = false;
            }
            else
            {
                if(pfd->cFileName[1] == '.')
                {
                    if(pfd->cFileName[2] == 0) r = false;// it equals ".."
                }
            }
        }
    }
    return r;
}


CRecursiveFileIterator::CRecursiveFileIterator()
{
}


CRecursiveFileIterator::~CRecursiveFileIterator()
{
    _Free();
}


void CRecursiveFileIterator::_Free()
{
    while(m_stack.size())
    {
        m_stack.pop();
    }
}


bool CRecursiveFileIterator::Reset(const std::string& sDir)
{
    bool r = false;
    m_stack.push(CFileIterator());
    CFileIterator& it = m_stack.top();
    if(it.Reset(sDir))
    {
        r = true;
    }
    else
    {
        m_stack.pop();
    }

    return r;
}


bool CRecursiveFileIterator::Next(std::string& sFullPath, DWORD* pdwAttributes)
{
    bool r = false;
    DWORD dwAttr = 0;

    while(m_stack.size())
    {
        CFileIterator& it = m_stack.top();
        if(it.Next(sFullPath, &dwAttr))
        {
            if(pdwAttributes) *pdwAttributes = dwAttr;
            if(dwAttr & FILE_ATTRIBUTE_DIRECTORY)
            {
                // push an enum to this thing.
                m_stack.push(CFileIterator());
                CFileIterator& it = m_stack.top();
                it.Reset(sFullPath);
            }

            r = true;
            break;
        }
        else
        {
            m_stack.pop();
        }
    }

    return r;
}



