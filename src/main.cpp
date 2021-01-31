/*===============блок библиотек=====================*/
#include <Arduino.h>
#include <U8g2lib.h>
#include "WiFi.h"
#include <NTPClient.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define FOREVER for(;;)

/*===============блок констант=====**================*/

const char* ssid = "Tomato24";
const char* password =  "77777777";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/*===============блок переменных=====================*/
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 27, /* data=*/ 21, /* cs=*/ 26, /* dc=*/ 13, /* reset=*/ 22);  

/*===============блок функции setup==================*/
void setup(void) {
  u8g2.begin();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  timeClient.begin();
  timeClient.setTimeOffset(10800);

}

/*===============блок пользовательских функций=======*/
void show_various_fonts(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_freedoomr25_mn);
    u8g2.setCursor(10, 45);
    u8g2.setContrast(255); 
    u8g2.print(timeClient.getFormattedTime());
    } while ( u8g2.nextPage() );
}

void get_co2(void) {
  u8g2.print(F("566 ppm"));
}

void get_time(void) {
  u8g2.print(F("566 ppm"));
}

void show_all_on_oled(void) {
  u8g2.print(F("566 ppm"));
}

void show_all_on_ips(void) {
  u8g2.print(F("566 ppm"));
}

/*===============блок основной зацикленной функции=====*/
void loop(void) {
  show_various_fonts();

  timeClient.update();
  //Serial.println(timeClient.getFormattedTime());

  delay(1000);
}