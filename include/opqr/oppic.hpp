//   Copyright 2022 - 2023 opqr - caozhanhao
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
#include "operror.hpp"
#include <vector>
#include <fstream>
namespace opqr::pic
{
  enum class Format
  {
    BMP, PPM
  };

  class Pic
  {
  private:
    std::vector<std::vector<bool>> data;

  public:
    Pic(std::vector<std::vector<bool>> data_)
      : data(std::move(data_)) {}

    void paint(Format fmt, std::string path, std::size_t size = 1)
    {
      std::ofstream fs(std::move(path));
      paint(fmt, fs, size);
    }
    void paint(Format fmt, std::ofstream &fs, std::size_t size = 1)
    {
      if (!fs.good())
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Failed reading file.");
      switch (fmt)
      {
      case Format::BMP:
        paint_bmp(fs, size);
        break;
      case Format::PPM:
        paint_ppm(fs, size);
        break;
      }
    }

  private:
    void paint_bmp(std::ofstream &fs, std::size_t size)
    {
      union LITTLE
      {
        uint8_t bytes[4];
        uint32_t value;
      };
  
      LITTLE l_width, l_height, l_bfSize, l_biSizeImage;
      uint32_t width = static_cast<uint32_t>(data.size() * size);
      uint32_t height = static_cast<uint32_t>(data.size() * size);
  
      uint32_t width_r = (width * 24 / 8 + 3) / 4 * 4;
      uint32_t bf_size = width_r * height + 54 + 2;
      uint32_t bi_size_image = width_r * height;
  
      l_width.value = width;
      l_height.value = height;
      l_bfSize.value = bf_size;
      l_biSizeImage.value = bi_size_image;

      std::array<unsigned char, 54> header
      {
          0x42, 0x4d,
          l_bfSize.bytes[0], l_bfSize.bytes[1], l_bfSize.bytes[2], l_bfSize.bytes[3],
          0, 0, 0, 0,
          54, 0, 0, 0,
          40, 0, 0, 0,
          l_width.bytes[0], l_width.bytes[1], l_width.bytes[2], l_width.bytes[3],
          l_height.bytes[0], l_height.bytes[1], l_height.bytes[2], l_height.bytes[3],
          1, 0,
          24, 00,
          0, 0, 0, 0,
          l_biSizeImage.bytes[0], l_biSizeImage.bytes[1], l_biSizeImage.bytes[2], l_biSizeImage.bytes[3],
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0,
          0, 0, 0, 0
      };

      fs.write(reinterpret_cast<char *>(header.data()), header.size());

      for (int i = 0; i < data.size(); i++)
      {
        for (int k = 0; k < size; ++k)
        {
          for (int j = 0; j < data.size(); j++)
          {
            int color = data[j][i] ? 0 : 255;
            for (int l = 0; l < size; ++l)
            {
              fs.put(color).put(color).put(color);
            }
          }
          for (std::size_t j = 0; j < width_r - width * 3; j++)
          {
            fs.put(0);
          }
        }
      }
      fs.put(0).put(0);
      fs.close();
    }

    void paint_ppm(std::ofstream &fs, std::size_t size)
    {
      fs << "P1\n" << data.size() * size << " " << data[0].size() * size << "\n";
      for (int i = static_cast<int>(data.size() - 1); i >= 0; --i)
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