#pragma once
#include <vector>
#include <bitset>
#include <charconv>
#include <algorithm>
#include <array>
#include "optables.h"
#include "oppic.h"
#include "operror.h"
#include "oppos.h"
//void text_debug(std::vector<std::vector<bool>>& s)
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
//void pic_debug(std::vector<std::vector<bool>>& s, std::string name = "test.ppm")
//{
//  op::pic::Pic pic(s);
//  auto f = std::ofstream(std::move(name));
//  pic.paint(f);
//}
//void fill_box_debug(std::vector<std::vector<bool>>& s, int ik)
//{
//  for(auto& i : s)
//  {
//    for(auto j : i)
//    {
//      j = ik;
//    }
//  }
//}
//void check_vec_debug(std::vector<bool> vec)
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

namespace op::qr
{
  enum class ECLevel : std::size_t
  {
    L = 0, M, Q, H
  };
  enum class Mode : std::size_t
  {
    NUM = 0,
    ALNUM,
    BIT8,
    KANJI,
    EMPTY
  };
  void bin_to_dec(const std::vector<bool>::const_iterator first, const std::vector<bool>::const_iterator last,
                  std::vector<unsigned char> &dest)
  {
    for (auto it = first; it < last; it += 8)
    {
      std::bitset<8> bits;
      for (int j = 0; j < 8; ++j)
        bits[7 - j] = *(it + j);
      dest.emplace_back(bits.to_ulong());
    }
  }
  
  void dec_to_bin(const std::vector<unsigned char>::const_iterator first,
                  const std::vector<unsigned char>::const_iterator last,
                  std::vector<bool> &dest)
  {
    for (auto it = first; it < last; ++it)
    {
      std::bitset<8> bits(*it);
      for (int j = 0; j < 8; ++j)
        dest.emplace_back(bits[7 - j]);
    }
  }
  template<std::size_t S>
  void add_bits(std::vector<bool>& v, int a)
  {
    std::bitset<S> bits(a);
    for (int i = S - 1; i >= 0; --i)
      v.emplace_back(bits[i]);
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
  private:
    int version;
    ECLevel level;
    Mode mode;
    std::string raw;
    std::vector<unsigned char> encoded_data;
    std::vector<unsigned char> ec_data;
    std::vector<unsigned char> final_data;
    std::vector<std::vector<bool>> filled;
    std::vector<std::vector<bool>> final_qr;
    pos::PosSet function_patterns;
    int mask;
  public:
    QR(int version_, ECLevel level_,  Mode mode_ = Mode::BIT8, int mask_ = -1)
        : version(version_), level(level_), mode(mode_), mask(mask_)
    {
      if (version_ > 40 || version_ < 1)
        throw error::Error(OP_ERROR_LOCATION, __func__, "Version is range from 1 to 40.");
    }
    QR(): version(-1), mask(-1), mode(Mode::BIT8)
    {}
    
    void add_data(const std::string& data)
    {
      raw = data;
      init();
    }
    void set_mode(Mode mode_)
    {
      mode = mode_;
    }
    void set_mask(int m)
    {
      mask = m;
    }
    void set_version(int v)
    {
      version = v;
    }
    void set_level(ECLevel l)
    {
      level = l;
    }
    pic::Pic generate()
    {
      data_encode();
      generate_ECBlock();
      allocate_data();
      fill_function_patterns();
      fill_data();
      select_mask_pattern_to_final_qr();
      fill_format_infomation();
      return pic::Pic(final_qr);
    }
  private:
    void select_qr()
    {
      int l;
      for (l = 3; l >=  0; --l)
      {
        version = 1;
        while (version < 41 && tables::QRTable[version].level[l].capacity[to_sz(mode)] < raw.size())
          ++version;
        if (version == 41)
        {
          if (l == 0)
            throw error::Error(OP_ERROR_LOCATION, __func__, "The data is too big.");
          else
            continue;
        }
        else
          break;
      }
      level = to_ecl(l);
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
      if (version < 0)
        select_qr();
      else if(tables::QRTable[version].level[to_sz(level)].capacity[to_sz(mode)] < raw.size())
        throw error::Error(OP_ERROR_LOCATION, __func__, "The data is too big.");
      encoded_data.clear();
    }
    
    void add_term(std::vector<bool> &v)
    {
      int nt = tables::QRTable[version].level[to_sz(level)].capacity[to_sz(mode)] - raw.size();
      if (nt > 4) nt = 4;
      for (int i = 0; i < nt; ++i)
        v.emplace_back(0);
      
      while (v.size() % 8 != 0)
        v.emplace_back(0);
      
      int capacity = tables::QRTable[version].level[to_sz(level)].ndatawords * 8;
      int npadding = (capacity - v.size()) / 8;
      std::vector<bool> padding_bytes;
      bool flag = true;
      for (int i = 0; i < npadding; ++i)
      {
        if (flag)
        {
          padding_bytes.insert(padding_bytes.end(), {1, 1, 1, 0, 1, 1, 0, 0});
          flag = false;
        } else
        {
          padding_bytes.insert(padding_bytes.end(), {0, 0, 0, 1, 0, 0, 0, 1});
          flag = true;
        }
      }
      v.insert(v.end(), std::make_move_iterator(padding_bytes.begin()),
               std::make_move_iterator(padding_bytes.end()));
    }
    
    void num_encode()
    {
      std::vector<bool> v = {0, 0, 0, 1};
      //Character Count Indicator
      int ncci = tables::QRTable[version].nccindicator[to_sz(mode)];
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
        } else if (i + 1 == raw.size())
        {
          add_bits<4>(v, (raw[i] - '0'));
          break;
        } else
        {
          add_bits<10>(v, (raw[i] - '0') * 100 + (raw[i + 1] - '0') * 10 + (raw[i + 2] - '0'));
        }
      }
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }
    
    
    void alnum_encode()
    {
      std::vector<bool> v = {0, 0, 1, 0};
      //Character Count Indicator
      int ncci = tables::QRTable[version].nccindicator[to_sz(mode)];
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
      
      for (auto i = 0; i < raw.size();i += 2)
      {
        if (i + 1 == raw.size())
        {
          auto w = tables::alnum[raw[i]];
          if (w == -1)
            throw error::Error(OP_ERROR_LOCATION, __func__, "The data is not Alphanumeric.");
          add_bits<11>(v, w);
          break;
        } else
        {
          auto w1 = tables::alnum[raw[i]];
          auto w2 = tables::alnum[raw[i + 1]];
          if (w1 == -1 || w2 == -1)
            throw error::Error(OP_ERROR_LOCATION, __func__, "The data is not Alphanumeric.");
          add_bits<11>(v, w1 * 45 + w2);
        }
      }
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }
    void bit8_encode()
    {
      std::vector<bool> v = { 0,1,0,0 };
      //Character Count Indicator
      int ncci = tables::QRTable[version].nccindicator[to_sz(mode)];
      switch (ncci)
      {
      case 8:
        add_bits<8>(v, raw.size());
        break;
      case 16:
        add_bits<16>(v, raw.size());
        break;
      }
      for (auto i = 0; i < raw.size();i++)
        add_bits<8>(v, raw[i]);
      add_term(v);
      bin_to_dec(v.cbegin(), v.cend(), encoded_data);
    }
    void kanji_encode()
    {
      std::vector<bool> v = { 1,0,0,0 };
      //Character Count Indicator
      int ncci = tables::QRTable[version].nccindicator[to_sz(mode)];
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
      for (int i = 0; i < raw.size(); i += 2)
      {
        unsigned int jis = ((unsigned int)raw[i] << 8) | raw[i + 1];
        if (jis >= 0x8140 && jis <= 0x9ffc)
        {
          jis -= 0x8140;
          unsigned int h = jis >> 8;
          unsigned int l = jis & 0xff;
          h *= 0xc0;
          add_bits<13>(v, h + l);
        }
        else if (jis >= 0xe040 && jis <= 0xebbf)
        {
          jis -= 0xc140;
          unsigned int h = jis >> 8;
          unsigned int l = jis & 0xff;
          h *= 0xc0;
          add_bits<13>(v, h + l);
        }
        else if (jis >= 0xa1 && jis <= 0xaa)
        {
          unsigned int h = jis >> 8;
          unsigned int l = jis & 0xff;
          h -= 0xa1;
          l -= 0xa1;
          h *= 0x60;
          add_bits<13>(v, h + l);
        }
        else if (jis >= 0xb0 && jis <= 0xfa)
        {
          unsigned int h = jis >> 8;
          unsigned int l = jis & 0xff;
          h -= 0xa6;
          l -= 0xa1;
          h *= 0x60;
          add_bits<13>(v, h + l);
        }
        else
          throw error::Error(OP_ERROR_LOCATION, __func__, "The data is not Kanji.");
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
      
      std::array<unsigned char, 123> ecwork;
      ecwork.fill(0);
      
      int necb_group = tables::QRTable[version].level[to_sz(level)].necb_group;
      auto ecb_group = tables::QRTable[version].level[to_sz(level)].ecb_group;
      auto move = [&ecwork]()
      {
        for (int m = 0; m < 122; ++m)
          ecwork[m] = ecwork[m + 1];
        ecwork[122] = 0;
      };
      auto data_it = encoded_data.begin();
      for (int i = 0; i < necb_group; i++)
      {
        int nec_block = ecb_group[i].nec_block;
        int ndatawords = ecb_group[i].ndatawords;
        int necwords = (ecb_group[i].ntotalwords - ecb_group[i].ndatawords);
        
        for (int j = 0; j < nec_block; j++)
        {
          ecwork.fill(0);
          std::copy(data_it, data_it + ndatawords, ecwork.begin());
          
          for (int k = 0; k < ndatawords; k++)
          {
            if (ecwork[0] == 0)
            {
              move();
              continue;
            }
            
            int e = tables::fac_to_exp[ecwork[0]];
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
      int necb_group = tables::QRTable[version].level[to_sz(level)].necb_group;
      auto ecb_group = tables::QRTable[version].level[to_sz(level)].ecb_group;
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
      const std::size_t dimension = tables::QRTable[version].dimension;
      filled.resize(dimension);
      for (auto &r:filled)
        r.resize(dimension);
      
      //Position Detection Pattern
      pos::PosBox pdp_box(tables::position_detection_pattern_pos);
      pos::PosBox pdp_box_temp(tables::position_detection_pattern_pos);
      pdp_box.add_up(pdp_box_temp, dimension - 7)
          .add_right(pdp_box_temp.up(dimension - 7), dimension - 7)
          .fill(filled);
      
      
      //Alignment Pattern
      auto nap = tables::QRTable[version].nalignment_pos;
      auto ap = tables::QRTable[version].alignment_pos;
      pos::PosBox ap_box;
      for (int i = 0; i < nap; i++)
      {
        for (int j = 0; j < nap; j++)
        {
          if (
              (ap[i] == 6 && ap[j] == 6)
              || (ap[i] == 6 && ap[j] == dimension - 7)
              || (ap[i] == dimension - 7 && ap[j] == dimension - 7)
              )
            continue;
          else
            ap_box.add(pos::Pos(ap[i], ap[j]));//center
        }
      }
      auto apbt = ap_box;
      for (int i = 0; i < apbt.box.size(); ++i)
      {
        ap_box.add(pos::Pos(apbt.box[i].x - 2, apbt.box[i].y + 2),
                   pos::Pos(apbt.box[i].x + 2, apbt.box[i].y + 2));
        ap_box.add(pos::Pos(apbt.box[i].x - 2, apbt.box[i].y + 1),
                   pos::Pos(apbt.box[i].x - 2, apbt.box[i].y - 2));
        ap_box.add(pos::Pos(apbt.box[i].x - 1, apbt.box[i].y - 2),
                   pos::Pos(apbt.box[i].x + 2, apbt.box[i].y - 2));
        ap_box.add(pos::Pos(apbt.box[i].x + 2, apbt.box[i].y - 1),
                   pos::Pos(apbt.box[i].x + 2, apbt.box[i].y + 1));
      }
      ap_box.fill(filled);
      
      //Timing Pattern
      pos::PosBox tp_box;
      for (int i = 8; i <= dimension - 9; i += 2)
        tp_box.add(pos::Pos(6, i));
      for (int i = 8; i <= dimension - 9; i += 2)
        tp_box.add(pos::Pos(i, dimension - 7));
      tp_box.fill(filled);
      
      //fill function zone
      //around Position Detection Pattern
      function_patterns.add({0, 0}, {7, 7});
      function_patterns.add({0, dimension - 1}, {7, dimension - 8});
      function_patterns.add({dimension - 8, dimension - 8}, {dimension - 1, dimension - 1});
      
      //around Alignment Pattern
      auto apbt_left_down = apbt;
      apbt_left_down.left(2).down(2);
      apbt.right(2).up(2);
      for (int i = 0; i < apbt.box.size(); ++i)
        function_patterns.add(apbt_left_down.box[i], apbt.box[i]);
      
      //around Timing Pattern
      function_patterns.add({6, 8}, {6, dimension - 10});
      function_patterns.add({9, dimension - 7}, {dimension - 9, dimension - 7});
      
      //around Format Information
      function_patterns.add(pos::Pos(8, dimension - 1), pos::Pos(8, dimension - 9));
      function_patterns.add(pos::Pos(0, dimension - 9), pos::Pos(8, dimension - 9));
      function_patterns.add(pos::Pos(dimension - 8, dimension - 9), pos::Pos(dimension - 1, dimension - 9));
      function_patterns.add(pos::Pos(8, 0), pos::Pos(8, 7));
      //around Version Infomation(Version >= 7)
      if (version >= 7)
      {
        function_patterns.add(pos::Pos(0, 8), pos::Pos(5, 10));
        function_patterns.add(pos::Pos(dimension - 9, dimension - 1), pos::Pos(dimension - 11, dimension - 6));
      }
    }
    
    void fill_data()
    {
      const std::size_t dimension = tables::QRTable[version].dimension;
      pos::Pos pos(dimension - 1, 0);//from (dimension - 1,0)
      
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
        } while (function_patterns.has_pos(pos));
      };
      std::vector<bool> final_databits;
      dec_to_bin(final_data.cbegin(), final_data.cend(), final_databits);
      //Remainder Bits
      final_databits.insert(final_databits.end(), tables::QRTable[version].remainder_bits, 0);
      
      for (auto b:final_databits)
      {
        filled[pos.x][pos.y] = b;
        next();
      }
      filled[8][7] = 1;//Dark Module
    }
    
    std::vector<std::vector<bool>> apply_a_mask_pattern(int type)
    {
      std::vector<std::vector<bool>> applied = filled;
      const std::size_t dimension = tables::QRTable[version].dimension;
      for (int i = 0; i < dimension; i++)
      {
        for (int j = 0; j < dimension; j++)
        {
          pos::Pos true_pos(j, dimension - i - 1);
          //According to the QR Spec, (i, j) = (0, 0) is in the top left module in the symbol.
          //https://files-cdn.cnblogs.com/files/elaron/qr_code.pdf
          //but this (0,0) is in the bottom left
          if (function_patterns.has_pos(true_pos))
            continue;
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
      const std::size_t dimension = tables::QRTable[version].dimension;
      std::array<int, 8> penalties;
      std::size_t penalties_pos = 0;
      for (auto &app:applies)
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
        //3
        for (int i = 0; i < dimension; ++i)
        {
          auto check_11311 = [&dimension, &app, &i]() -> bool
          {
            std::array<int, 5> arr{-1, -1, -1, -1, -1};
            std::size_t pos = 0;
            for (int j = 0; j < dimension; ++j)
            {
              if (j == 0)
              {
                if (app[i][j])
                {
                  arr[pos] = 1;
                  continue;
                }
                else
                  return false;//the first must be black 
              }
              if (app[i][j] == app[i][j - 1])
                ++arr[pos];
              else
              {
                while (pos < 5 && arr[pos] != -1)pos++;
                if (pos == 5)
                {
                  if (arr[0] == arr[1] && arr[1] * 3 == arr[2] && arr[1] == arr[3] && arr[1] == arr[4])
                    return true;
                  else
                    return false;
                }
                else 
                  arr[pos] = 1;
              }
            }
            return false;
          };
          if (check_11311())
            penalty += 40;
        }
        //4
        int black = 0;
        for (int i = 0; i < dimension; ++i)
        {
          for (int j = 0; j < dimension; ++j)
          {
            if (app[i][j])++black;
          }
        }
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
        std::size_t bestmask = 0;
        auto applies = std::move(apply_all_mask_pattern());
        auto penalties = evaluate_mask_pattern(applies);
        for (size_t i = 0; i < 8; i++)
        {
          if (penalties[i] < penalties[bestmask])
            bestmask = i;
        }
        mask = bestmask;
        final_qr = applies[mask];
      }
      else
      {
        final_qr = std::move(apply_a_mask_pattern(mask));
      }
    }
    
    void fill_format_infomation()
    {
      const std::size_t dimension = tables::QRTable[version].dimension;
      //Format infomation
      int fmt = ((to_sz(level) ^ 1) << 3) | mask;
      int modulo = fmt << 10;
      for (int i = 14; i >= 10; i--)
      {
        if ((modulo & (1 << i)) == 0)
          continue;
        modulo ^= 0x537 << (i - 10);
      }
      fmt = ((fmt << 10) + modulo) ^ 0x5412;//0x5412 == 101010000010010 to ensure that result data is not all 0
      std::bitset<15> formatbit(fmt);
      auto formatstr = formatbit.to_string();
      pos::PosBox fpb1(
          {{8, dimension - 1},
           {8, dimension - 2},
           {8, dimension - 3},
           {8, dimension - 4},
           {8, dimension - 5},
           {8, dimension - 6},
           {8, dimension - 8},
           {8, dimension - 9},
           {7, dimension - 9},
           {5, dimension - 9},
           {4, dimension - 9},
           {3, dimension - 9},
           {2, dimension - 9},
           {1, dimension - 9},
           {0, dimension - 9}
          });
      for (int i = 0; i < 15; ++i)
        final_qr[fpb1.box[i].x][fpb1.box[i].y] = formatstr[14 - i] - '0';
      pos::PosBox fpb2(
          {{dimension - 1, dimension - 9},
           {dimension - 2, dimension - 9},
           {dimension - 3, dimension - 9},
           {dimension - 4, dimension - 9},
           {dimension - 5, dimension - 9},
           {dimension - 6, dimension - 9},
           {dimension - 7, dimension - 9},
           {dimension - 8, dimension - 9},
           {8,             6},
           {8,             5},
           {8,             4},
           {8,             3},
           {8,             2},
           {8,             1},
           {8,             0}
          }
      );
      for (int i = 0; i < 15; ++i)
        final_qr[fpb2.box[i].x][fpb2.box[i].y] = formatstr[14 - i] - '0';
      //Version Infomation
      if (version >= 7)
      {
        std::bitset<18> bits(tables::VersionInfo[version]);
        pos::Pos pos1(0, 10);
        for (int i = 0; i < 18; ++i)
        {
          final_qr[pos1.x][pos1.y] = bits[i];
          if (pos1.y != 8)
            pos1.y--;
          else
          {
            pos1.x++;
            pos1.y = 10;
          }
        }
        pos::Pos pos2(dimension - 11, dimension - 1);
        for (int i = 0; i < 18; ++i)
        {
          final_qr[pos2.x][pos2.y] = bits[i];
          if (pos2.x != dimension - 9)
            pos2.x++;
          else
          {
            pos2.x = dimension - 11;
            pos2.y--;
          }
        }
      }
    }
  };
}