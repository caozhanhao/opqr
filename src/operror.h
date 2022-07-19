#pragma once
#include <stdexcept>
#include <string>

#define OP_STRINGFY(x) _OP_STRINGFY(x)
#define _OP_STRINGFY(x) #x
#define OP_ERROR_LOCATION  __FILE__ ":" OP_STRINGFY(__LINE__) 
namespace op::error
{
  class Error : public std::logic_error
  {
  public:
    Error(std::string location, std::string func_name, std::string details)
      : logic_error("In File: " + location + ":" + func_name + "(): \n" + details)
    {}
  };
}