# opqr

## 功能
- C++二维码生成(header only)
## 示例
```c++
#include <fstream>
#include "opqr.hpp"
using namespace opqr;
int main()
{
  std::ofstream bmp("test.bmp");
  QR qr("https://gitee.com/cmvy2020/opqr");
  qr.generate().paint(pic::Format::BMP, bmp);
  return 0;
}
```
## 编译
```
mkdir build && cd build 
cmake .. && make
./opQR
```
## 注意事项
- Kanji Mode目前可能存在Bug
## 原理
[二维码生成原理](https://zhuanlan.zhihu.com/p/543574464)