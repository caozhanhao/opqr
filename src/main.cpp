#include <iostream>
#include "opqr.hpp"
int main()
{
  std::ofstream fs("C:\\Users\\caozhanhao\\Pictures\\test.ppm");
  op::qr::QR qr;
  qr.add_data("https://gitee.com/cmvy2020/opqr");
  auto pic = qr.generate();
  pic.paint(fs);
  return 0;
}
