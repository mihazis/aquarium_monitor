Исходники прошивки управляющего модуля над аквариумом на базе ESP32.
Показывает время (ежедневный синк), содержание CO2 в воздухе и температуру воды в акрариуме.
Должен еще отсылать в графану, и даже вроде отсылает, но там нет пока парсера.
По таймауту выдает сообщение о необходимости внести УДО, нажатием на энкодер обнуляет таймер.
Экран OLED 2.42 дюйма.
Датчик CO2 MH-Z19B, расположен сбоку самого устройства.
Датчик температуры воды, выносной, разъем minijack, погружной.

Экран с али https://ali.onl/1LbF
Разрешение 125*64
Контроллер SPD0301

Interface definition:
GND = Power Ground
VCC = Power is positive
SCL = Clock Line
SDA = Data Cable
RES = Reset line
D/C = Data/Command
CS = Chip select

Подключение к ttgo t-display

#MH-Z19B
#define MHZ19B_TX_PIN        17
#define MHZ19B_RX_PIN        25

#OLED SPD0301
CS = GPIO26
DC = GPIO13
RES = GPIO22
SDA = GPIO21
SCK = GPIO27

#ENCODER
SW = GPIO33
DT = GPIO32
CLK = GPIO38
