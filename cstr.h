#include <string>

typedef (char*) LPTSTR

class CStr {
public:
  CStr(LPSTR);
  CStr(std::string);
  ~CStr();
  CStr operator +(LPTSTR);
  CStr operator +(std::string);
  CStr operator +(CStr&);
  LPTSTR operator +(LPTSTR);
  LPTSTR operator +(std::string);
  LPTSTR operator +(CStr&);
private:
  std::string m_str;
};
