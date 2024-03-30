
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,20,4);  // Устанавливаем дисплей


const uint8_t PIN_direction_TX_RX = 11;
bool lcd_display = true;

#define diodDataPower 2
#define diodRX 3
#define diodTX 4

long count  = 0;
String receive_data = "";

//---------------------------------------
//Варианты запроса
//            /G123/
//G - Получение данных "GET"
//A - Выполнение действия "ACTION"
//---------------------------------------


void setup() {
  pinMode(PIN_direction_TX_RX,   OUTPUT);
  digitalWrite(PIN_direction_TX_RX, LOW);
  pinMode(diodRX, OUTPUT);
  pinMode(diodTX, OUTPUT);

  lcd.init();                     
  lcd.setBacklight(1);

  Serial.begin(9600);

}

void loop() {

  if(count == 200000)
  {
    send_message("G123");
  }
  else if (count == 400000)
  {
    send_message("G124");
  }
  else if (count == 600000)
  {
    send_message("G125");
  }
  else if (count > 800000)
  {
    send_message("G126");
    count = 0;
  }
  count += 1;

  if(Serial.available()>0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    receive_data = receive_message();
    if(receive_data.length() > 16 && receive_data.substring(0, 1) == "S")
    {
      LCDPrint();
    }
  }

}
//-------------ПОЛЬЗОВАТЕЛЬСКИЕ ФУНКЦИИ--------------


// Отправка сообщения
void send_message(String branch)
{
  digitalWrite(PIN_direction_TX_RX, HIGH);
  digitalWrite(diodTX, HIGH);
  delay(1);
  Serial.println(branch);
  delay(4);
  digitalWrite(PIN_direction_TX_RX, LOW);
  digitalWrite(diodTX, LOW);  
}

// Получение сообщения
String receive_message()
{
  int i=0;
  char buffer[100];
  String buffer_str;
  
  digitalWrite(diodRX, HIGH);

  delay(23);
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
    digitalWrite(diodDataPower, LOW);
  }
  return buffer_str;
}
void LCDPrint()
{
  lcd.print(receive_data.substring(4, 6));
  lcd.print("%");
  lcd.print(receive_data.substring(6, 8));
  lcd.print("%");
  lcd.print(receive_data.substring(8, 10));
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print(receive_data.substring(10, 12));
  lcd.print("%");
  lcd.print(receive_data.substring(12, 14));
  lcd.print("%");
  lcd.print(receive_data.substring(14, 16));
  lcd.print("%");  
}
