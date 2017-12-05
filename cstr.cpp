#include "cstr.h"

CStr::CStr(LPSTR){
  this->m_str = std::string(LPSTR);
}

CStr::CStr(std::string){
}

CStr::~CStr(){
}

CStr CStr::operator +(LPTSTR){
}

CStr CStr::operator +(std::string){
}

CStr CStr::operator +(CStr&){
}

LPTSTR CStr::operator +(LPTSTR){
}

LPTSTR CStr::operator +(std::string){
}

LPTSTR CStr::operator +(CStr&){
}

