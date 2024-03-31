#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей

#define sensorPower_1 2
#define sensorPower_2 3
#define sensorPower_3 4
#define sensorPower_4 5
#define sensorPower_5 6
#define sensorPower_6 7
#define fitoPower 8
#define btn_fito 9
#define btn_1 10
const uint8_t     PIN_direction_TX_RX = 11;
#define diodRX 12
#define diodTX 13

#define sensorPin_1 A0
#define sensorPin_2 A1
#define sensorPin_3 A2
#define sensorPin_4 A3
#define sensorPin_5 A6
#define sensorPin_6 A7

bool lcd_display = true;

String receive_data = "";
String data = "";
long count  = 0;
bool valOld = 0;
bool btnFitoOld = 0;
bool statusFitoLight = false;
char buffer[100];

String BRANCHNAME = "123";

void setup()
{
  pinMode(PIN_direction_TX_RX,   OUTPUT);
  digitalWrite(PIN_direction_TX_RX, LOW);
  
  lcd.init();                     
  lcd.setBacklight(1);

  pinMode(btn_1, INPUT);
  pinMode(btn_fito, INPUT);
  pinMode(sensorPower_1, OUTPUT);
  pinMode(sensorPower_2, OUTPUT);
  pinMode(sensorPower_3, OUTPUT);
  pinMode(sensorPower_4, OUTPUT);
  pinMode(sensorPower_5, OUTPUT);
  pinMode(sensorPower_6, OUTPUT);
  pinMode(fitoPower, OUTPUT);
  pinMode(diodRX, OUTPUT);
  pinMode(diodTX, OUTPUT);

  Serial.begin(9600);  
}
void loop()
{

  //Читаем Кнопку выключения дисплея
  //
  bool val = digitalRead(btn_1);
  if (val == 1 && valOld == 0)
  {
    valOld = 1;
    if (lcd_display == true)
    {
      lcd.setBacklight(0);
      lcd_display = false;
    }
    else
    {
      lcd.setBacklight(1);
      lcd_display = true;
    }
  }
  if (val == 0 && valOld == 1)
  {
    valOld = 0;
  }


  
  //Читаем Кнопку выключения Фитолампы
  bool valFitoNew = digitalRead(btn_fito);
  if (valFitoNew == 1 && btnFitoOld == 0)
  {
    btnFitoOld = 1;
    if (statusFitoLight == true)
    {
      //lcd.setBacklight(0);
      digitalWrite(fitoPower, LOW);
      statusFitoLight = false;
    }
    else
    {
      //lcd.setBacklight(1);
      digitalWrite(fitoPower, HIGH);
      statusFitoLight = true;
    }
  }
  if (valFitoNew == 0 && btnFitoOld == 1)
  {
    btnFitoOld = 0;
  }



  //Прием/Отправка сообщения
  if(Serial.available()>0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    receive_data = receive_message();
    if(receive_data.substring(0,1) == "G")
    {
      if(receive_data.substring(1,4) == BRANCHNAME)
        {
          data = get_all_data();
          send_message("S" + BRANCHNAME + data);
          count = 0;
        }
    }
    else if(receive_data.substring(0,1) == "A")
    {
      
    }
    LCDPrint();
  }

  if(count > 1000000)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    data = get_all_data();
    LCDPrint();
    count = 0;
  }
  count++;
  
}
//-------------ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ--------------

void LCDPrint()
{
  lcd.print(data.substring(0, 2));
  lcd.print("% ");
  lcd.print(data.substring(2, 4));
  lcd.print("% ");
  lcd.print(data.substring(4, 6));
  lcd.print("% ");
  lcd.setCursor(0, 1);
  lcd.print(data.substring(6, 8));
  lcd.print("% ");
  lcd.print(data.substring(8, 10));
  lcd.print("% ");
  lcd.print(data.substring(10, 12));
  lcd.print("% ");  
}

String get_all_data()
{
  data = ""; 
  digitalWrite(sensorPower_1, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_1));
  digitalWrite(sensorPower_1, LOW);

  digitalWrite(sensorPower_2, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_2));
  digitalWrite(sensorPower_2, LOW);

  digitalWrite(sensorPower_3, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_3));
  digitalWrite(sensorPower_3, LOW);

  digitalWrite(sensorPower_4, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_4));
  digitalWrite(sensorPower_4, LOW);

  digitalWrite(sensorPower_5, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_5));
  digitalWrite(sensorPower_5, LOW);

  digitalWrite(sensorPower_6, HIGH);
  delay(5);
  data += ground(analogRead(sensorPin_6));
  digitalWrite(sensorPower_6, LOW);
  
  return data;
}

// Замер влажности
String ground(int sensor)
{
  if(sensor >= 1000)
  {
    sensor = 0;
  }
  else
  {
    sensor = 1000 - sensor;
    sensor = map(sensor, 1, 1000, 1, 100);
  }
  if(sensor < 10)
  {
    return "0" + (String)sensor;
  }
  else
  {
    return (String)sensor;
  }
  return (String)sensor;
}

// Отправка сообщения
void send_message(String branch)
{
  digitalWrite(PIN_direction_TX_RX, HIGH);
  digitalWrite(diodTX, HIGH);
  delay(1);
  Serial.println(branch);
  delay(23);
  digitalWrite(PIN_direction_TX_RX, LOW);
  digitalWrite(diodTX, LOW);  
}

// Получение сообщения
String receive_message()
{
  int i=0;
  char buffer[100];
  String buffer_str = buffer;
  
  digitalWrite(diodRX, HIGH);
  delay(3);
  while(Serial.available())
  {
    buffer[i++] = Serial.read();
  }   
  digitalWrite(diodRX, LOW);
  if(i>0)
  {
    buffer[i++]='\0';
    Serial.println(buffer);
    buffer_str = buffer;
    digitalWrite(diodRX, LOW);
  }
  return buffer_str;
}
