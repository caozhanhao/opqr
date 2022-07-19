#pragma once
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
    :data(std::move(data_)){}

    void paint(std::ofstream& fs)
    {
      fs << "P3\n" << data.size() << " " << data[0].size() << "\n255\n";
      for (int i = data.size() - 1; i >= 0; --i)
      {
        for (int j = 0; j < data[i].size(); ++j)
        {
          int ir = 0;
          int ig = 0;
          int ib = 0;
          if(!data[j][i])
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