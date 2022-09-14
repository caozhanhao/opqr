#include <fstream>
#include "opqr.hpp"

using namespace opqr;
int main()
{
  std::ofstream bmp("test.bmp");
  QR qr("https://gitee.com/cmvy2020/opqr");
  qr.generate().paint(pic::Format::BMP, bmp, 10);
  return 0;
}
