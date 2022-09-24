# opqr
[![License](https://img.shields.io/github/license/caozhanhao/opqr?style=flat-square)](LICENSE)
[![Release](https://img.shields.io/github/v/release/caozhanhao/opqr?style=flat-square)](https://github.com/caozhanhao/opqr/releases)
![Language](https://img.shields.io/github/languages/top/caozhanhao/opqr?style=flat-square)
## Introduction
- A Simple C++ header-only QR encode Library.
## Usage
You can also see these examples in [main.cpp](src/main.cpp)
```c++
#include "opqr/opqr.hpp"
```
### Add Data
#### constructor
```c++
QR qr1("https://github.com/caozhanhao/opqr"); // string
QR qr2(std::vector{ 0,1,2,3 });//or other containers, e.g. std::list, std::initializer_list
```
#### add_data()
```c++
QR qr1;
qr1.add_data({ 4,5,6,7 });// initializer_list or some containers
QR qr2;
qr2.add_data(789);        // integral value
```
### Specify
#### constructor
```c++
QR qr(20, ECLevel::Q, Mode::ALNUM, 0);
```
#### setter
```c++
QR qr("12345"); 
qr.set_version(10);
qr.set_level(ECLevel::L);
qr.set_mode(Mode::NUM);
qr.set_mask(6);
```
### Quiet Zone
```c++
qr.disable_quiet_zone();
```
### Output
- paint(format, filename, size)
- the image size = dimension * size x dimension * size
```c++
qr.generate().paint(pic::Format::BMP, "example.bmp", 10);
```
## Note
- There may be bugs in Kanji Mode
## Reference
[二维码生成原理](https://zhuanlan.zhihu.com/p/543574464)