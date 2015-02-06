
#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

std::wstring widen(const char* str);
std::wstring widen(const std::string& str);
std::string narrow(const wchar_t* str);
std::string narrow(const std::wstring& str);    
std::string get_last_error();

void show_message_box(const std::string& caption, const std::string& text);

#endif
