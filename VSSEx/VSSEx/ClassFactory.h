

#include "Guids.h"


class CClassFactory : public IClassFactory
{
public:

    CClassFactory();
    ~CClassFactory();

    //IUnknown methods
    STDMETHODIMP QueryInterface(REFIID, PVOID*);
    STDMETHODIMP_(DWORD) AddRef();
    STDMETHODIMP_(DWORD) Release();

    //IClassFactory methods
    STDMETHODIMP CreateInstance(PUNKNOWN, REFIID, PVOID*);
    STDMETHODIMP LockServer(BOOL);

    void SetCLSID(CLSID* pGuid);

private:

    CLSID clsid;
    DWORD dwRefCount;
};


