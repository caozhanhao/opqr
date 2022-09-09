#include <iostream>
#include "opqr.hpp"
int main()
{
  std::ofstream fs("test.ppm");
  opqr::QR qr;
  qr.add_data("https://gitee.com/cmvy2020/opqr");
  auto pic = qr.generate();
  pic.paint(fs);
  return 0;
}
