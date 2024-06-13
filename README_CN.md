# SX1276_Receive_LBJ
一个用于接受LBJ(列车接近预警信息)的项目 基于TTGO LoRa 32 v1.6.1(ESP32 + SX1276 868MHZ)

本项目修改自[RadioLib](https://github.com/jgromes/RadioLib)的Pager_Receive.ino以及[LilyGo-LoRa-Series](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series)的模板
本项目基于TTGO LoRa 32 v1.6.1开发板
本项目为了昂贵的中国铁路列车接近预警设备(又名LBJ，在821.2375MHz，遵循POCSAG协议)消息经常在CIR的机载LBJ系统上传递。提供一种可编程的廉价解决方案

## 注意事项
这是一个用于学习交流用途的开发板和亚Ghz接收实验性项目,代码质量较差,可能会遇到未知问题.

## 硬件设施
基于[TTGO LoRa 32 v1.6.1 dev board](https://www.lilygo.cc/products/lora3)开发板使用利用SX1276的FSK调制解调器接收LBJ消息。
这块主板可以在[这里](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series/blob/master/schematic/T3_V1.6.1.pdf找到.

### 附加功能
  ***注意这是可选的功能,可能需要额外设备启用该功能***
  附加功能可以通过将特定外设连接到指定引脚来启用，如下所示:

### 1. DS3231 RTC
使用OLED屏幕1306和SSDIIC共享总线.
```c++
#define I2C_SDA                     21
#define I2C_SCL                     22
```
请在断电时保持时间状态,如果没有维持时间将会在每次重启时在NTP服务器获取标准时间,否则请构建/编译时在[utilities.h](src/utilities.h)中注释掉一下行:
```c++
#define HAS_RTC // soldered an external DS3231 module.
```

#### 2. AD Buttons (WIP)
由于缺少 GPIO，使用了四键模拟按钮。
```c++
#define BUTTON_PIN                  34
```
为交互式功能提供输入，包括 OLED(WIP) 上的日志检查和设备设置。如果连接了***button_equipped***分支，而不是主分支。
有关引脚定义的更多信息，请参阅 [utilities.h](src/utilities.h)

## 一些特性
- 显示格式化后的LBJ信息
- 将格式化后的LBJ信息以文本/csv形式保存在TF卡上
- 可以作为Telent服务器向客户端发送格式化后的数据
- 从MMDVM_HS_Hat处获取针对BCH的错误修正[BCH3121.cpp](https://github.com/phl0/POCSAG_HS/blob/master/BCH3121.cpp)

## 已知问题
- TF卡中文件过多时,启动时间过长
- TF卡不支持热插拔,以断电的方式重启是不被允许的,因为在下一次信息接收的时候会报错.
- 不稳定的无线局域网链接
- 没有蜂鸣器警告
- 作为Telnet服务端不支持一对多.
- 部分错误信息会显示在显示屏上
- 文件和许可证均为初步的,需要更新或者删除.

## 关于
### 关于 LBJ 长信息
***请注意,这些内容可能违反您所在国家或者地区的法律法规,在使用之前请询问您地区的无线电管理机构及铁路公安***
这些内容并没有在 铁路标准TB/T 3504-2018 中描述,但是我在收听821.2375MHz时使用SDR收到了这种消息,一些列车发送了这些信息POCSAG地址`1234002`,这些消息的正式名称和结构是未知的，其内容是当前猜测出来的.下表列出了一个典型的long LBJ消息中的当前标识信息。

| Nibbles(4bit) | Encode  | Meaning                            |
|---------------|---------|------------------------------------|
| 0-3           | ASCII   | Type                               |
| 4-11          | Decimal | 8 digit locomotive registry number |
| 12-13         | Unknown | Unknown, usually 30                |
| 14-29         | GB2312  | Route                              |
| 30-38         | Decimal | Longitude (XXX°XX.XXXX′ E)         |
| 39-46         | Decimal | Latitude (XX°XX.XXXX′ N)           |
| 47-49         | Unknown | Unknown, usually 000               |

共50 nibbles / 200比特，以10个POCSAG帧传输。

### 2. SX1276 配置
- Freq = 821.2375MHz + 6 ppm (default)
- Mode = FSK, RxDirect (DIO2)
- Gain = 001 + LnaBoostHf (AGC off)
- RxBW = 12.5 kHz

由于开发板使用的SX1276模块上没有TCXO，利用SX1276的频率误差指示器(frequency error indicator, FEI)测量载波和前导码接收的频率误差，实现自动频率调节机制。它在接收到载波或前导信号后试图锁定信号，从而补偿由晶体或发射机引起的频率误差。这种机制可以通过串行命令`afc off`来禁用。

### 3. Telnet/串口 命令
#### 串口
- 'ping' 命令用来测试串口是否可用,返回 pong
- 'time'返回系统时间
- 'sd end' 用来结束挂载SD卡
- 'sd begin' 用来挂在sd卡
- 'men' 返回剩余可用内存 基于`esp_get_free_heap_size()`
- 'rst' 返回重置时间
- 'ppm read' 返回PPM
- 'afc off' 禁用自动频率校正
- 'afc on' 启动自动频率校正

#### Telnet
- `ping` Telnet状态测试命令，返回pong。
- `read`从TF卡上的log返回1000字节的数据。
- `log read [int]`返回TF卡上log的`[int]`字节数。
- `log status`返回TF log是否启用。
- `afc off`禁用自动频率校正。
- 'afc on'启用自动频率校正。
- `ppm [float]`将ppm设置为`[float]`。
- `bat`返回电池电压。
- `rssi`返回SX1276模块的当前rssi。
- `gain`返回SX1276模块的电流增益。
- `time`返回系统时间。
- `bye`断开Telnet连接。

### 接收失败
有时接收到的消息可能是损坏的，部分解码或错误纠正，因此可能显示不可靠的结果,如果显示了`<NUL>`、`NA`、`********`或这些字符的一部分，则意味着这部分消息已损坏.

## 引用的库/代码
[//]: # (todo: add links)
- [RadioLib](https://github.com/jgromes/RadioLib) (Modified)
- [U8G2](https://github.com/olikraus/u8g2)
- [ESP32-Arduino](https://github.com/espressif/arduino-esp32)
- [PlatformIO](https://platformio.org/)
- [ESP32AnalogRead](https://github.com/madhephaestus/ESP32AnalogRead.git) (for battery voltage checking)
- BCH3121.cpp/.h from [POCSAG_HS](https://github.com/phl0/POCSAG_HS)
- [LilyGo-LoRa-Series](https://github.com/Xinyuan-LilyGO/LilyGo-LoRa-Series)
- [ESPTelnet](https://github.com/LennartHennigs/ESPTelnet)
- [RTClib](https://github.com/adafruit/RTClib.git)
- [Multimon-ng](https://github.com/EliasOenal/multimon-ng)
- Project inspired by [GoRail_Pager](https://github.com/killeder/GoRail_Pager).
- 如果我还记得的话，我很抱歉

感谢所有相关代码的开发者们
