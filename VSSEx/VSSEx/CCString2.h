/*

	CityCraft_String:

	010116 Definition finished
	010117 Implementation finished
    010509 Unicode functionality started

*/

#ifndef CCSTRING2_H
#define CCSTRING2_H

#include <windows.h>


// Returns the size, in TCHARS, of the given string.
int GetSizeOfStringA(PCSTR sz);
int GetSizeOfStringW(PCWSTR wsz);

PWSTR StrToWide(PCSTR sz);


//////////////////////////////////////////////////////////////////////////////
class CCString{
public:

    // Constructors
	CCString();
	CCString(PCSTR sz);
	CCString(WCHAR wc);
	CCString(PCWSTR wsz);

    // Copy Constructor
	CCString(const CCString& str);

    // Destructor
	~CCString();

    PWSTR GetBuffer(int nLen);

    // Equality operators
	const CCString& operator = (PCSTR sz);
	const CCString& operator = (PCWSTR wsz);
	const CCString& operator = (CCString& str);
	const CCString& operator += (CCString& str);
	const CCString& operator += (PCWSTR str);
	const CCString& operator += (WCHAR wc);

    // Casting operators
	operator PCSTR();
	operator PCWSTR() const;

    // Index operator
    const WCHAR operator [](int n) const;

    void SetAt(int n, WCHAR wc);

    // Substring - extracting routines
    void Mid(int nStart, int Len, CCString&);
    void Mid(int nStart, CCString&);
    void Left(int n, CCString&);
    void Right(int n, CCString&);

	//File routines:
	HRESULT ReadFromFileA(HANDLE hFile);//Reads a null-terminated string from a file.
	HRESULT ReadFromFileW(HANDLE hFile);//Reads a null-terminated string from a file.
	HRESULT WriteToFileA(HANDLE hFile);
	HRESULT WriteToFileW(HANDLE hFile);

	//String Manipulation
    void  __cdecl FormatA(PCSTR szFormat, ...);
    void  __cdecl FormatW(PCWSTR wszFormat, ...);
    void  FormatvA(PCSTR szFormat, va_list arg);
    void  FormatvW(PCWSTR wszFormat, va_list arg);
	void  cpy(PCWSTR s);//sets 'this' to s
	void  cat(PCWSTR s);
    int   cmp(PCWSTR s);
	int   cmpi(PCWSTR s);
	int   len();
	void  upr();
	void  lwr();
	void  TrimLeft(PCWSTR wszChars);
	void  TrimRight(PCWSTR wszChars);

    void Insert(int nStart, PCWSTR wsz);
    void Insert(int nStart, WCHAR wc);
    void Remove(int nStart, int nLen);
    void Truncate(int nStart);

    void  RemoveAll(WCHAR ch);
    void  Decode(WCHAR ch);
    void  Encode(WCHAR chKey, PCWSTR wszChars);

    // Returns an index
    int   FindNextWord(int nStart);
    int   FindPreviousWord(int nStart);

	int   Find(PCWSTR s, int nStart);
	int   Find(WCHAR wc, int nStart);
	int   FindI(PCWSTR s, int nStart);
	int   FindI(WCHAR wc, int nStart);

	//Path functions
	int   Path_FindExtension();
	int   Path_FindFileName();
    int   Path_FindArgs();

    void  Path_RenameExtension(PCWSTR wsz);
    void  Path_RemoveArgs();
	void  Path_StripToPath();
	void  Path_StripToFilename();
	void  Path_Canonicalize();
	void  Path_AbsoluteToRelative(PCWSTR Root);

    void  Path_Append(PCWSTR);
    void  Path_Combine(PCWSTR);

    BOOL  Path_IsDirectory();
    BOOL  Path_IsRelative();
    BOOL  Path_FileExists();
    BOOL  Path_MatchSpec(PCWSTR wszSpec);

    void  DoEnvironmentSubst();
    void  GetTempFileName();
    void  GetFolderPath(int csidl);
    void  Path_GetCurrentDirectory();
    void  Path_GetTempFileName(PCWSTR wszFileName);
    void  Path_GetTempFileName();

    //Conversion functions
    HRESULT FromGuid(GUID*);
    HRESULT FromNewGuid();
    HRESULT ToGuid(GUID*);
    HRESULT FromInt(int n, int nBase);
    int     ToInt(int nBase);
    HRESULT FromDouble(double d);
    double  ToDouble();
    COLORREF ToColorref();

    // Other stuff
    HRESULT  CopyToClipboard();
    HRESULT  PasteFromClipboard();

    PWSTR bstr();

private:

	//Memory allocation - takes in WCHARs, not bytes.
	int   _SizeGet();
	void  _SizeDelete();//delete the memory
	void  _SizeSetDestructive(int nSize);
	void  _SizeSetPreserve(int nSize);

    // General modifiers... these should be pretty low level
    void  _CreateFromA(PCSTR);
    void  _CreateFromW(PCWSTR);
    void  _CatA(PCSTR);
    void  _CatW(PCWSTR);

    /*
        This holds an ANSI version of the string, only when it's needed.
        (used for casting to an ansi string, etc..)
    */
    PSTR szStringA;
    int nSizeA;
    void _GetAnsiVersion();// Will create it for us.

	PWSTR wszString;
	int nSize;//in WCHARs.

    /*
        This holds the SysAllocString() allocated version of the string...
        To use this, call CCString::BSTRValue().
    */
    PWSTR wszSysAlloc;
};

#endif

