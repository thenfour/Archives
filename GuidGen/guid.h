
#pragma once


#include <string>


class Guid
{
public:
  GUID val;

  Guid()
  {
    memset(&val, 0, sizeof(val));
  }
  Guid(const Guid& x) { Assign(x); }
  Guid(const GUID& x) { Assign(x); }

  Guid& operator =(const Guid& x) { return Assign(x); }
  Guid& operator =(const GUID& x) { Assign(x); }
  Guid& operator =(const std::string& x) { Assign(x); }

  bool operator ==(const Guid& x) const  { return Equals(x); }
  bool operator !=(const Guid& x) const  { return !Equals(x); }
  Guid& Assign(const std::string& x)
  {
    OLECHAR tempW[45];
    if(x[0] != '{')
    {
      tempW[0] = '{';
      LibCC::StringCopyN(tempW+1, x.c_str(), 39);
      tempW[37] = '}';
      tempW[38] = 0;
    }
    else
    {
      LibCC::StringCopyN(tempW, x.c_str(), 39);
    }
    CLSIDFromString(tempW, &val);
    return *this;
  }
  Guid& Assign(const Guid& x)
  {
    memcpy(&val, &x.val, sizeof(GUID));
    return *this;
  }
  Guid& Assign(const GUID& x)
  {
    memcpy(&val, &x, sizeof(GUID));
    return *this;
  }
  bool Equals(const Guid& x) const
  {
    return (memcmp(&x.val, &val, sizeof(GUID)) == 0);
  }
  void CreateNew()
  {
    CoCreateGuid(&val);
  }
  std::string ToString() const
  {
    LPOLESTR p;
    StringFromCLSID(val, &p);
    std::string ret;
    LibCC::StringCopyN(ret, p+1, 36);
    CoTaskMemFree(p);
    return ret;
  }

};