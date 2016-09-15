# zxcarddumpreader  [![travis-ci][1]][2] [![GPLv3][3]][4]
[1]: https://travis-ci.org/zhangjingye03/zxcarddumpreader.svg?branch=master
[2]: https://travis-ci.org/zhangjingye03/zxcarddumpreader
[3]: https://img.shields.io/badge/license-GPLv3-blue.svg
[4]: LICENSE

## 执信中学饭卡dump文件读取工具（未完成）

### 使用方法

```
$ gcc -o zxcarddumpreader zxcard.c
$ ./zxcarddumpreader
No input file path specified.

Usage:  zxcarddumpreader <file>
The parameter <file> is your zx card dump file
Either 1KB version with all sectors or 192B version with first three blocks is acceptable.

$ ./zxcarddumpreader demo.bin

ZX Card File Dump Reader

File size: 192

------------Sector 0------------
Card UID: 2333333333
Card default sign: bcdefghi
Sec0 keyA: ffffffffffff
Sec0 keyB: ffffffffffff
Sec0 ACs: ff078069

------------Sector 1------------
Zone A checksum = 0x1ff, OK.
Zone B checksum = 0x1ff, OK.

--- Zone B ---
Last balance: RMB 0125.80
Last payment at POS 08
Continuous payment 01 times
Last total payment 345 times

--- Zone A ---
Current balance: RMB 0100.86
Current payment at POS 04
Continuous payment 01 times
Total payment 346 times

--- Zone C ---
Card No.20146317
Sec0 keyA: ffffffffffff
Sec0 keyB: ffffffffffff
Sec0 ACs: ff078069

```

饭卡dump文件请自行获取；已在`Cygwin x64`环境下编译并测试成功。

### 饭卡结构

具体结构见`zxcard.c`中的结构体声明。详细信息请用`WinHex`加载`WinHex.pos`并读取饭卡dump文件。

P.S. 里面还有大堆大堆的不确定的block。如果发现已知block判断错误或者发现某个block新的作用，请开pr/issue告诉我。

![demo](https://raw.githubusercontent.com/zhangjingye03/zxcarddumpreader/master/demo.png)

### License

GNU GENERAL PUBLIC LICENSE Version 3
