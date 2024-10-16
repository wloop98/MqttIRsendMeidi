# ESP8266 MQTT 红外控制美的空调
## 简介
通过ESP8266模块，利用MQTT协议实现对美的空调的红外控制。

## 配置
- ESP8266开发板
- 3w红外发射器模块

## 依赖库
- crankyoldgit/IRremoteESP8266【需下载】
- RN02S13红外发射控制库【已在库中直接使用】
  
## 使用说明
- 配置WiFi和mqtt地址
- mqtt协议publish指令到主题inTopic上
- 指令组成为1+2个数字字符串，mode+digital
- mode：
  + 1:打开空调
  + 2:制冷热模式
  + 3:设置温度
  + 4:定时
  + 5:风速
  + 6:ECO
  + 7:防直吹
  + 8:扫风
- 指示灯已关闭，可通过注释ino文件中digitalWrite(BUILTIN_LED, HIGH);开启
 
