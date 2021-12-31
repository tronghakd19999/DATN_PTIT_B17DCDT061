/*
  IoT Manager mqtt device client

  https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
  https://itunes.apple.com/us/app/iot-manager/id1155934877

  Based on Basic MQTT example with Authentication

  Tested with Arduino IDE 1.6.12    http://arduino.cc
  ESP8266 Community Edition v2.3.0  https://github.com/esp8266/Arduino
  PubSubClient library v 1.91.1     https://github.com/Imroy/pubsubclient

  sketch version : 2.0
  IoT Manager    : any version

  toggle, range, small-badge widgets demo
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

//const char *ssid =  "Linksys01979";            // cannot be longer than 32 characters!
//const char *pass =  "WIRELESS@2017";       // WiFi password
//const char *ssid =  "BSH_Mobile_4";            // cannot be longer than 32 characters!
//const char *pass =  "VNC@2018";       // WiFi password

const char *ssid =  "Bphone";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";     // global prefix for all topics - must be some as mobile device
String deviceID = "dev01-kitchen";   // thing ID - unique device id in our project

//unsigned char g_StepMotor[6]= {0xAA,0x03,0x01,0x01,0x66,0x55};
unsigned char RelayNumBer_1[6]= {0xAA,0x03,0x01,0x03,0x66,0x55};//ok
unsigned char RelayNumBer_2[6]= {0xAA,0x03,0x01,0x11,0x66,0x55};//ok
//unsigned char RelayNumBer_3[6]= {0xAA,0x03,0x01,0x12,0x66,0x55};
unsigned char g_BlinkLedSuccess[6]= {0xAA,0x03,0x01,0x10,0x66,0x55};
unsigned char g_DimmerLed[6]= {0xAA,0x03,0x01,0x09,0x66,0x55};
int SetDimmer =0;
unsigned char Startbyte = 0x00;
unsigned char Commandbyte = 0x00;
unsigned char Databyte = 0x00;
unsigned char Stopbyte = 0x00;
unsigned char Nhietdo = 0;
unsigned char Doamdat = 0; 
unsigned char Doamkk = 0;
unsigned char Dosang = 0;
WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 10927;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "test";                              // from CloudMQTT account data
String mqttpass =  "test";                              // from CloudMQTT account data
//String mqttuser =  "quanddc";                              // from CloudMQTT account data
//String mqttpass =  "1234";                              // from CloudMQTT account data

PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname


// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 135);                        // for local broker - by address
//int    mqttport = 1883;                                    // default 1883
//String mqttuser =  "";                                     // from broker config
//String mqttpass =  "";                                     // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address

String val;
String val1;
String val2;
String val3;
String val4;
String ids = "";
int oldtime, newtime, pushtime;
int newValue;
//Serial comPort;
const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 8;
String stat        [nWidgets];
String sTopic      [nWidgets];
String color       [nWidgets];
String style       [nWidgets];
String badge       [nWidgets];
String widget      [nWidgets];
String descr       [nWidgets];
String page        [nWidgets];
String pageId      [nWidgets];
String thing_config[nWidgets];
String id          [nWidgets];
int    pin         [nWidgets];
int    defaultVal  [nWidgets];
bool   inverted    [nWidgets];

void WriteUart( unsigned char* pData, unsigned char nLen)
{
    for (unsigned char nIndex = 0; nIndex < nLen; nIndex++)
    {
       // Serial.println(pData[nIndex]-0x30, HEX);
        Serial.write(pData[nIndex]);
    }
}
String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}
String setStatus ( int s ) {
  String stat = "{\"status\":\"" + String(s) + "\"}";
  return stat;
}
void initVar() {
	//9-->0
	///////////Hien thi nhiet do
  id    [0] = "0";/////////////DinhLK changed
  page  [0] = "Kitchen";
  pageId[0] = 1;
  descr [0] = "Nhiet do ";
  widget[0] = "small-badge";
  //pin   [9] = A0;                                          // Nhiet do
  sTopic[0] = prefix + "/" + deviceID + "/Nhiet do";
  badge [0] = "\"badge\":\"badge-balanced\"";             // see http://ionicframework.com/docs/components/#colors
  //10-->1
  id    [1] = "1";/////////////DinhLK changed
  page  [1] = "Kitchen";
  pageId[1] = 1;
  descr [1] = "Do am dat";
  widget[1] = "small-badge";                                       
  sTopic[1] = prefix + "/" + deviceID + "/Do am dat";
  badge [1] = "\"badge\":\"badge-balanced\"";             // see http://ionicframework.com/docs/components/#colors
  
  //11-->2
  id    [2] = "2";/////////////DinhLK changed
  page  [2] = "Kitchen";
  pageId[2] = 1;
  descr  [2] = "Do am khong khi";
  widget[2] = "small-badge";                                         
  sTopic[2] = prefix + "/" + deviceID + "/Do am khong khi";
  badge [2] = "\"badge\":\"badge-balanced\"";             // see http://ionicframework.com/docs/components/#colors
  ///3
  id    [3] = "3";/////////////DinhLK changed
  page  [3] = "Kitchen";
  pageId[3] = 1;
  descr [3] = "Do sang";
  widget[3] = "small-badge";                                         
  sTopic[3] = prefix + "/" + deviceID + "/Do sang";
  badge [3] = "\"badge\":\"badge-balanced\"";             // see http://ionicframework.com/docs/components/#colors
  
  //Relay1
  id    [4] = "4";/////////////DinhLK changed
  page  [4] = "Kitchen";
  pageId[4] = 1;
  descr [4] = "RelayNumber1";
  widget[4] = "toggle";
  defaultVal[4] = 1;                                       // defaultVal status
  inverted[4] = true;
  sTopic[4]   = prefix + "/" + deviceID + "/RelayNumber1";
  color[4]   = "\"color\":\"blue\"";                       // black, blue, green, orange, red, white, yellow (off - grey)
  
   //Relay2
  id    [5] = "5";/////////////DinhLK changed
  page  [5] = "Kitchen";
  pageId[5] = 1;
  descr [5] = "RelayNumber2";
  widget[5] = "toggle";
  defaultVal[5] = 1;                                       // defaultVal status
  inverted[5] = true;
  sTopic[5]   = prefix + "/" + deviceID + "/RelayNumber2";
  color[5]   = "\"color\":\"blue\"";                       // black, blue, green, orange, red, white, yellow (off - grey)
   
 /* id    [5] = "5";/////////////DinhLK changed
  page  [5] = "Outdoor";
  pageId[5] = 2;
  descr [5] = "RelayNumber2";
  widget[5] = "toggle";
  pin   [5] = 2;                                           // GPIO2
  defaultVal[5] = 1;                                       // defaultVal status 0%
  inverted[5] = true;
  sTopic[5] = prefix + "/" + deviceID + "/RelayNumber2";
  color [5] = "\"color\":\"red\"";                         // black, blue, green, orange, red, white, yellow (off - grey)
*/
////-->Dimmer 1
//5--> 6
  id    [6] = "6";/////////////DinhLK changed
  page  [6] = "Kitchen";
  pageId[6] = 1;
  descr [6] = "Dimmer1";
  widget[6] = "range";
  pin[6] = 0;                                              // GPIO0 - range
  defaultVal[6] = 1023;                                    // defaultVal status 0%, inverted
  inverted[6] = true;
  sTopic[6]   = prefix + "/" + deviceID + "/dim-light";
  style[6]   = "\"style\":\"range-calm\"";                 // see http://ionicframework.com/docs/components/#colors
  badge[6]   = "\"badge\":\"badge-assertive\"";            // see http://ionicframework.com/docs/components/#colors
///7-->7
  id    [7] = "7";/////////////DinhLK changed
  page  [7] = "Kitchen";
  pageId[7] = 1;
  descr [7] = "Dimmer2";
  widget[7] = "range";
  pin[7] = 12;                                      // GPIO12 - range
  defaultVal[7] = 0;                                       // defaultVal 0%
  sTopic[7]   = prefix + "/" + deviceID + "/green";
  style[7]   = "\"style\":\"range-balanced\"";         // see http://ionicframework.com/docs/components/#colors
  badge[7]   = "\"badge\":\"badge-balanced\"";          // see http://ionicframework.com/docs/components/#colors

  
  for (int i = 0; i < nWidgets; i++) {
    if (inverted[i]) {
      if (defaultVal[i] > 0) {
        stat[i] = setStatus(0);
      } else {
        stat[i] = setStatus(1);
      }
    } else {
      stat[i] = setStatus(defaultVal[i]);
    }
  }
  //9-->0//
  /*DinhLK changed*/thing_config[0] = "{\"id\":\"" + id[0] + "\",\"page\":\"" + page[0] + "\",\"descr\":\"" + descr[0] + "\",\"widget\":\"" + widget[0] + "\",\"topic\":\"" + sTopic[0] + "\"," + badge[0] + "}"; // Nhiet do moi truong
  //10-->1
  /*DinhLK changed*/thing_config[1] = "{\"id\":\"" + id[1] + "\",\"page\":\"" + page[1] + "\",\"descr\":\"" + descr[1] + "\",\"widget\":\"" + widget[1] + "\",\"topic\":\"" + sTopic[1] + "\"," + badge[1] + "}"; // Do am dat
  //11-->2
  /*DinhLK changed*/thing_config[2] = "{\"id\":\"" + id[2] + "\",\"page\":\"" + page[2] + "\",\"descr\":\"" + descr[2] + "\",\"widget\":\"" + widget[2] + "\",\"topic\":\"" + sTopic[2] + "\"," + badge[2] + "}"; // Do am khong khi
  //-->3
  /*DinhLK changed*/thing_config[3] = "{\"id\":\"" + id[3] + "\",\"page\":\"" + page[3] + "\",\"descr\":\"" + descr[3] + "\",\"widget\":\"" + widget[3] + "\",\"topic\":\"" + sTopic[3] + "\"," + badge[3] + "}"; // Do sang
  //-->4
  /*DinhLK changed*/thing_config[4] = "{\"id\":\"" + id[4] + "\",\"page\":\"" + page[4] + "\",\"descr\":\"" + descr[4] + "\",\"widget\":\"" + widget[4] + "\",\"topic\":\"" + sTopic[4] + "\"," + color[4] + "}"; // GPIO switched On/Off by mobile widget toggle_RL1
  //-->5
  /*DinhLK changed*/thing_config[5] = "{\"id\":\"" + id[5] + "\",\"page\":\"" + page[5] + "\",\"descr\":\"" + descr[5] + "\",\"widget\":\"" + widget[5] + "\",\"topic\":\"" + sTopic[5] + "\"," + color[5] + "}"; // GPIO switched On/Off by mobile widget toggle_RL2
  //5-->6
  /*DinhLK changed*/thing_config[6] = "{\"id\":\"" + id[6] + "\",\"page\":\"" + page[6] + "\",\"descr\":\"" + descr[6] + "\",\"widget\":\"" + widget[6] + "\",\"topic\":\"" + sTopic[6] + "\"," + style[6] + "," + badge[6] + ",\"leftIcon\":\"ion-ios-rainy-outline\",\"rightIcon\":\"ion-ios-rainy\"}"; // for icons see http://ionicons.com
  //7-->7
  /*DinhLK changed*/thing_config[7] = "{\"id\":\"" + id[7] + "\",\"page\":\"" + page[7] + "\",\"descr\":\"" + descr[7] + "\",\"widget\":\"" + widget[7] + "\",\"topic\":\"" + sTopic[7] + "\"," + style[7] + "," + badge[7] + "}"; // GPIO13 B
  }
// send confirmation
void pubStatus(String t, String payload) {
  if (client.publish(t + "/status", payload)) {
    
    //Serial.println("Publish new status for " + t + ", value: " + payload);
  } else {
    //Serial.println("Publish new status for " + t + " FAIL!");
  }
}
//Publish new status for /IoTmanager/dev01-kitchen/red, value: {"status":"0"}

void pubConfig() {
  bool success;
  for (int i = 0; i < nWidgets; i = i + 1) {
    success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
    if (success) {
      //Serial.println("Publish config: Success (" + thing_config[i] + ")");
    } else {
     // Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
    }
    delay(50);
  }
  if (success) {
   // Serial.println("Publish config: Success");
  } else {
   // Serial.println("Publish config: FAIL");
  }
  for (int i = 0; i < nWidgets; i = i + 1) {
    pubStatus(sTopic[i], stat[i]);
    delay(50);
  }
}

//************************************************* Ham Callback *****************************************************//
void callback(const MQTT::Publish& sub) {
  //Serial.print("Get data from subscribed topic ");
 // Serial.print(sub.topic());
 // Serial.print(" => ");
 // Serial.println(sub.payload_string());
  ///1-->4
  if (sub.topic() == sTopic[4] + "/control") {/////////////////////DinhLK   0-->1????????????????
    if (sub.payload_string() == "0") {
      newValue = 1; // inverted
      stat[4] = stat0;
    } else {
      newValue = 0;
      stat[4] = stat1;
    }
    digitalWrite(pin[4], newValue);
    pubStatus(sTopic[4], stat[4]);
    WriteUart( RelayNumBer_1,6);
    
  } 

  else if (sub.topic() == sTopic[5] + "/control") {
    if (sub.payload_string() == "0") {
      newValue = 1; // inverted
      stat[5] = stat0;
    } else {
      newValue = 0; // inverted
      stat[5] = stat1;
    }
    digitalWrite(pin[5], newValue);
    pubStatus(sTopic[5], stat[5]); 
    WriteUart( RelayNumBer_2,6);
  } 
  
  else if (sub.topic() == sTopic[6] + "/control") {//////////////////////DinhLK changed  2--> 5
    String x = sub.payload_string();
    analogWrite(pin[6], 1023 - x.toInt());
    stat[6] = setStatus(x);
    pubStatus(sTopic[6], stat[6]);
  }
  else if (sub.topic() == sTopic[0] + "/control") {//////////////////////DinhLK changed  for hien thi nhiet do
    // Nhiet do : nothing, display only
  }
  //10-->1
  else if (sub.topic() == sTopic[1] + "/control") {//////////////////////DinhLK changed  for hien thi Do am dat
    // Do am dat: nothing, display only

  }
  //11-->2
  else if (sub.topic() == sTopic[2] + "/control") {//////////////////////DinhLK changed  for hien thi Do am khong khi
    // Do am khong khi: nothing, display only
 
  }
   //-->3
  else if (sub.topic() == sTopic[3] + "/control") {//////////////////////DinhLK changed  for hien thi Do sang
    // Do sang: nothing, display only-->3
    
  }
 else if (sub.topic() == sTopic[7] + "/control") {
    String x = sub.payload_string();
    analogWrite(pin[7], x.toInt());
    stat[7] = setStatus(x);
    pubStatus(sTopic[7], stat[7]);  
  } 
  /*else if (sub.topic() == sTopic[8] + "/control") {//////////////////////DinhLK changed  7--> 8
    String x = sub.payload_string();
    analogWrite(pin[8], x.toInt());
    stat[8] = setStatus(x);
    pubStatus(sTopic[8], stat[8]);
    ////////////////////////////////////Dimmer
    SetDimmer =(int)x.toInt()*100/1023;
    unsigned char g_DimmerLed[6]= {0xAA,0x03,0x01,0x09,SetDimmer,0x55};
    WriteUart( g_DimmerLed,6);
    }*/

    
    
   else if (sub.topic() == prefix) {
    if (sub.payload_string() == "HELLO") {
      pubConfig();
    }
  }
}
//*****************************************************************************************************************************************************
void setup() {
  initVar();
  WiFi.mode(WIFI_STA);
  
  pinMode(pin[4], OUTPUT);////////DinhLK changed
  digitalWrite(pin[4], defaultVal[4]);
  
  pinMode(pin[5], OUTPUT);////////DinhLK changed
  digitalWrite(pin[5], defaultVal[5]);

  pinMode(pin[6], OUTPUT);////////DinhLK changed
  analogWrite(pin[6], defaultVal[6]); // PWM*/

  pinMode(pin[7], OUTPUT);
  analogWrite(pin[7], defaultVal[7]); // PWM///////////////////////DinhLK changed

  //pinMode(pin[8], OUTPUT);
  //analogWrite(pin[8], defaultVal[8]); // PWM///////////////////////DinhLK changed
  
  /*pinMode(pin[3], OUTPUT);///////////////////////DinhLK changed
  digitalWrite(pin[3], defaultVal[3]);*/
  // Setup console
  Serial.begin(115200);
//unsigned char Startbyte =0x00;
//unsigned char Commandbyte =0x00;
//unsigned char Databyte =0x00;
//unsigned char Stopbyte =0x00; 
  
  
  
  delay(10);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
   // Serial.print("Connecting via WiFi to ");
  //  Serial.print(ssid);
  //  Serial.println("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
   //   Serial.println("Connecting to MQTT server ...");
      bool success;
      if (mqttuser.length() > 0) {
        success = client.connect( MQTT::Connect( deviceID ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceID );
      }
      if (success) {
        client.set_callback(callback);
       // Serial.println("Connect to MQTT server: Success");
        /////////////////////////////////////////////////////////Ket noi Success/////////////////
        WriteUart( g_BlinkLedSuccess,6);

       
        pubConfig();
        client.subscribe(prefix);                  // for receiving HELLO messages
        client.subscribe(prefix + "/+/+/control"); // for receiving GPIO control messages
       // Serial.println("Subscribe: Success");
      } else {
       // Serial.println("Connect to MQTT server: FAIL");
        delay(1000);
      }
    }

    if (client.connected()) {
      newtime = millis();
      if (newtime - oldtime > 6000) { // 7 sec
        //Serial.print("Time: ");
      //  Serial.println(val);
        //int x = analogRead(pin[4]);//////////////////////////////Doc ADC
        //val = "{\"status\":" + String(x) + "}";
        client.publish(sTopic[0] + "/status", val1);
        client.publish(sTopic[1] + "/status", val2);
        client.publish(sTopic[2] + "/status", val3);
	    	client.publish(sTopic[3] + "/status", val4);
		
      //  Serial.print("Publish ADC: ");
      //  Serial.println(val);
        oldtime = newtime;
      }
      client.loop();
    }
  }
  
  if (Serial.available() > 0) {
    Startbyte=Serial.read();
    
	if (Startbyte == 0xAA)
{
	Commandbyte=Serial.read();
	Databyte=Serial.read();
	Stopbyte=Serial.read();
	if (Stopbyte == 0x55) 
	{
	if (Commandbyte== 0x20)
	{
		Nhietdo = Databyte;
    int x1 = int (Nhietdo);
    val1 = "{\"status\":" + String(x1) + "}";
		client.publish(sTopic[0] + "/status", val1);//9-->0
	}
		if (Commandbyte== 0x21)
   {
		Doamdat = Databyte;
    int x2 = int (Doamdat);
    val2 = "{\"status\":" + String(x2) + "}";
		client.publish(sTopic[1] + "/status", val2);//10-->1

	}
	if (Commandbyte== 0x22)
   {
		Doamkk = Databyte;
    int x3 = int (Doamkk);
    val3 = "{\"status\":" + String(x3) + "}";
		client.publish(sTopic[2] + "/status", val3);//11-->2

	}
	//-->3
	if (Commandbyte== 0x23)
   {
		Dosang = Databyte;
    int x4 = int (Dosang);
    val4 = "{\"status\":" + String(x4) + "}";
		client.publish(sTopic[3] + "/status", val4);//-->3

	}
	}
}

     }
  yield();
}

	//	client.publish(sTopic[9] + "/status", val1);
	//	client.publish(sTopic[10] + "/status", val2);
