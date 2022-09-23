#include <fstream>
#include "../include/opqr/opqr.hpp"
using namespace opqr;
int main()
{
  std::ofstream bmp("test.bmp");
  QR qr("https://github.com/caozhanhao/opqr");
  qr.generate().paint(pic::Format::BMP, bmp, 10);
  return 0;
}
