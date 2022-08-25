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
#ifndef OPQR_OPPOS_H
#define OPQR_OPPOS_H
#include <set>
#include <vector>
namespace op::pos
{
  class Pos
  {
  public:
    std::size_t x;
    std::size_t y;
  public:
    Pos(std::size_t x_, std::size_t y_)
        : x(x_), y(y_) {}
    
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
    
    PosBox &add(PosBox p)
    {
      box.insert(box.end(), std::make_move_iterator(p.box.begin()), std::make_move_iterator(p.box.end()));
      return *this;
    }
    
    PosBox &up(std::size_t n)
    {
      for (auto &p: box)
      {
        p.y += n;
      }
      return *this;
    }
    
    PosBox &down(std::size_t n)
    {
      for (auto &p: box)
      {
        p.y -= n;
      }
      return *this;
    }
    
    PosBox &left(std::size_t n)
    {
      for (auto &p: box)
      {
        p.x -= n;
      }
      return *this;
    }
    
    PosBox &right(std::size_t n)
    {
      for (auto &p: box)
      {
        p.x += n;
      }
      return *this;
    }
    
    PosBox &add_up(const PosBox &pb, std::size_t n)
    {
      for (int i = 0; i < pb.box.size(); ++i)
      {
        add(Pos(pb.box[i].x, pb.box[i].y + n));
      }
      return *this;
    }
    
    PosBox &add_down(const PosBox &pb, std::size_t n)
    {
      for (int i = 0; i < pb.box.size(); ++i)
      {
        add(Pos(pb.box[i].x, pb.box[i].y - n));
      }
      return *this;
    }
    
    PosBox &add_right(const PosBox &pb, std::size_t n)
    {
      for (int i = 0; i < pb.box.size(); ++i)
      {
        add(Pos(pb.box[i].x + n, pb.box[i].y));
      }
      return *this;
    }
    
    PosBox &add_left(const PosBox &pb, std::size_t n)
    {
      for (int i = 0; i < pb.box.size(); ++i)
      {
        add(Pos(pb.box[i].x - n, pb.box[i].y));
      }
      return *this;
    }
    
    void fill(std::vector<std::vector<bool>> &vec)
    {
      for (auto &p: box)
      {
        vec[p.x][p.y] = 1;
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
    
    bool has_pos(const Pos &pos)
    {
      return (box.find(pos) != box.end());
    }
    
    void fill(std::vector<std::vector<bool>> &vec)
    {
      for (auto &p: box)
      {
        vec[p.x][p.y] = 1;
      }
    }
  };
}
#endif
