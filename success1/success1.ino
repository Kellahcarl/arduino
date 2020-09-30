#include "ESP8266.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2);

#define SSID        "kejani"
#define PASSWORD    "qwertyuiop"

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int trigger_pin = 2;
int echo_pin = 3;
int time;
int water_level; 

ESP8266 wifi(Serial1);

void setup(void)
{
    Serial.begin(9600);
    sensors.begin();
    lcd.init();
    lcd.backlight();
    pinMode (trigger_pin, OUTPUT); 
    pinMode (echo_pin, INPUT);
    Serial.print("setup begin\r\n");
    lcd.setCursor(0, 0);
    lcd.print("wifi module on!");    
    Serial.print("connecting to Wifi");
    Serial1.println("AT+GMR");
    delay(1000);
    printResponse();
        
    
    if (wifi.setOprToStation()) {
        Serial.print("to station ok\r\n");
    } else {
        Serial.print("to station err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    Serial.print("setup end\r\n");
}
void printResponse() {
  while (Serial1.available()) {
    Serial.println(Serial1.readStringUntil('\n')); 
  }
}
void sendData()
{
  Serial1.println("AT+CIPMODE=0");
  delay(1000);
  printResponse();

  Serial1.println("AT+CIPMUX=0");
  delay(1000);
  printResponse();

  Serial1.println("AT+CIPSTART=\"TCP\",\"192.168.137.1\",80");
  lcd.setCursor(0, 0);
  lcd.print("connecting to ");
  lcd.setCursor(1, 1);
  lcd.print("server");
  delay(2000);
  printResponse();

  Serial1.println("AT+CIPSEND=130");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("sending data ");
  delay(5000);
  printResponse();

  int sensorValue = analogRead(A1);
  float Temp = sensors.getTempCByIndex(0);
  float turbidity = sensorValue * (5.0 / 1024.0);
  float ph = 4;
  water_level = (time * 0.034) / 2;
  
  //float value=Temp;
  String part1 = "GET /write_data.php?";
  String part2 = "temp="+String(Temp)+"&";
  String part3 = "turbidity="+String(turbidity)+"&";
  String part4 = "ph="+String(ph)+"&";
  String part5 = "water_level="+String(water_level);
  String part6 = " HTTP/1.0\r\nHost: 192.168.137.1\r\n\r\n";
  String cmd = part1+part2+part3+part4+part5+part6;
  //String cmd = "POST /write_data.php HTTP/1.1\r\nHost: 192.168.137.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-length: 8\r\n\r\nvalue=60\r\n\r\n";
  Serial1.println(cmd);
  //Serial.println(cmd);
  
  delay(30000);
  printResponse();
  
  Serial1.println("AT+CIPCLOSE");
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.print("data sent ");
  delay(5000);
  printResponse();
  }
void loop(void)
{ 
  sensors.requestTemperatures();
  Serial.print("temperature is ");
  float Temp = sensors.getTempCByIndex(0);
  Serial.println(Temp);
  int sensorValue = analogRead(A1);
  float turbidity = sensorValue * (5.0 / 1024.0);
  Serial.print("turbidity in voltage is ");
  Serial.println(turbidity);

  digitalWrite (trigger_pin, HIGH);
  delayMicroseconds (10);
  digitalWrite (trigger_pin, LOW);
  time = pulseIn (echo_pin, HIGH);
  water_level = (time * 0.034) / 2;
  Serial.print("water level in centimeters = ");
  Serial.println(water_level);

  lcd.setCursor(0, 0);
  lcd.clear(); 
  lcd.print("temperature");
  lcd.setCursor(1, 1);
  lcd.print(Temp);
  delay(2000); 

  lcd.setCursor(0, 0);
  lcd.clear(); 
  lcd.print("turbidity");
  lcd.setCursor(1, 1);
  lcd.print(turbidity); 
  delay(2000); 

  lcd.setCursor(0, 0);
  lcd.clear(); 
  lcd.print("PH");
  lcd.setCursor(1, 1);
  lcd.print(7); 
  delay(2000); 

  lcd.setCursor(0, 0);
  lcd.clear(); 
  lcd.print("Water level");
  lcd.setCursor(1, 1);
  lcd.print(water_level ); 
  delay(2000); 
    
  sendData();
  delay(5000);
 }
