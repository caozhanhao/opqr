# opqr

## Introduction
- A Simple header-only QR encode Library.
## Usage
```c++
#include <fstream>
#include "opqr/opqr.hpp"
using namespace opqr;
int main()
{
  std::ofstream bmp("test.bmp");
  QR qr("https://gitee.com/cmvy2020/opqr");
  qr.generate().paint(pic::Format::BMP, bmp, 10);
  return 0;
}
```
## Note
- There may be bugs in Kanji Mode
## Reference
[二维码生成原理](https://zhuanlan.zhihu.com/p/543574464)