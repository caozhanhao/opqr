#include <fstream>
#include "../include/opqr/opqr.hpp"

using namespace opqr;
int main()
{
  std::ofstream bmp("test.bmp");
  QR qr;
  qr.set_version(10);
  qr.add_data("https://gitee.com/cmvy2020/opqr");
  qr.generate().paint(pic::Format::BMP, bmp, 10);
  return 0;
}
