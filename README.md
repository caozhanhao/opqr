<h2 align="center">
opqr
</h2> 

<p align="center">
<strong> A C++ header-only QR encode Library.</strong>
</p>

<p align="center">
  <a href="https://github.com/caozhanhao/opqr/releases" >
    <img src="https://img.shields.io/github/v/release/caozhanhao/opqr?style=flat-square" alt="Release" />  
  </a>
</p>

- header only
- easy to use

### Setup

just `#include "opqr/opqr.hpp"`.

### Usage

#### Add Data

##### constructor

```c++
QR qr1("https://github.com/caozhanhao/opqr"); // string
QR qr2(std::vector{ 0,1,2,3 });//or other containers, e.g. std::list, std::initializer_list
```

##### add_data()

```c++
QR qr1;
qr1.add_data({ 4,5,6,7 });// initializer_list or some containers
QR qr2;
qr2.add_data(789);        // integral value
```

#### Specify

##### constructor

```c++
QR qr(20, ECLevel::Q, Mode::ALNUM, 0);
```

##### setter

```c++
QR qr("12345"); 
qr.set_version(10);
qr.set_level(ECLevel::L);
qr.set_mode(Mode::NUM);
qr.set_mask(6);
```

#### Quiet Zone

```c++
qr.disable_quiet_zone();
```

#### Output

##### Support Format

- JPG
- PNG
- TGA
- BMP
- ANSI

##### paint(format, filename/ostream, enlarge)

- the image size = dimension * enlarge x dimension * enlarge

##### paint(format, filename/ostream, width, height)

- the image size = width x height

```c++
auto pic = qr.generate();
pic.paint(pic::Format::BMP, "example.bmp", 10);
std::ofstream fs("examples/example2.png", std::ios::binary | std::ios::out);
pic.paint(pic::Format::PNG, fs, 1024, 1024);
fs.close();
```

For more examples, see [here](examples/src/main.cpp)

### Note

- There may be bugs in Kanji Mode

### Dependency

- [stb](https://github.com/nothings/stb) (also header-only)
- C++ 17

### Reference

[二维码生成原理](https://zhuanlan.zhihu.com/p/543574464)