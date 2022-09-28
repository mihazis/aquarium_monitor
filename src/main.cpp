/*===============блок библиотек=====================*/
#include <Arduino.h>
#include <U8g2lib.h>
#include "WiFi.h"
#include <NTPClient.h>
#include <ErriezMHZ19B.h>
#include "GyverEncoder.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>

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

/*=========Температурный датчик==DS18B20=(mini jack)=*/
#define ONE_WIRE_BUS 15
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/*===============таймеры===========**================*/
#define PERIOD_1 5000              // перерыв между включением 
#define PERIOD_2 180000              // как часто синхронизировать время
#define PERIOD_3 10000               // как часто отправлять данные на mqtt
#define PERIOD_4 86400000            // на сколько времени обновлять триггер по CO2
unsigned long timer_1, timer_2, timer_3, timer_4;
/*===============блок констант=====**================*/

const char* ssid = "Tomato24";
const char* password =  "77777777";
//const char* mqtt_server = "192.168.1.33";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/*===============блок переменных=====================*/
U8G2_SSD1309_128X64_NONAME2_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 27, /* data=*/ 21, /* cs=*/ 26, /* dc=*/ 13, /* reset=*/ 22);  
ErriezMHZ19B mhz19b(&mhzSerial);
int16_t result;
int16_t counter = 0;
String stringOne = "Hello String";
String stringTwo = "Hello String";
String stringThree = "Hello String";
String stringInfo = "Hello String";
String stringOnePlus = "Hello String";
boolean one_time_flag1 = true; //cтартовать автоматом прогрев MH-Z19
boolean one_time_flag2 = true;  //стартовать автоматом подключение к mqtt
boolean trigger_udo = false;    //отображение триггера по УДО
int contrast = 1;

/*============== настройка MQTT =====================*/
WiFiClient espClient;
PubSubClient client(espClient);
#define TEMP_TOPIC    "smarthome/aqua/temp"
#define LED_TOPIC     "smarthome/aqua/led" /* 1=on, 0=off */
float temperature = 0;
char msg[20];

/*===============блок функции setup==================*/
void setup(void) {
  u8g2.begin();
  Serial.begin(115200);
  sensors.begin();
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
  timeClient.update();
  enc1.setType(TYPE1);
  timer_1 = millis();
  timer_2 = millis();
  timer_3 = millis();
  
}

/*===============блок пользовательских функций=======*/
void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
 
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  if ((char)payload[0] == '1') {
    Serial.print("OK"); 
  } else {
    Serial.print("NOT OK");
  }
 
}
void mqttconnect() {
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}
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
void show_all(String stringOne, String stringTwo, String stringThree) {
  u8g2.firstPage();
  do {
    u8g2.clearBuffer();
    u8g2.setContrast(1);

    /*первая цифра - отступ слева, вторая - отступ сверху*/
    
    u8g2.setFont(u8g2_font_wqy12_t_gb2312); /*подпись до CO2*/
    u8g2.setCursor(0, 64);
    u8g2.print("co2:");
    
    u8g2.setFont(u8g2_font_gb24st_t_1); /*количество CO2*/
    u8g2.setCursor(20, 64);
    u8g2.print(stringOne);

    u8g2.setFont(u8g2_font_freedoomr25_mn); /*часы*/
    u8g2.setCursor(10, 30);
    u8g2.print(stringTwo);

    u8g2.setFont(u8g2_font_gb24st_t_1); /*температура*/
    u8g2.setCursor(87, 64);
    u8g2.print(stringThree);

    u8g2.setFont(u8g2_font_wqy12_t_gb2312); /*подпись до температуры*/
    u8g2.setCursor(80, 64);
    u8g2.print("t:");

    u8g2.setFont(u8g2_font_wqy12_t_gb2312); /*триггер для УДО*/
    u8g2.setCursor(70, 40);
    if (trigger_udo){
    u8g2.print("udo today!");
    }
  } while ( u8g2.nextPage() );
}  
void mhz19_heating(void) {
  char firmwareVersion[5];
  mhzSerial.begin(9600);
  while ( !mhz19b.detect() ) {
        delay(2000);
    };
  while (mhz19b.isWarmingUp()) {
        delay(2000);
    };
  mhz19b.getVersion(firmwareVersion, sizeof(firmwareVersion));
  Serial.println(mhz19b.getAutoCalibration() ? F("On") : F("Off"));
} 
void first_timer() {
  if (millis() - timer_1 > PERIOD_1) {
    timer_1 = millis();
    sensors.requestTemperatures();
    temperature = sensors.getTempCByIndex(0);
    String stringOne = String(get_co2());
    String stringTwo = String(timeClient.getFormattedTime());
    String stringThree = String(temperature);
    stringThree.remove(4, 1); // удаляем сотые доли у градусов
    show_all(stringOne, stringTwo, stringThree);
    }
}
void second_timer() {
  if (millis() - timer_2 > PERIOD_2) {
    timer_2 = millis();
    timeClient.update();
  }
}
void third_timer() {
  if (millis() - timer_3 > PERIOD_3) {
    timer_3 = millis();
    snprintf (msg, 20, "%lf", temperature);
    client.publish(TEMP_TOPIC, msg);
   }
}
void fourth_timer() {
  if (millis() - timer_4 > PERIOD_4) {
    timer_4 = millis();
    trigger_udo = true;
   }
}
void loop(void) {
  enc1.tick();

  if(one_time_flag1){ //один раз в начале делаем прогрев датчика CO2
    mhz19_heating();
    one_time_flag1 = false;    
  }

  //if(one_time_flag2){ //один раз в начале задаём адрес mqtt сервера
  //  client.setServer(mqtt_server, 8883);
  //  client.setCallback(receivedCallback);
  //  one_time_flag2 = false;    
  //}

  //if (!client.connected()) {
  //  mqttconnect();
  //}

  // client.loop();

  first_timer();      //по первому таймеру обновляется oled экран
  second_timer();     //по второму таймеру синхронизируется время
  third_timer();      //по третьему таймеру отправляются данные на mqtt-сервер
  fourth_timer();     //по четвёртому таймеру обновляется триггер
  

  if (enc1.isPress()){
    trigger_udo = false;
    timer_4 = millis();
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);
        u8g2.setCursor(10, 35);
        u8g2.print("done!");
    } 
    while ( u8g2.nextPage() );
  }



  /*блоком ниже проверял, нет ли затупов при обработке всего, что лежит в loop */
  /*u8g2.firstPage(); 
  do {
    u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    u8g2.setCursor(10, 35);
    u8g2.print(counter);
    counter = counter + 1;
    } while ( u8g2.nextPage() );  */
}