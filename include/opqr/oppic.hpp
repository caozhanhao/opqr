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

#define STB_IMAGE_IMPLEMENTATION
//#define STBI_ONLY_PNM
#include "stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include "stb/stb_image_resize.h"

#include <vector>
#include <fstream>

namespace opqr::pic
{
  enum class Format
  {
    JPG, PNG, TGA, BMP, ANSI256
  };
  
  class Pic
  {
  private:
    struct StbData
    {
      int width;
      int height;
      int channels;
      stbi_uc *data;
    };
  private:
    std::vector<std::vector<bool>> data;
  
  public:
    Pic(std::vector<std::vector<bool>> data_)
        : data(std::move(data_)) {}
    
    void paint(Format fmt, const std::string &path, size_t width, size_t height) const
    {
      if (fmt == Format::ANSI256)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Unsupported format(ANSI256) when writing to file");
      }
      size_t enlarge = (std::max(width, height) / data.size()) - 1;
      auto pic = load_pic(enlarge);
      auto out = (unsigned char *) malloc(
          static_cast<size_t>(static_cast<double>(width * height * pic.channels) * 1.5));
      int ret = stbir_resize_uint8(pic.data, pic.width, pic.height, 0,
                                   out, static_cast<int>(width), static_cast<int>(height), 0, pic.channels);
      if (ret == 0)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, std::string("resize failed: ") + stbi_failure_reason());
      }
      stbi_image_free(pic.data);
      pic.data = out;
      pic.width = static_cast<int>(width);
      pic.height = static_cast<int>(height);
      write_pic(fmt, path, pic);
      stbi_image_free(out);
    }
  
    void paint(Format fmt, const std::string &path, size_t enlarge = 1) const
    {
      if (enlarge == 0)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "enlarge must >= 1.");
      }
      if (fmt == Format::ANSI256)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Unsupported format(ANSI256) when writing to path");
      }
      auto pic = load_pic(enlarge);
      write_pic(fmt, path, pic);
      stbi_image_free(pic.data);
    }
  
    void paint(Format fmt, std::ostream &os, size_t enlarge = 1) const
    {
      if (enlarge == 0)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "enlarge must >= 1.");
      }
      switch (fmt)
      {
        case Format::ANSI256:
          for (int i = static_cast<int>(data.size() - 1); i >= 0; --i)
          {
            for (int l = 0; l < enlarge; ++l)
            {
              for (int j = 0; j < data.size(); ++j)
              {
                for (int l = 0; l < enlarge; ++l)
                {
                  if (data[j][i])
                  {
                    os << "\033[48;5;16m  \033[0m";
                  }
                  else
                  {
                    os << "\033[48;5;231m  \033[0m";
                  }
                }
              }
              os << "\n";
            }
          }
          break;
        default:
          throw error::Error(OPQR_ERROR_LOCATION, __func__, "Unsupported format when writing to a stream");
          break;
      }
    }

  private:
    void write_pic(Format fmt, const std::string &path, StbData data) const
    {
      int ret = 0;
      switch (fmt)
      {
        case Format::JPG:
          ret = stbi_write_jpg(path.c_str(), data.width, data.height, data.channels, data.data, 100);
          break;
        case Format::PNG:
          ret = stbi_write_png(path.c_str(), data.width, data.height, data.channels, data.data, 0);
          break;
        case Format::TGA:
          ret = stbi_write_tga(path.c_str(), data.width, data.height, data.channels, data.data);
          break;
        case Format::BMP:
          ret = stbi_write_bmp(path.c_str(), data.width, data.height, data.channels, data.data);
          break;
      }
      if (ret == 0)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, std::string("write failed: ") + stbi_failure_reason());
      }
    }
    
    StbData load_pic(size_t enlarge) const
    {
      if (enlarge == 0)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "enlarge must >= 1.");
      }
      
      std::vector<stbi_uc> raw_ppm{'P', '5', ' '};
      size_t pgm_size = data.size() * enlarge;
      std::string pgm_header = std::to_string(pgm_size) + " " + std::to_string(pgm_size) + " 255 ";
      for (auto &r: pgm_header)
      {
        raw_ppm.emplace_back(r);
      }
      
      for (int i = static_cast<int>(data.size() - 1); i >= 0; --i)
      {
        for (int l = 0; l < enlarge; ++l)
        {
          for (int j = 0; j < data.size(); ++j)
          {
            for (int k = 0; k < enlarge; ++k)
            {
              raw_ppm.emplace_back(data[j][i] ? 0 : 255);
            }
          }
        }
      }
      
      int w, h, n;
      auto pgm_data = stbi_load_from_memory(raw_ppm.data(), static_cast<int>(raw_ppm.size()), &w, &h, &n, 0);
      if (w != pgm_size || h != pgm_size)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Unexpected error when generating pgm.");
      }
      if (!pgm_data)
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, std::string("load pgm failed: ") + stbi_failure_reason());
      }
      return {w, h, n, pgm_data};
    }
  };
}
#endif