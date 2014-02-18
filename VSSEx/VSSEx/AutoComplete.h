

#include <Windows.h>
#include <shlobj.h>
#include <Shldisp.h>


class CVSSEnumString : public IEnumString
{
public:

    CVSSEnumString();
    ~CVSSEnumString();

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID, PVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    // IEnumString methods
    STDMETHODIMP Next(ULONG celt, LPOLESTR* rgelt, ULONG* pceltFetched);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(IEnumString ** ppenum);

private:

    HRESULT _EnsureKeyOpen();

    DWORD dwRefCount;

    int nIndex;
    HKEY hKey;

};

