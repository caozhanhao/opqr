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
#ifndef OPQR_OPPIC_HPP
#define OPQR_OPPIC_HPP
#include <vector>
#include <fstream>
namespace opqr::pic
{
  class Pic
  {
  private:
    std::vector<std::vector<bool>> data;
  public:
    Pic(std::vector<std::vector<bool>> data_)
        : data(std::move(data_)) {}
  
    void paint(std::ofstream &fs, std::size_t size)
    {
      fs << "P1\n" << data.size() * size << " " << data[0].size() * size << "\n";
      for (int i = data.size() - 1; i >= 0; --i)
      {
        for (int l = 0; l < size; ++l)
        {
          for (int j = 0; j < data[i].size(); ++j)
          {
            for (int k = 0; k < size; ++k)
            {
              fs << data[j][i] << "\n";
            }
          }
          fs << "\n";
        }
      }
      fs.close();
    }
  };
}
#endif