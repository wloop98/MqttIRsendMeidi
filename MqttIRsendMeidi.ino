/*
 * 美的空调遥控器RN02S13红外发射控制库，需配合IRremoteESP8266库使用。
 * 可以控制的参数：温度（精确到0.5），模式，风速，开关机，定时，扫风，ECO，防直吹。
 * 温度设置，17-30，分辨率0.5
 * 设置模式，0自动，1制冷，2制热，3抽湿，4送风
 * 设置风速，0自动，1为20%，2为40%，3为60%，4为80%，5为100%
 * ECO，扫风，防直吹，参数1为打开，参数0为关闭。
 */
#include <Arduino.h>
#include "IRsendMeidi.h"
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
const uint8_t IR_LED = 4;        // 设置发送信号的GPIO引脚号，官方推荐4
IRsendMeidi irsendmeidi(IR_LED); // 声明类对象
//wifi名称
const char *ssid = "";
//wifi密码
const char *password = "";
//mqtt服务器地址
const char *mqtt_server = "";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setup_wifi()
{

    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    // Serial.print("Message arrived [");
    // Serial.print(topic);
    // Serial.print("] ");
    String str;
    for (int i = 0; i < length; i++)
    {
        str += String((char)payload[i]);
        Serial.print((char)payload[i]);
    }
    Serial.println();
    if (str.length() == 3)
    {
        // Serial.println(str);
        int Mmode = str.substring(0, 1).toInt();
        int Mdig = str.substring(1, 3).toInt();
        Serial.println(Mmode);
        Serial.println(Mdig);
        switch (Mmode)
        {
            case 1:
                irsendmeidi.setPowers(Mdig); // 打开空调
                break;
            case 2:
                irsendmeidi.setModes(Mdig); // 设置为模式
                break;
            case 3:
                irsendmeidi.setTemps(Mdig); // 打开温度
                break;
            case 4:
                irsendmeidi.setTimers(Mdig); // 定时
                break;
            case 5:
                irsendmeidi.setFanSpeeds(Mdig); // 设置风速为20
                break;
            case 6:
                irsendmeidi.setEco(Mdig); // 打开ECO
                break;
            case 7:
                irsendmeidi.setFZC(Mdig); // 打开防直吹模式
                break;
            case 8:
                irsendmeidi.setSwingUD(Mdig); // 打开上下扫风
                break;
        }
    }
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect
        if (client.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            // client.publish("outTopic", "hello world");
            // ... and resubscribe
            client.subscribe("inTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup()
{
    pinMode(BUILTIN_LED, OUTPUT); // Initialize the BUILTIN_LED pin as an output
      // pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); 
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    delay(200);
    irsendmeidi.begin_2();                                     // 初始化
    irsendmeidi.setZBPL(40);                                   // 设置红外载波频率，单位kHz,不调用此函数则默认38，由于未知原因，我设置为40，示波器测得频率为38左右，当发送信号后没反应时，尝试更改此值。
    irsendmeidi.setCodeTime(500, 1600, 550, 4400, 4400, 5220); // 设置信号的高低电平占比，分别为标记位，1位，0位，前导码低电平，前导码高电平，间隔码高电平
                                                               // 不调用此函数默认为（500,1600,550,4400,4400,5220）
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }
    client.loop();

}