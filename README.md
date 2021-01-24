# oled_242
test it
Тестируем новенький экран ОЛЕД на 2.42 дюйма, беленький, только что с али https://ali.onl/1LbF
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
Rx подключен на Tx2 GPIO17
Tx подключен на GPIO25 его надо сделать Rx видимо

#OLED SPD0301
CS = GPIO12
DC = GPIO13
RES = GPIO15
SDA = GPIO2
SCK = GPIO27

#ENCODER
SW = GPIO33
DT = GPIO32
CLK = GPIO38
