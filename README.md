# Vllink Lite
## 简介
Vllink Lite是一款超低成本高性能调试器。硬件基于GD32F350制作，最小封装为QFN28，标准版本使用GD32F350G8U6（8K RAM / 64K ROM），亦可精简缓冲，使用GD32F350G6U6（6K RAM / 32K ROM），无需外部晶振。

## 成本
标准版本的Vllink Lite硬件设计中，除了阻容及接口器件外，仅需一颗LDO和一颗GD32F350G8U6，此芯片在淘宝GD32旗舰店有售，当前零售价为6.00￥。

## 功能
* 提供一路CMSIS-DAP V2协议免驱接口，支持SWD，JTAG，SWO接口，支持IAR for ARM（版本8.32.1及以上）、MDK-ARM（版本5.29）、PyOCD（需安装libusb，详情请看pyocd安装说明）
* 提供一路USB CDC接口，最高波特率可达8M

## 性能
目前DAPLink 0254固件使用CMSIS-DAP V2协议，通过SWD接口对SRAM区域的读写速度大致在110kB/S左右（默认配置，不指定时钟速率）。

Vllink Lite优化了底层传输协议，尽量使用SPI通讯，同样的测试环境下，读写速度提升到230kB/S左右。

同时SPI通讯能支持更高的时钟速率，最高可达32M，不过当时钟速率超过16M时，读写速度提升有限。更多细节可参看源码。

下表是2018年时使用专用协议的测试数据，可供参考

| 时钟频率 | 读速度（KiB/s） | 写速度（KiB/s） |
| --------| -----:  | -----:  |
| 2000    | 128.672 | 131.903 |
| 4000    | 213.449 | 223.565 |
| 8000    | 323.268 | 347.579 |
| 16000   | 402.034 | 425.731 |
| 32000   | 438.233 | 456.286 |

## 硬件制作
[原理图及PCBA制作资料](https://github.com/vllogic/vllink_lite/tree/master/hardware)

[固件](https://github.com/vllogic/vllink_lite/releases)

## 固件构建
* IAR for ARM 8.32.3
* GD32F3x0 AddOn [获取地址](http://gd32mcu.21ic.com/documents)

## 授权
GPLv3，随便玩

## 交流
欢迎加入QQ群：512256420
