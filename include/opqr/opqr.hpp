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
#ifndef OPQR_OPQR_HPP
#define OPQR_OPQR_HPP
#include <vector>
#include <type_traits>
#include <variant>
#include <array>
#include <bitset>
#include <cstddef>
#include <charconv>
#include <algorithm>
#include <functional>
#include <array>
#include "optables.hpp"
#include "oppic.hpp"
#include "operror.hpp"
#include "oputils.hpp"

//#include <iostream>
//void text_debug(const std::vector<std::vector<bool>>& s)
//{
//  for (int i = s.size()-1 ; i >=0; --i)
//  {
//    for (int j = 0;j<s[i].size();++j)
//    {
//      if (s[j][i])
//        std::cout << "\033[31mx\033[0m";
//      else
//        std::cout << "\033[32mx\033[0m";
//    }
//    std::cout << std::endl;
//  }
//}
//void pic_debug(const std::vector<std::vector<bool>>& s, std::string name = "test.bmp")
//{
//  opqr::pic::Pic pic(s);
//  auto f = std::ofstream(std::move(name));
//  pic.paint(opqr::pic::Format::BMP, f, 10);
//  f.close();
//}
//void fill_box_debug(const std::vector<std::vector<bool>>& s, int ik)
//{
//  for(auto& i : s)
//  {
//    for(auto j : i)
//    {
//      j = ik;
//    }
//  }
//}
//void check_vec_debug(const std::vector<bool> vec)
//{
//  int n = 0;
//  for (int i = 0; i < vec.size(); ++i)
//  {
//    n++;
//    std::cout << vec[i];
//    if(n == 8)
//    {
//      std::cout << " ";
//      n= 0;
//    }
//  }
//  std::cout << std::flush;
//}

namespace opqr
{
  enum class ECLevel : std::size_t
  {
    L = 0, M, Q, H, AUTO
  };
  enum class Mode : std::size_t
  {
    NUM = 0,
    ALNUM,
    BIT8,
    KANJI,
    AUTO
  };

  void bin_to_dec(const std::vector<bool>::const_iterator first, const std::vector<bool>::const_iterator last,
    std::vector<std::byte> &dest)
  {
    for (auto it = first; it < last; it += 8)
    {
      std::bitset<8> bits;
      for (int j = 0; j < 8; ++j)
        bits[7 - j] = *(it + j);
      dest.emplace_back(static_cast<std::byte>(bits.to_ulong()));
    }
  }

  void dec_to_bin(const std::vector<std::byte>::const_iterator first,
    const std::vector<std::byte>::const_iterator last,
    std::vector<bool> &dest)
  {
    for (auto it = first; it < last; ++it)
    {
      std::bitset<8> bits(std::to_integer<unsigned long>(*it));
      for (int j = 0; j < 8; ++j)
        dest.emplace_back(bits[7 - j]);
    }
  }
  
  template<std::size_t S>
  void add_bits(std::vector<bool> &v, std::size_t a)
  {
    std::bitset<S> bits(a);
    for (int i = S - 1; i >= 0; --i)
    {
      v.emplace_back(bits[i]);
    }
  }
  
  ECLevel to_ecl(std::size_t l)
  {
    return static_cast<ECLevel>(l);
  }

  Mode to_mode(std::size_t l)
  {
    return static_cast<Mode>(l);
  }

  std::size_t to_sz(ECLevel l)
  {
    return static_cast<std::size_t>(l);
  }

  std::size_t to_sz(Mode l)
  {
    return static_cast<std::size_t>(l);
  }
  class QR
  {
  public:
    int version;
    ECLevel level;
    Mode mode;
    std::vector<std::vector<bool>> final_qr;
    std::string raw;
    int mask;
    bool quiet_zone;
  private:
    bool inited;
    std::vector<std::byte> encoded_data;
    std::vector<std::byte> ec_data;
    std::vector<std::byte> final_data;
    std::vector<std::vector<bool>> filled;
    utils::PosSet function_pattern_pos;
  public:
    QR(int version_ = -1, ECLevel level_ = ECLevel::AUTO, Mode mode_ = Mode::AUTO, int mask_ = -1)
      : version(version_), level(level_), mode(mode_), mask(mask_), inited(false), quiet_zone(true)
    {}
    template<typename T, typename = std::enable_if_t<!std::is_base_of_v<QR, std::decay_t<T>>>>
    QR(T &&data)
      : version(-1), mask(-1), level(ECLevel::AUTO), mode(Mode::AUTO), inited(false), quiet_zone(true)
    {
      add_data(std::forward<T>(data));
    }
    template <typename T, typename =
      std::enable_if_t<std::is_integral_v<T>>>
    QR &add_data(const T &data)
    {
      return add_data(std::to_string(data));
    }
    template <typename T, typename =
      std::enable_if_t<!std::is_same_v<std::string, std::decay_t<T>>
      &&std::is_integral_v<typename T::value_type>>>
      QR &add_data(T &&data)
    {
      std::string str;
      for (auto &r : data)
        str += std::to_string(r);
      return add_data(str);
    }

    template <typename T, typename =
      std::enable_if_t<std::is_integral_v<T>>>
    QR &add_data(const std::initializer_list<T> &data)
    {
      std::string str;
      for (auto &r : data)
        str += std::to_string(r);
      return add_data(str);
    }
    QR &add_data(std::string data)
    {
      if (inited)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Can not add data after generating.");
      raw = std::move(data);
      return *this;
    }
    QR &set_mode(Mode mode_)
    {
      if (inited)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Can not set Mode after generating.");
      mode = mode_;
      return *this;
    }

    QR &set_mask(int m)
    {
      if (m < 0 || m > 7)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Invalid Mask '" + std::to_string(m) + "'.");
      if (inited)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Can not set Mask after generating.");
      mask = m;
      return *this;
    }

    QR &set_version(int v)
    {
      if (v < 1 || v > 40)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Invalid Version '" + std::to_string(v) + "'.");
      if (inited)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Can not set version after generating.");
      version = v;
      return *this;
    }

    QR &set_level(ECLevel l)
    {
      if (inited)
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "Can not set Error correction Level after adding data.");
      level = l;
      return *this;
    }
    QR &clear()
    {
      encoded_data.clear();
      ec_data.clear();
      final_data.clear();
      function_pattern_pos.box.clear();
      final_qr.clear();
      filled.clear();
      raw.clear();
      version = -1;
      mask = -1;
      mode = Mode::BIT8;
      inited = false;
      return *this;
    }


    pic::Pic generate()
    {
      init();
      data_encode();
      generate_ECBlock();
      allocate_data();
      fill_function_patterns();
      fill_data();
      select_mask_pattern_to_final_qr();
      fill_format_infomation();
      if (quiet_zone)
        add_quiet_zone();
      return pic::Pic(final_qr);
    }

    QR &disable_quiet_zone()
    {
      quiet_zone = false;
      return *this;
    }
  private:
    void select_qr()
    {
      if (mode == Mode::AUTO)
      {
        bool is_num = true;
        bool is_alnum = true;
        bool is_kanji = false;
        for (auto &r : raw)
        {
          if (!std::isdigit(r)) is_num = false;
          if (!std::isalnum(r) || std::islower(r)) is_alnum = false;
        }
        if (!is_num && !is_alnum && raw.size() % 2 == 0)
        {
          is_kanji = true;
          int p = 0;
          for (std::size_t i = 0; i < raw.size(); i += 2)
          {
            unsigned char a = static_cast<unsigned char>(raw[i]);
            unsigned char b = static_cast<unsigned char>(raw[i + 1]);
            if (!(a >= 0x81 && a <= 0x9f) && !(a >= 0xe0 && a <= 0xea))
            {
              is_kanji = false;
              break;
            }
            if (!(b >= 0x40 && b <= 0xfc))
            {
              is_kanji = false;
              break;
            }
          }
        }
        if (is_num) mode = Mode::NUM;
        else if (is_alnum) mode = Mode::ALNUM;
        else if (is_kanji) mode = Mode::KANJI;
        else mode = Mode::BIT8;
      }

      if (level == ECLevel::AUTO && version == -1)
      {
        int l;
        for (l = 3; l >= 0; --l)
        {
          version = 1;
          while (version < 41 && tables::qr_info[version].level[l].capacity[to_sz(mode)] < raw.size())
          {
            ++version;
          }
          if (version == 41)
          {
            if (l == 0)
            {
              throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is too big.");
            }
            else
              continue;
          }
          else
            break;
        }
        level = to_ecl(l);
      }
      else if (level != ECLevel::AUTO)//version == -1
      {
        version = 1;
        while (version < 41 && tables::qr_info[version].level[to_sz(level)].capacity[to_sz(mode)] < raw.size())
          ++version;
        if (version == 41)
          throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is too big.");
      }
      else //ecl undef
      {
        int l = 3;
        while (l >= 0 && tables::qr_info[version].level[l].capacity[to_sz(mode)] < raw.size())
        {
          --l;
        }
        if (l == -1)
        {
          throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is too big.");
        }
        level = to_ecl(l);
      }
    }

    void data_encode()
    {
      switch (mode)
      {
      case Mode::NUM:
        num_encode();
        break;
      case Mode::ALNUM:
        alnum_encode();
        break;
      case Mode::BIT8:
        bit8_encode();
        break;
      case Mode::KANJI:
        kanji_encode();
        break;
      }
    }

    void init()
    {
      if (version < 0 || level == ECLevel::AUTO)
      {
        select_qr();
      }
      else if (tables::qr_info[version].level[to_sz(level)].capacity[to_sz(mode)] < raw.size())
      {
        throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is too big.");
      }
      encoded_data.clear();
      function_pattern_pos = tables::make_function_pattern_pos(version,
        tables::qr_info[version].dimension,
        tables::qr_info[version].alignment_pos,
        tables::qr_info[version].nalignment_pos);
      inited = true;
    }

    void add_term(std::vector<bool> &v)
    {
      std::size_t nt = tables::qr_info[version].level[to_sz(level)].capacity[to_sz(mode)] - raw.size();
      if (nt > 4) nt = 4;
      for (int i = 0; i < nt; ++i)
        v.emplace_back(0);

      while (v.size() % 8 != 0)
        v.emplace_back(0);
  
      int capacity = tables::qr_info[version].level[to_sz(level)].ndatawords * 8;
      std::size_t npadding = (capacity - v.size()) / 8;
      std::vector<bool> padding_bytes;
      bool flag = true;
      for (int i = 0; i < npadding; ++i)
      {
        if (flag)
        {
          padding_bytes.insert(padding_bytes.end(), { 1, 1, 1, 0, 1, 1, 0, 0 });
          flag = false;
        }
        else
        {
          padding_bytes.insert(padding_bytes.end(), { 0, 0, 0, 1, 0, 0, 0, 1 });
          flag = true;
        }
      }
      v.insert(v.end(), std::make_move_iterator(padding_bytes.begin()),
        std::make_move_iterator(padding_bytes.end()));
    }

    void num_encode()
    {
      std::vector<bool> v = { 0, 0, 0, 1 };
      //Character Count Indicator
      int ncci = tables::qr_info[version].nccindicator[to_sz(mode)];
      switch (ncci)
      {
      case 10:
        add_bits<10>(v, raw.size());
        break;
      case 12:
        add_bits<12>(v, raw.size());
        break;
      case 14:
        add_bits<14>(v, raw.size());
        break;
      }

      for (auto i = 0; i < raw.size(); i += 3)
      {
        if (i + 2 == raw.size())
        {
          add_bits<7>(v, (raw[i] - '0') * 10 + (raw[i + 1] - '0'));
          break;
        }
        else if (i + 1 == raw.size())
        {
          add_bits<4>(v, (raw[i] - '0'));
          break;
        }
        else
        {
          add_bits<10>(v, (raw[i] - '0') * 100 + (raw[i + 1] - '0') * 10 + (raw[i + 2] - '0'));
        }
      }
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }


    void alnum_encode()
    {
      std::vector<bool> v = { 0, 0, 1, 0 };
      //Character Count Indicator
      int ncci = tables::qr_info[version].nccindicator[to_sz(mode)];
      switch (ncci)
      {
      case 9:
        add_bits<9>(v, raw.size());
        break;
      case 11:
        add_bits<11>(v, raw.size());
        break;
      case 13:
        add_bits<13>(v, raw.size());
        break;
      }

      for (auto i = 0; i < raw.size(); i += 2)
      {
        if (i + 1 == raw.size())
        {
          auto w = tables::alnum[raw[i]];
          if (w == -1)
          {
            throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is not Alphanumeric.");
          }
          add_bits<6>(v, w);
          break;
        }
        else
        {
          auto w1 = tables::alnum[raw[i]];
          auto w2 = tables::alnum[raw[i + 1]];
          if (w1 == -1 || w2 == -1)
          {
            throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is not Alphanumeric.");
          }
          add_bits<11>(v, w1 * 45 + w2);
        }
      }
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }

    void bit8_encode()
    {
      std::vector<bool> v = { 0, 1, 0, 0 };
      //Character Count Indicator
      int ncci = tables::qr_info[version].nccindicator[to_sz(mode)];
      switch (ncci)
      {
      case 8:
        add_bits<8>(v, raw.size());
        break;
      case 16:
        add_bits<16>(v, raw.size());
        break;
      }
      for (auto i = 0; i < raw.size(); i++)
        add_bits<8>(v, raw[i]);
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }

    void kanji_encode()
    {
      std::vector<bool> v = { 1, 0, 0, 0 };
      //Character Count Indicator
      int ncci = tables::qr_info[version].nccindicator[to_sz(mode)];
      switch (ncci)
      {
      case 8:
        add_bits<8>(v, raw.size() / 2);
        break;
      case 10:
        add_bits<10>(v, raw.size() / 2);
        break;
      case 12:
        add_bits<12>(v, raw.size() / 2);
        break;
      }
      for (std::size_t i = 0; i < raw.size(); i += 2)
      {
        unsigned char a = static_cast<unsigned char>(raw[i]);
        unsigned char b = static_cast<unsigned char>(raw[i + 1]);
        std::uint16_t character = a << 8 | b & 0xff;
        if (character >= 0x8140 && character <= 0x9ffc)
        {
          character -= 0x8140;
          character = (character >> 8) * 0xc0 + (character & 0xff);
        }
        else if (character >= 0xe040 && character <= 0xebbf)
        {
          character -= 0xc140;
          character = (character >> 8) * 0xc0 + (character & 0xff);
        }
        else if (a > 0xa1 && a < 0xaa)
        {
          character = (a - 0xa1) * 0x60 + b - 0xa1;
        }
        else if (a > 0xb0 && a < 0xfa)
        {
          character = (a - 0xa6) * 0x60 + b - 0xa1;
        }
        else
          throw error::Error(OPQR_ERROR_LOCATION, __func__, "The data is not Kanji.");
        add_bits<13>(v, character);
      }
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }

    void generate_ECBlock()
    {
      ////test
      //encoded_data = {32 ,65 ,205 ,69 ,41 ,220 ,46 ,128 ,236};
      //version = 1;
      //level = ECLevel::H;
      //expected 42 159 74 221 244 169 239 150 138 70 237 85 224 96 74 219 61

      std::array<std::byte, 123> ecwork;
      ecwork.fill(std::byte{ 0 });

      int necb_group = tables::qr_info[version].level[to_sz(level)].necb_group;
      auto ecb_group = tables::qr_info[version].level[to_sz(level)].ecb_group;
      auto move = [&ecwork]()
      {
        for (int m = 0; m < 122; ++m)
        {
          ecwork[m] = ecwork[m + 1];
        }
        ecwork[122] = std::byte{ 0 };
      };
      auto data_it = encoded_data.begin();
      for (int i = 0; i < necb_group; i++)
      {
        int nec_block = ecb_group[i].nec_block;
        int ndatawords = ecb_group[i].ndatawords;
        int necwords = (ecb_group[i].ntotalwords - ecb_group[i].ndatawords);

        for (int j = 0; j < nec_block; j++)
        {
          ecwork.fill(std::byte{ 0 });
          std::copy(data_it, data_it + ndatawords, ecwork.begin());

          for (int k = 0; k < ndatawords; k++)
          {
            if (ecwork[0] == std::byte{ 0 })
            {
              move();
              continue;
            }

            int e = tables::fac_to_exp[std::to_integer<short>(ecwork[0])];
            move();
            for (int m = 0; m < necwords; ++m)
              ecwork[m] ^= tables::exp_to_fac[(tables::generator[necwords][m] + e) % 255];
          }
          for (int h = 0; h < necwords; h++)
            ec_data.emplace_back(ecwork[h]);
          data_it += ndatawords;
        }
      }
    }

    void allocate_data()
    {
      int necb_group = tables::qr_info[version].level[to_sz(level)].necb_group;
      auto ecb_group = tables::qr_info[version].level[to_sz(level)].ecb_group;
      int ndatawords_max = ecb_group[necb_group - 1].ndatawords;
      int necwords_max = ecb_group[necb_group - 1].ntotalwords - ecb_group[necb_group - 1].ndatawords;

      for (int i = 0; i < ndatawords_max; i++)
      {
        int pos = i;
        for (int j = 0; j < necb_group; j++)
        {
          int ndatawords = ecb_group[j].ndatawords;
          for (int k = 0; k < ecb_group[j].nec_block; k++)
          {
            if (i < ndatawords)
              final_data.emplace_back(encoded_data[pos]);
            pos += ndatawords;
          }
        }
      }

      for (int i = 0; i < necwords_max; i++)
      {
        int pos = i;
        for (int j = 0; j < necb_group; j++)
        {
          int necwords = ecb_group[j].ntotalwords - ecb_group[j].ndatawords;
          for (int k = 0; k < ecb_group[j].nec_block; k++)
          {
            if (i < necwords)
              final_data.emplace_back(ec_data[pos]);
            pos += necwords;
          }
        }
      }
    }

    void fill_function_patterns()
    {
      const std::size_t dimension = tables::qr_info[version].dimension;
      filled.resize(dimension);
      for (auto &r : filled)
      {
        r.resize(dimension);
      }

      //Position Detection Pattern
      tables::make_pdp_pos(dimension).fill(filled);
      //Alignment Pattern
      tables::make_alignment_pos(tables::qr_info[version].alignment_pos,
        tables::qr_info[version].nalignment_pos,
        dimension).fill(filled);

      //Timing Pattern
      tables::make_timing_pos(dimension).fill(filled);
    }

    void fill_data()
    {
      const std::size_t dimension = tables::qr_info[version].dimension;
      utils::Pos pos(dimension - 1, 0);//from (dimension - 1,0)

      int delta_y = 1;
      int delta_x = -1;
      auto next = [&pos, &delta_x, &delta_y, &dimension, this]()
      {
        do
        {
          pos.x += delta_x;
          if (delta_x > 0) pos.y += delta_y;
          delta_x = -delta_x;

          if (pos.y < 0 || pos.y >= dimension)
          {
            pos.y -= delta_y;
            delta_y = -delta_y;
            pos.x -= 2;
            if (pos.x == 6) pos.x--;//skip Timing Pattern
          }
        } while (function_pattern_pos.has_pos(pos));
      };
      std::vector<bool> final_databits;
      dec_to_bin(final_data.cbegin(), final_data.cend(), final_databits);
      //Remainder Bits
      final_databits.insert(final_databits.end(), tables::qr_info[version].remainder_bits, 0);
      for (auto b : final_databits)
      {
        filled[pos.x][pos.y] = b;
        next();
      }
      filled[8][7] = 1;//Dark Module
    }

    std::vector<std::vector<bool>> apply_a_mask_pattern(int type)
    {
      std::vector<std::vector<bool>> applied = filled;
      const std::size_t dimension = tables::qr_info[version].dimension;
      for (int i = 0; i < dimension; i++)
      {
        for (int j = 0; j < dimension; j++)
        {
          utils::Pos true_pos(j, dimension - i - 1);
          //According to the QR Spec, (i, j) = (0, 0) is in the top left module in the symbol.
          //https://files-cdn.cnblogs.com/files/elaron/qr_code.pdf
          //but this (0,0) is in the bottom left
          if (function_pattern_pos.has_pos(true_pos))
          {
            continue;
          }
          if ((type == 0 && (i + j) % 2 == 0) ||
            (type == 1 && i % 2 == 0) ||
            (type == 2 && j % 3 == 0) ||
            (type == 3 && (i + j) % 3 == 0) ||
            (type == 4 && ((i / 2) + (j / 3)) % 2 == 0) ||
            (type == 5 && (i * j) % 2 + (i * j) % 3 == 0) ||
            (type == 6 && ((i * j) % 2 + (i * j) % 3) % 2 == 0) ||
            (type == 7 && ((i * j) % 3 + (i + j) % 2) % 2 == 0))
          {
            applied[true_pos.x][true_pos.y].flip();
          }
        }
      }
      return applied;
    }

    //https://www.jianshu.com/p/cfa2bae198ea
    std::array<std::vector<std::vector<bool>>, 8> apply_all_mask_pattern()
    {
      std::array<std::vector<std::vector<bool>>, 8> applies;
      for (int type = 0; type < 8; ++type)
      {
        applies[type] = std::move(apply_a_mask_pattern(type));
      }
      return applies;
    }

    std::array<int, 8> evaluate_mask_pattern(const std::array<std::vector<std::vector<bool>>, 8> &applies)
    {
      const std::size_t dimension = tables::qr_info[version].dimension;
      std::array<int, 8> penalties;
      std::size_t penalties_pos = 0;
      for (auto &app : applies)
      {
        int penalty = 0;
        //1
        int n = 0;
        for (int i = 0; i < dimension; ++i)
        {
          for (int j = 1; j < dimension; ++j)
          {
            if (app[i][j] == app[i][j - 1])
              n++;
            else
            {
              if (n >= 5)
                penalty += 3 + n - 5;
              n = 1;
            }
          }
          if (n >= 5)
          {
            penalty += 3 + n - 5;
          }
          n = 0;
        }
        for (int i = 0; i < dimension; ++i)
        {
          for (int j = 1; j < dimension; ++j)
          {
            if (app[j][i] == app[j - 1][i])
              n++;
            else
            {
              if (n >= 5)
                penalty += 3 + n - 5;
              n = 1;
            }
          }
          if (n >= 5)
          {
            penalty += 3 + n - 5;
          }
          n = 0;
        }
        //2
        for (int i = 0; i < dimension - 1; ++i)
        {
          for (int j = 1; j < dimension - 1; ++j)
          {
            if (app[i][j] == app[i][j + 1]
              && app[i][j] == app[i + 1][j + 1]
              && app[i][j] == app[i + 1][j])
            {
              penalty += 3;
            }
          }
        }
        //3 11311
        constexpr std::array<bool, 11> pattern{ 1,0,1,1,1,0,1,0,0,0,0 };
        for (std::size_t i = 0; i < dimension; ++i)
        {
          auto it = std::search(app[i].cbegin(), app[i].cend(),
            std::default_searcher(pattern.cbegin(), pattern.cend()));
          if (it != app[i].cend())
            penalty += 40;
        }
        for (std::size_t i = 0; i < dimension; ++i)
        {
          auto it = std::search(utils::xcbegin(app, i),
            utils::xcend(app, i),
            std::default_searcher(pattern.cbegin(), pattern.cend()));
          if (it != utils::xcend(app, i))
            penalty += 40;
        }
        //4
        std::size_t black = 0;
        for (auto &r : app)
          black += std::count(r.begin(), r.end(), true);
        penalty += std::abs(long(black * 100 / (dimension * dimension) - 50)) / 5 * 10;

        //end
        penalties[penalties_pos] = penalty;
        ++penalties_pos;
      }
      return penalties;
    }

    void select_mask_pattern_to_final_qr()
    {
      if (mask == -1)
      {
        auto applies = std::move(apply_all_mask_pattern());
        auto penalties = evaluate_mask_pattern(applies);
        auto it = std::min_element(penalties.begin(), penalties.end());
        mask = static_cast<int>(it - penalties.begin());
        final_qr = applies[mask];
      }
      else
        final_qr = std::move(apply_a_mask_pattern(mask));
    }

    void fill_format_infomation()
    {
      const std::size_t dimension = tables::qr_info[version].dimension;
      //Format infomation
      int fmt = static_cast<int>(((to_sz(level) ^ 1) << 3) | mask);
      int modulo = fmt << 10;
      for (int i = 14; i >= 10; i--)
      {
        if ((modulo & (1 << i)) == 0)
        {
          continue;
        }
        modulo ^= 0x537 << (i - 10);
      }
      fmt = ((fmt << 10) + modulo) ^ 0x5412;//0x5412 == 101010000010010 to ensure that result data is not all 0
      std::bitset<15> format_bits(fmt);
      auto [fpb1, fpb2] = tables::make_format_pos(dimension);
      fpb1.fill(final_qr, format_bits);
      fpb2.fill(final_qr, format_bits);
      //Version Infomation
      if (version >= 7)
      {
        auto [vpb1, vpb2] = tables::make_version_pos(dimension);
        vpb1.fill(final_qr, tables::version_info[version]);
        vpb2.fill(final_qr, tables::version_info[version]);
      }
    }
    void add_quiet_zone()
    {
      const std::size_t dimension = tables::qr_info[version].dimension;
      final_qr.insert(final_qr.begin(), 4, std::vector<bool>(dimension, 0));
      final_qr.insert(final_qr.end(), 4, std::vector<bool>(dimension, 0));
      for (auto &r : final_qr)
      {
        r.insert(r.begin(), 4, 0);
        r.insert(r.end(), 4, 0);
      }
    }
  };
}
#endif