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
namespace op::pic
{
  class Pic
  {
  private:
    std::vector<std::vector<bool>> data;
  public:
    Pic(std::vector<std::vector<bool>> data_)
        : data(std::move(data_)) {}
    
    void paint(std::ofstream &fs)
    {
      fs << "P3\n" << data.size() << " " << data[0].size() << "\n255\n";
      for (int i = data.size() - 1; i >= 0; --i)
      {
        for (int j = 0; j < data[i].size(); ++j)
        {
          int ir = 0;
          int ig = 0;
          int ib = 0;
          if (!data[j][i])
          {
            ir = 255;
            ig = 255;
            ib = 255;
          }
          fs << ir << " " << ig << " " << ib << "\n";
        }
        fs << "\n";
      }
      fs.close();
    }
  };
}
#endif