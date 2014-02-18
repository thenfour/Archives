/*
    Criteria:   "c*cor?n"
    String:     "carl corcoran"

    Criteria list:
        Literal("c")
        Star
        Literal("cor")
        Question
        Literal("n")
        End-of-line

    Criteria Types:
        Literal - compares the exact given string to the current position, if it
                  matches, then continue with the next criteria list item.
        Star - advance the cursor until the next criteria list item matches.  If a
               next-file-part is found, no match.
        Question - advance the cursor by 1 blindly *so long as there's one char left*.
        EndOfLine - match only if there are no more chars
        Ellipse - match

    The string will match the criteria if the criteria is found at position 0.
*/

#pragma once


#include <string>
#include <vector>
#include <stack>


#define P4_IS_BAD_HANDLE(h) (((h) == 0) || ((h) == INVALID_HANDLE_VALUE))
#define P4_IS_VALID_HANDLE(h) (!(((h) == 0) || ((h) == INVALID_HANDLE_VALUE)))


#define BEGIN_CRITERIA_TYPE(name, type) \
    class name : public CCriteriaTypeBase \
    { \
    public: \
        name(){ m_Type = type; }\
        bool Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s);

#define END_CRITERIA_TYPE() };


enum CriteriaType
{
    CT_Star,
    CT_Question,
    CT_Literal,
    CT_Ellipses,
    CT_EndOfLine,
};


class CCriteriaTypeBase
{
public:
    typedef std::vector<CCriteriaTypeBase*> CriteriaList_T;
    virtual ~CCriteriaTypeBase(){}
    virtual bool Match(CriteriaList_T& cl, CriteriaList_T::iterator itCriteria, const std::string& s) = 0;
    CriteriaType m_Type;
};


BEGIN_CRITERIA_TYPE(CStarType, CT_Star);
END_CRITERIA_TYPE();

BEGIN_CRITERIA_TYPE(CQuestionType, CT_Question);
END_CRITERIA_TYPE();

BEGIN_CRITERIA_TYPE(CEllipsesType, CT_Ellipses);
END_CRITERIA_TYPE();

BEGIN_CRITERIA_TYPE(CEndOfLineType, CT_EndOfLine);
END_CRITERIA_TYPE();

BEGIN_CRITERIA_TYPE(CLiteralType, CT_Literal);
    std::string m_s;
END_CRITERIA_TYPE();


class CP4PathMatchSpec
{
public:
    CP4PathMatchSpec();
    ~CP4PathMatchSpec();

    void SetCriteria(const std::string& s);
    bool Match(const std::string& s);
    void DumpCriteria();

    bool SpansSubDirectories();// convenience function so we don't always have to search sub directories.
    bool GetRootDir(std::string& s);// returns a path that can be searched for files to match teh criteria.

    ///////////////////////////////////////////////////////////////////////////
    static bool IteratorPointsToEllipses(std::string::const_iterator& it, const std::string& s);
    static bool IsPathSeparator(const std::string::iterator& it, const std::string& s);
    static bool IsPathSeparator(const std::string::const_iterator& it, const std::string& s);
    static bool IsPathSeparator(const std::string::value_type& c);
    static std::string PathJoin(const std::string& dir, const std::string& file);
    static std::string PathRemoveFilename(const std::string& path);
    static bool PathIsAbsolute(const std::string& path);
    static std::string GetCurrentDir();

private:

    void ClearCriteria();

    typedef std::vector<CCriteriaTypeBase::CriteriaList_T> CriteriaListList_T;// list of criteria lists (for semicolon-separated dealies)
    typedef CCriteriaTypeBase::CriteriaList_T CriteriaList_T;// list of criteria lists (for semicolon-separated dealies)
    CriteriaListList_T m_CriteriaLists;

    bool SpansSubDirectories(CriteriaList_T* pCriteria);
    void SetCriteria(CriteriaList_T* pCriteria, const std::string& s);
    CriteriaList_T* GetNewCriteriaList();
};


#include <windows.h>


class CFileIteratorBase
{
public:
    virtual bool Reset(const std::string& sDir) = 0;
    virtual bool Next(std::string& sFullPath, DWORD* pdwAttributes) = 0;
};


// class for iterating through a single directory.
class CFileIterator : public CFileIteratorBase
{
public:
    CFileIterator();
    ~CFileIterator();

    bool Reset(const std::string& sDir);
    bool Next(std::string& sFullPath, DWORD* pdwAttributes = 0);

private:
    bool IsUsableFileName(const WIN32_FIND_DATA* pfd);
    bool IsDirectory(const WIN32_FIND_DATA* pfd);

    void _Free();

    std::string m_BaseDir;// directory for this find
    HANDLE m_hFind;
    WIN32_FIND_DATA m_fd;
};


class CRecursiveFileIterator : public CFileIteratorBase
{
public:
    CRecursiveFileIterator();
    ~CRecursiveFileIterator();

    bool Reset(const std::string& sDir);
    bool Next(std::string& sFullPath, DWORD* pdwAttributes = 0);

private:
    std::stack<CFileIterator> m_stack;
    void _Free();
};
