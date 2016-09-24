# zxcarddumpreader  [![travis-ci][1]][2] [![GPLv3][3]][4]
[1]: https://travis-ci.org/zhangjingye03/zxcarddumpreader.svg?branch=master
[2]: https://travis-ci.org/zhangjingye03/zxcarddumpreader
[3]: https://img.shields.io/badge/license-GPLv3-blue.svg
[4]: LICENSE

## 执信中学饭卡dump文件读取工具（未完成）

### 使用方法

```
$ gcc -o zxcarddumpreader zxcarddumpreader.c
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
Last balance: RMB 125.80
Last payment at month 08
Last total payment 345 times

--- Zone A ---
Current balance: RMB 100.86
Current payment at month 04
Total payment 346 times

--- Zone C ---
Card No. 20146317
Sec1 keyA: ffffffffffff
Sec1 keyB: ffffffffffff
Sec1 ACs: ff078069

------------Sector 2------------

Card No. 20146317
Unknown characteristic:
0xd5 0x94 0x34 0x06 0xa2 0xf9
Sec2 keyA: ffffffffffff
Sec2 keyB: ffffffffffff
Sec2 ACs: ff078069

-------------E N D--------------


```

饭卡dump文件请自行获取；已在`Cygwin x64`、`Travis Docker Ubuntu/Linaro 4.6.3`、`Kali Linux ARMv8`等环境下编译并测试成功。

### 饭卡结构

具体结构见`zxcard.h`中的结构体声明。详细信息请用`WinHex`加载`WinHex.pos`并读取饭卡dump文件。

P.S. 里面还有大堆大堆的不确定的block。如果发现已知block判断错误或者发现某个block新的作用，请开pr/issue告诉我。

![demo](https://raw.githubusercontent.com/zhangjingye03/zxcarddumpreader/master/demo.png)

### License

GNU GENERAL PUBLIC LICENSE Version 3
