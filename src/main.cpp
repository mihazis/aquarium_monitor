/*===============блок библиотек=====================*/
#include <Arduino.h>
#include <U8g2lib.h>
#include "WiFi.h"
#include <NTPClient.h>
#include <ErriezMHZ19B.h>
#include "GyverEncoder.h"


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#define FOREVER for(;;)


#if defined(ARDUINO_ARCH_ESP32)
    #define MHZ19B_TX_PIN        17
    #define MHZ19B_RX_PIN        25

    #include <SoftwareSerial.h>          // Use software serial
    SoftwareSerial mhzSerial(MHZ19B_TX_PIN, MHZ19B_RX_PIN);
#else
    #error "May work, but not tested on this target"
#endif

/*===============Энкодер===========**================*/
#define CLK 38
#define DT 32
#define SW 33
Encoder enc1(CLK, DT, SW);

/*===============таймеры===========**================*/
#define PERIOD_1 5000              // перерыв между включением 
#define PERIOD_2 60000              // время работы 
unsigned long timer_1, timer_2;
/*===============блок констант=====**================*/

const char* ssid = "Tomato24";
const char* password =  "77777777";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/*===============блок переменных=====================*/
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 27, /* data=*/ 21, /* cs=*/ 26, /* dc=*/ 13, /* reset=*/ 22);  
ErriezMHZ19B mhz19b(&mhzSerial);
int16_t result;
String stringOne = "Hello String";
String stringTwo = "Hello String";
String stringThree = "Hello String";
String stringInfo = "Hello String";
String stringOnePlus = "Hello String";
boolean one_time_flag1 = false;
boolean one_time_flag2 = true;
int contrast = 1;

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
  enc1.setType(TYPE1);
  timer_1 = millis();
  timer_2 = millis();
  
  char firmwareVersion[5];
  mhzSerial.begin(9600);
  while ( !mhz19b.detect() ) {
        Serial.println(F("Detecting MH-Z19B sensor..."));
        delay(2000);
    };
  while (mhz19b.isWarmingUp()) {
        Serial.println(F("Warming up..."));
        delay(2000);
    };
  mhz19b.getVersion(firmwareVersion, sizeof(firmwareVersion));
  Serial.println(mhz19b.getAutoCalibration() ? F("On") : F("Off"));
  
}

/*===============блок пользовательских функций=======*/
void show_various_fonts(void) {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_freedoomr25_mn);
    u8g2.setCursor(10, 35);
    u8g2.setContrast(5); 
    u8g2.print(timeClient.getFormattedTime());
    u8g2.setFont(u8g2_font_4x6_tn);
    u8g2.setCursor(0,5);
    u8g2.print(WiFi.localIP());    
    } while ( u8g2.nextPage() );
}
void printErrorCode(int16_t result)
{
    // Print error code
    switch (result) {
        case MHZ19B_RESULT_ERR_CRC:
            Serial.println(F("CRC error"));
            break;
        case MHZ19B_RESULT_ERR_TIMEOUT:
            Serial.println(F("RX timeout"));
            break;
        default:
            Serial.print(F("Error: "));
            Serial.println(result);
            break;
    }
}
int16_t get_co2() {
    // Minimum interval between CO2 reads is required
    if (mhz19b.isReady()) {
        // Read CO2 concentration from sensor
        result = mhz19b.readCO2();

        // Print result
        if (result < 0) {
            // An error occurred
            printErrorCode(result);
        } else {
            return result;
        }
    }
return 0; // без этого нуля варнинг при компиляции. надо понять почему в обычных функциях не
}
void show_screen(void) {
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_freedoomr25_mn);
    u8g2.setCursor(10, 35);
    u8g2.setContrast(5); 
    u8g2.print(timeClient.getFormattedTime());
    u8g2.setFont(u8g2_font_4x6_tn);
    u8g2.setCursor(0,5);
    u8g2.print(WiFi.localIP());    
    } while ( u8g2.nextPage() );
}
void show_info(String stringInfo) {
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_4x6_tn);
    u8g2.setCursor(0, 55);
    u8g2.print(stringInfo);
    delay(1000);
  } while ( u8g2.nextPage() );
}
void show_all(String stringOne, String stringTwo, String stringThree, int contrast) {
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setContrast(contrast);

    u8g2.setFont(u8g2_font_cardimon_pixel_tn);
    u8g2.setCursor(21, 62);
    u8g2.print(stringOne);

    u8g2.setFont(u8g2_font_freedoomr25_mn);
    u8g2.setCursor(10, 30);
    u8g2.print(stringTwo);
    
    //u8g2.setFont(u8g2_font_4x6_tn);
    //u8g2.setCursor(10,50);
    //u8g2.print(stringThree);
    
  } while ( u8g2.nextPage() );
}  
void mhz19_heating(void) {
  char firmwareVersion[5];
  mhzSerial.begin(9600);
  while ( !mhz19b.detect() ) {
        Serial.println(F("Detecting MH-Z19B sensor..."));
        delay(2000);
    };
  while (mhz19b.isWarmingUp()) {
        Serial.println(F("Warming up..."));
        delay(2000);
    };
  mhz19b.getVersion(firmwareVersion, sizeof(firmwareVersion));
  Serial.println(mhz19b.getAutoCalibration() ? F("On") : F("Off"));
} 

void first_timer() {
  if (millis() - timer_1 > PERIOD_1) {
    timer_1 = millis();
    //String stringOne = String(get_co2());
    String stringOne = String(get_co2());
    String stringOnePlus = "ppm";
    stringOne += stringOnePlus;
    String stringTwo = String(timeClient.getFormattedTime());
    String stringThree = String(contrast);
    show_all(stringOne, stringTwo, stringThree, contrast);
    }
}
void second_timer() {
  if (millis() - timer_2 > PERIOD_2) {
    timer_2 = millis();
    timeClient.update();
  }
}


void loop(void) {
  enc1.tick();
  first_timer();      //по первому таймеру обновляется oled экран
  second_timer();     //по второму таймеру синхронизируется время
  
  if(one_time_flag1){ //когда допилю включить, чтоб прогрев запускался автоматом один раз при запуске
    mhz19_heating();
    String stringOne = "WARM OK!";
    //show_status_line(stringOne);
    one_time_flag1 = false;    
  }
  if (enc1.isPress()){
    mhz19_heating();
    }
  if (enc1.isRight()){
    contrast = contrast + 30;
    }
  if (enc1.isLeft()){
    contrast = contrast - 30;
    }
}