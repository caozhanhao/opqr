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
#ifndef OPQR_OPUTILS_HPP
#define OPQR_OPUTILS_HPP
#include <set>
#include <bitset>
#include <array>
#include <vector>
#include <iterator>
namespace opqr::utils
{
  class Pos
  {
  public:
    int x;
    int y;
  public:
    Pos(std::size_t x_, std::size_t y_)
      : x(x_), y(y_) {}

    Pos(std::array<std::size_t, 2> s)
      : x(s[0]), y(s[1]) {}

    Pos() : x(0), y(0) {}
  };

  bool operator<(const Pos &p1, const Pos &p2)
  {
    if (p1.x == p2.x)
      return p1.y < p2.y;
    return p1.x < p2.x;
  }

  class PosBox
  {
  public:
    std::vector<Pos> box;
  public:
    PosBox(std::vector<Pos> posbox) : box(std::move(posbox)) {}

    template<std::size_t sz>
    PosBox(std::array<std::array<std::size_t, 2>, sz> posbox) : box(posbox.cbegin(), posbox.cend()) {}

    PosBox(const Pos &p1, const Pos &p2)
    {
      add(p1, p2);
    }

    PosBox() {}

    PosBox &add(const Pos &p)
    {
      box.emplace_back(p);
      return *this;
    }

    PosBox &add(const Pos &p1, const Pos &p2)
    {
      Pos t1(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
      Pos t2(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
      for (int i = t1.x; i <= t2.x; i++)
      {
        for (int j = t1.y; j <= t2.y; j++)
        {
          box.emplace_back(Pos(i, j));
        }
      }
      return *this;
    }

    void fill(std::vector<std::vector<bool>> &vec)
    {
      for (auto &p : box)
      {
        vec[p.x][p.y] = 1;
      }
    }

    template<std::size_t sz>
    void fill(std::vector<std::vector<bool>> &vec, std::bitset<sz> data)
    {
      for (std::size_t i = 0; i < sz; ++i)
      {
        vec[box[i].x][box[i].y] = data[i];
      }
    }
  };

  class PosSet
  {
  public:
    std::set<Pos> box;
  public:
    PosSet() = default;

    PosSet &add(const Pos &p1, const Pos &p2)
    {
      Pos t1(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
      Pos t2(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
      for (int i = t1.x; i <= t2.x; i++)
      {
        for (int j = t1.y; j <= t2.y; j++)
        {
          box.insert(Pos(i, j));
        }
      }
      return *this;
    }

    void add(const Pos &p1)
    {
      box.insert(p1);
    }

    bool has_pos(const Pos &pos)
    {
      return (box.find(pos) != box.end());
    }

    void fill(std::vector<std::vector<bool>> &vec)
    {
      for (auto &p : box)
      {
        vec[p.x][p.y] = 1;
      }
    }
  };

  template<typename Itr>
  class XIterator
  {
  public:
    using difference_type = typename Itr::difference_type;
    using value_type = typename Itr::value_type;
    using pointer = typename Itr::pointer;
    using reference = typename Itr::reference;
    using iterator_category = typename Itr::iterator_category;
  public:
    Itr it;
    std::size_t delta;
  public:
    XIterator(const Itr &it_, std::size_t delta_) : it(it_), delta(delta_) {}
    typename value_type::value_type operator*() const
    {
      return *(it->begin() + delta);
    }
    XIterator &operator++()
    {
      ++it;
      return *this;
    }

    difference_type operator-(const XIterator &other) const
    {
      return it - other.it;
    }
  };
  template<typename T>
  bool operator==(const XIterator<T> &it1, const XIterator<T> &it2)
  {
    return (it1.it == it2.it) && (it1.delta == it2.delta);
  }
  template<typename T>
  bool operator!=(const XIterator<T> &it1, const XIterator<T> &it2)
  {
    return (it1.it != it2.it) || (it1.delta != it2.delta);
  }
  template<typename T>
  XIterator<typename T::const_iterator> xcbegin(const T &v, std::size_t delta)
  {
    return XIterator<typename T::const_iterator>(v.cbegin(), delta);
  }
  template<typename T>
  XIterator<typename T::const_iterator> xcend(const T &v, std::size_t delta)
  {
    return XIterator<typename T::const_iterator>(v.cend(), delta);
  }
  template<typename T>
  XIterator<typename T::iterator> xbegin(T &v, std::size_t delta)
  {
    return XIterator<T::iterator>(v.begin(), delta);
  }
  template<typename T>
  XIterator<typename T::iterator> xend(T &v, std::size_t delta)
  {
    return XIterator<T::iterator>(v.end(), delta);
  }
}
#endif
