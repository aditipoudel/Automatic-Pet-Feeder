#include "Arduino.h"
#include "ESP8266.h"#include "Keypad.h"
#include "LiquidCrystal.h"
#include "Wire.h"
#include "RTClib.h"
#include "Servo.h"

#define WIFI_PIN_TX  11
#define WIFI_PIN_RX 10
#define KEYPADMEM3X4_PIN_ROW1 5
#define KEYPADMEM3X4_PIN_ROW2 6
#define KEYPADMEM3X4_PIN_ROW3 7
#define KEYPADMEM3X4_PIN_ROW4 8
#define KEYPADMEM3X4_PIN_COL1 2
#define KEYPADMEM3X4_PIN_COL2 3
#define KEYPADMEM3X4_PIN_COL3 4
#define LCD_PIN_RS  A0
#define LCD_PIN_E A1  
#define LCD_PIN_DB4 9
#define LCD_PIN_DB5 12
#define LCD_PIN_DB6 13
#define LCD_PIN_DB7 A3
#define SERVOSM_PIN_SIG A2

//For blynk 
#define BLYNK_TEMPLATE_ID "TMPLuXMuM-AW"
#define BLYNK_DEVICE_NAME "PetFeeder"  //connecting to my project from blynk app


//wifi setting 
const char *SSID     = "Nirvana#Stay Home@ClassicTech"; 
const char *PASSWORD = "N0wifiherePle@$e" ; 

char* const host = "www.google.com";
int hostPort = 80;


char keypadmem3x4keys[ROWS][COLS] = {
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'}
};
const int servoSMRestPosition   = 20;  //Starting position
const int servoSMTargetPosition = 150; //Position when event is detected
// object initialization
ESP8266 wifi(WIFI_PIN_RX,WIFI_PIN_TX);
Keypad keypadmem3x4(KEYPADMEM3X4_PIN_COL1,KEYPADMEM3X4_PIN_COL2,KEYPADMEM3X4_PIN_COL3,KEYPADMEM3X4_PIN_ROW1,KEYPADMEM3X4_PIN_ROW2,KEYPADMEM3X4_PIN_ROW3,KEYPADMEM3X4_PIN_ROW4);
LiquidCrystal lcd(LCD_PIN_RS,LCD_PIN_E,LCD_PIN_DB4,LCD_PIN_DB5,LCD_PIN_DB6,LCD_PIN_DB7);
RTC_DS3231 rtcDS;
Servo servoSM;


const int timeout = 10000;      
char menuOption = 0;
long time0;


void setup() 
{
    Serial.begin(9600);
     Blynk.begin(auth, ssid, pass);
    Blynk.syncAll(); 
    
    wifi.init(SSID, PASSWORD);
    keypadmem3x4.begin(keypadmem3x4keys);
  
    lcd.begin(16, 2);
    if (! rtcDS.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
    }
    if (rtcDS.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
   //date & time at compilation
    rtcDS.adjust(DateTime(2021, 7, 31, 8, 45, 0)));
   
    }
    servoSM.attach(SERVOSM_PIN_SIG);
    servoSM.write(servoSMRestPosition);
    delay(100);
    servoSM.detach();
    menuOption = menu();
    
}

void loop() 
{    //for blynk app 
  Blynk.run(); //
  
     if(menuOption == '1') {
      wifi.httpGet(host, hostPort);
      char* wifiBuf = wifi.getBuffer();
    char *wifiDateIdx = strstr (wifiBuf, "Date");
    for (int i = 0; wifiDateIdx[i] != '\n' ; i++)
    Serial.print(wifiDateIdx[i]);
  }
    else if(menuOption == '2') {
 
    char keypadmem3x4Key = keypadmem3x4.getKey();
    if (isDigit(keypadmem3x4Key) ||  keypadmem3x4Key == '*' ||  keypadmem3x4Key == '#')
    {
    Serial.print(keypadmem3x4Key);
    }
    }
    else if(menuOption == '3') {
   
    lcd.setCursor(0, 0);
    lcd.print("working !");
    lcd.noDisplay();
    delay(500);
    lcd.display();
    delay(500);
    }
    else if(menuOption == '4') {
  
    DateTime now = rtcDS.now();
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print('/');
    Serial.print(now.year(), DEC);
    Serial.print("  ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    delay(1000);
    }
    else if(menuOption == '5') {
   servoSM.attach(SERVOSM_PIN_SIG);        
    servoSM.write(servoSMTargetPosition);  
    delay(500);                              
    servoSM.write(servoSMRestPosition);    
    delay(500);                              
    servoSM.detach();                    
    }
    if (millis() - time0 > timeout)
    {   menuOption = menu();
    }
}

char menu()
{ Serial.println(F("\Testing components"));
    Serial.println(F("(1) ESP8266-01 - Wifi Module"));
    Serial.println(F("(2) Membrane 3x4 Matrix Keypad"));
    Serial.println(F("(3) LCD 16x2"));
    Serial.println(F("(4) RTC - Real Time Clock"));
    Serial.println(F("(5) Servo - Generic Metal Gear (Micro Size)"));
    Serial.println(F("(menu) send anything else or press on board reset button\n"));
    while (!Serial.available());
    while (Serial.available()) 
    {     char c = Serial.read();
        if (isAlphaNumeric(c)) 
        {               if(c == '1') 
          Serial.println(F("Now Testing ESP8266-01 - Wifi Module"));
        else if(c == '2') 
          Serial.println(F("Now Testing Membrane 3x4 Matrix Keypad"));
        else if(c == '3') 
          Serial.println(F("Now Testing LCD 16x2"));
        else if(c == '4') 
          Serial.println(F("Now Testing RTC - Real Time Clock"));
        else if(c == '5') 
          Serial.println(F("Now Testing Servo - Generic Metal Gear (Micro Size)"));
            else
            {  Serial.println(F("illegal input!"));
                return 0;     }
            time0 = millis();
            return c;
        } }}
