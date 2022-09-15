//   Copyright 2022 opqr - caozhanhao
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
#ifndef OPQR_OPERROR_HPP
#define OPQR_OPERROR_HPP

#include <stdexcept>
#include <string>

#define OPQR_STRINGFY(x) _OPQR_STRINGFY(x)
#define _OPQR_STRINGFY(x) #x
#define OPQR_ERROR_LOCATION  __FILE__ ":" OPQR_STRINGFY(__LINE__)
namespace opqr::error
{
  class Error : public std::runtime_error
  {
  public:
    Error(std::string location, std::string func_name, std::string details)
      : std::runtime_error("\n\033[31mError\033[0m : In File: " + location +
        ":\033[33m" + func_name + "\033[0m(): \n\033[37m" + details + "\033[0m") {}
  };
}
#endif