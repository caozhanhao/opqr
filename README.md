# opqr

## 功能
- C++二维码生成(header only)
## 示例
```c++
  std::ofstream fs("C:\\Users\\caozhanhao\\Pictures\\test.ppm");
  op::qr::QR qr;
  qr.add_data("https://gitee.com/cmvy2020/opqr");
  auto pic = qr.generate();
  pic.paint(fs);
```
## 编译
```
mkdir build && cd build 
cmake .. && make
./opQR
```
## 注意事项
- Kanji Mode目前可能存在Bug
- 目前生成图片仅支持ppm格式
## 原理
[二维码生成原理](https://zhuanlan.zhihu.com/p/543574464)