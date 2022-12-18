#include <Arduino.h>
#include <LeifHomieLib.h>

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#else
#include "WiFi.h"
#endif

#include <ArduinoOTA.h>
#include <initializer_list>
#include <string>

// #include <Button.h>

#include "pinDefs.h"
#include "secrets.h"
#include "config.h"
#include "commonFwUtils.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

HomieDevice homie;
// HomieProperty *pPropBuzzer = NULL;
// HomieProperty *pPropButton1 = NULL;
// HomieProperty *pPropButton2 = NULL;
// HomieProperty *pPropButton3 = NULL;

// Button* buttons[3];

// void init_homie_button(HomieNode* pNode, int pin_num, String id){
//   Button* pButton = new Button(pin_num, INPUT);
//   HomieProperty *pProperty = pNode->NewProperty();

//   pProperty->strFriendlyName = id;
//   pProperty->strID = id;
//   pProperty->datatype = homieBool;
//   pProperty->SetBool(pButton->isPressed());
//   pProperty->strFormat = "";
//   pButton->onChange([pButton,pProperty]() {
//     pProperty->SetBool(pButton->isPressed());
//   });
// }

// void init_buzzer(HomieNode* pNode, HomieProperty* pProperty, String id){
//   pProperty = pNode->NewProperty();
//   pProperty->strFriendlyName = "Annoying " + id;
//   pProperty->strID = id;
//   pProperty->SetRetained(true);
//   pProperty->SetSettable(true);
//   pProperty->datatype = homieBool;
//   pProperty->SetBool(false);
//   pProperty->strFormat = "";
//   //callback is actually not needed cause buzzer is handled in loop
//   // pPropBuzzer->AddCallback([](HomieProperty *pSource) {
//   // 	Serial.printf("%s is now %s\n",pSource->strFriendlyName.c_str(),pSource->GetValue().c_str()); 
//   // });
// }

void init_homie_stuff(HomieDevice* pHomie){
  pHomie->strFriendlyName = friendlyName;
  #if defined(APPEND_MAC_TO_HOSTNAME)
    char out[20];
    sprintf(out, "%s-%X",hostname, 2137);
    pHomie->strID = out;
  #else
    pHomie->strID = hostname;
  #endif
  pHomie->strID.toLowerCase();

  pHomie->strMqttServerIP = "192.168.88.20";
	pHomie->strMqttUserName = MQTT_USERNAME;
	pHomie->strMqttPassword = MQTT_PASSWD;
  pHomie->Init();
}

TwoWire i2c_rail = TwoWire(0);
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &i2c_rail);


void setup() {
  Serial.begin(115200);
  // client.begin(, IOT_WIFI_PASSWD);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);
  while (WiFi.waitForConnectResult(3000) != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED,flag);
    flag = !flag;
    Serial.print(".");
  }

  begin_hspota();    
  HomieNode *pNode = homie.NewNode();
  pNode->strID = "properties";
  pNode->strFriendlyName = "Properties";

  // init_homie_button(pNode, PIN_BUTTON_1, "slot-1");
  // init_homie_button(pNode, PIN_BUTTON_2, "slot-2");
  // init_homie_button(pNode, PIN_BUTTON_3, "slot-3");

  // init_buzzer(pNode,pPropBuzzer, "buzzer");  
  init_homie_stuff(&homie);

  i2c_rail.begin(3, 2, 100000);

  Serial.println ("Init scan");

  for (byte i = 8; i < 120; i++)
  {
    i2c_rail.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (i2c_rail.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
    }
  }

  delay(1000);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  display.display();

  delay(100000);

}

void loop() {
  if(homie.IsConnected()){
    handle_io_pattern(PIN_LED,PATTERN_HBEAT);
  } else {
    handle_io_pattern(PIN_LED,PATTERN_ERR);
  }

  // if (pPropBuzzer->GetValue() == "true")
  // {
  //   handle_io_pattern(PIN_BUZZER,PATTERN_HBEAT);
  // } else {
  //   handle_io_pattern(PIN_BUZZER,PATTERN_NONE);
  // }

  ArduinoOTA.handle();
  homie.Loop();
  delay(100);
}



  // // Clear the buffer.
  // display.clearDisplay();
  // display.display();

  // Serial.println("IO test");

  // pinMode(BUTTON_A, INPUT_PULLUP);
  // pinMode(BUTTON_B, INPUT_PULLUP);
  // pinMode(BUTTON_C, INPUT_PULLUP);

  // // text display tests
  // display.setTextSize(1);
  // display.setTextColor(SSD1306_WHITE);
  // display.setCursor(0,0);
  // display.print("Connecting to SSID\n'adafruit':");
  // display.print("connected!");
  // display.println("IP: 10.0.1.23");
  // display.println("Sending val #0");
  // display.setCursor(0,0);
  // display.display(); // actually display all of the above

