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

#include <U8g2lib.h>

U8G2_SSD1306_72X40_ER_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE,7,8); 
// U8X8_SSD1306_72X40_ER_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE,7,8);   // EastRising 0.42"


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

// TwoWire i2c_rail = TwoWire(0);


void setup() {
  Serial.begin(115200);
  // client.begin(, IOT_WIFI_PASSWD);
  // pinMode(PIN_LED, OUTPUT);
  // pinMode(PIN_BUZZER, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(IOT_WIFI_NAME, IOT_WIFI_PASSWD);
  while (WiFi.waitForConnectResult(3000) != WL_CONNECTED) {
    static bool flag = false;
    digitalWrite(PIN_LED,flag);
    flag = !flag;
    Serial.print(".");
    delay(1000);
  }

  // begin_hspota();    
  // HomieNode *pNode = homie.NewNode();
  // pNode->strID = "properties";
  // pNode->strFriendlyName = "Properties";

  // // init_homie_button(pNode, PIN_BUTTON_1, "slot-1");
  // // init_homie_button(pNode, PIN_BUTTON_2, "slot-2");
  // // init_homie_button(pNode, PIN_BUTTON_3, "slot-3");

  // // init_buzzer(pNode,pPropBuzzer, "buzzer");  
  // init_homie_stuff(&homie);

  // i2c_rail.begin(8, 7, 100000);

  // Serial.println ("Init scan");

  // for (byte i = 8; i < 120; i++)
  // {
  //   i2c_rail.beginTransmission (i);          // Begin I2C transmission Address (i)
  //   if (i2c_rail.endTransmission () == 0)  // Receive 0 = success (ACK response) 
  //   {
  //     Serial.print ("Found address: ");
  //     Serial.print (i, DEC);
  //     Serial.print (" (0x");
  //     Serial.print (i, HEX);     // PCF8574 7 bit address
  //     Serial.println (")");
  //   }
  // }
  u8g2.begin();
  u8g2.setFlipMode(1);

  pinMode(PIN_VBAT, ANALOG);
  pinMode(10, OUTPUT);
  digitalWrite(10,1);
}


const uint8_t x_bits[] = { 0x40, 0xa0, 0x90, 0x48, 0x48, 0x86, 0x92, 0x00 };

void loop() {
  // if(homie.IsConnected()){
  //   handle_io_pattern(PIN_LED,PATTERN_HBEAT);
  // } else {
  //   handle_io_pattern(PIN_LED,PATTERN_ERR);
  // }

  // if (pPropBuzzer->GetValue() == "true")
  // {
  //   handle_io_pattern(PIN_BUZZER,PATTERN_HBEAT);
  // } else {
  //   handle_io_pattern(PIN_BUZZER,PATTERN_NONE);
  // }

  // ArduinoOTA.handle();
  // homie.Loop();
  // delay(100);

  char buf [15];

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_4x6_mf);

    int anal_result = analogReadMilliVolts(PIN_VBAT)*2;
    snprintf(buf,15,"%d.%02dV",anal_result/1000,(anal_result/10)%100);

    u8g2.drawBox(0,0,72,8);
    u8g2.setColorIndex(0);
    u8g2.drawStr(50,7,buf);

    snprintf(buf,15,"%ddb",WiFi.RSSI());
    u8g2.drawStr(1,7,buf);

    u8g2.setColorIndex(1);

    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(0,17,"192.168.100.144");

    snprintf(buf,15,"Uptime %d",millis());
    u8g2.drawStr(0,27,buf);

    u8g2.drawStr(0,37,"Wezwanie do zaplaty");

    // u8g2.drawXBM(0,30,8,8,x_bits);
  } while ( u8g2.nextPage() );
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

