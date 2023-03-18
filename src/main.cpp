//   Copyright 2022 - 2023 opqr - caozhanhao
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
#include "opqr/opqr.hpp"

using namespace opqr;

int main()
{
  std::vector<QR> examples;
  /*
   *   Add Data
   */
  //1 constructor
  QR qr1("https://github.com/caozhanhao/opqr");
  QR qr2(std::vector{0, 1, 2, 3});//or other containers, e.g. std::list
  //2 add_data()
  QR qr3;
  QR qr4;
  qr3.add_data({ 4,5,6,7 });// initializer_list
  qr4.add_data(789);// integral value


  /*
   *  Specify
   */
  //1 constructor
  QR qr5(20, ECLevel::Q, Mode::ALNUM, 0);
  qr5.add_data("OPQR"); // string
  //2 setter
  QR qr6("12345");
  qr6.set_version(10);
  qr6.set_level(ECLevel::L);
  qr6.set_mode(Mode::NUM);
  qr6.set_mask(6);
  
  
  /*
   *  Quiet Zone
   */
  qr6.disable_quiet_zone();
  
  
  /*
   *  Output
   */
  //1 PPM
  qr1.generate().paint(pic::Format::PPM, "examples/example1.ppm", 10);
  //2 BMP
  qr2.generate().paint(pic::Format::BMP, "examples/example2.bmp", 10);
  qr3.generate().paint(pic::Format::BMP, "examples/example3.bmp", 10);
  qr4.generate().paint(pic::Format::BMP, "examples/example4.bmp", 10);
  qr5.generate().paint(pic::Format::BMP, "examples/example5.bmp", 10);
  qr6.generate().paint(pic::Format::BMP, "examples/example6.bmp", 10);
  return 0;
}
