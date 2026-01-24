#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <ESP32Encoder.h>
#include <EEPROM.h>

// ===== Pin settings =====
///////////////////////////////////////////
#define HUMIDITY_PIN_1	36
#define HUMIDITY_PIN_2	39   // input pin
#define HUMIDITY_PIN_3	34
int dryValue_1 = 3000;  // dry (calibrate!)
int wetValue_1 = 1200;  // wet (calibrate!)
int raw_1;
int moisture_1;
//////////////////
int dryValue_2 = 3000;  // dry (calibrate!)
int wetValue_2 = 1200;  // wet (calibrate!)
int raw_2;
int moisture_2;
/////////////////
int dryValue_3 = 3000;  // dry (calibrate!)
int wetValue_3 = 1200;  // wet (calibrate!)
int raw_3;
int moisture_3;
/////////////////////////////////////////////
unsigned long WateringMillis = 0;
bool WateringdeviceState = LOW;

unsigned long LED_previousMillis = 0;
bool LED_deviceState = LOW;

// Intervals in milliseconds
unsigned long WateringOff;//3600000; // 1 h = 60*60*1000 ms
unsigned long WateringOn;//10000;    // 10 s

unsigned long LED_ON_TIME;//  = HOURS_ON * 3600000UL;
unsigned long LED_OFF_TIME;// = (24 - HOURS_ON) * 3600000UL;
bool LED_ON_OFF_MILIS = true;

unsigned long Water_lcdMillis = 0;
const unsigned long Water_lcdInterval = 500UL; // 5 сек
bool Water_Visible = false;


/////////////////////////////////////////////
#define ONE_WIRE_BUS 19
#define WATER_LOW_PIN	33
int WATER_LCD;
///////////////////////////////

#define HEATER_PIN	25
#define LED_PIN		26
#define PUMP_1_PIN	27  // relay pin
#define PUMP_2_PIN	14
#define PUMP_3_PIN	13
#define VENTILATION_FAN_PIN		18
/////////////////////////////////

#define ENC_CLK 16
#define ENC_DT  4       // Encoder pin
#define ENC_SW  17
/////////////////////////////////
#define EEPROM_SIZE 30


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int tempC;
// ===== LCD =====
LiquidCrystal_PCF8574 lcd(0x27); // <-- display i2s

// ===== Encoder =====
ESP32Encoder encoder;

// ===== Button =====
bool lastBtnState = HIGH;
bool btnPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
unsigned long lastClickTime = 0;
const unsigned long doubleClickTime = 500; // calibration doubleClickTime
unsigned long encoderMillis = 0;       // 0
const unsigned long encoderInterval = 400; // left right Encoder calibration

unsigned long logo_lcd_Print_previousMillis = 0;
unsigned long logo_lcd_Print_currentMillis  = 0;
const unsigned long logo_lcd_Print_interval = 1000; //  lcd print timer > 1000 ms
bool logoVisible = false;
bool status_clear=true;

int state = 0;

// ===== Menu =====
const char* menuItems[] = {"Temp min", "Temp max", "Watering", "Watering","Humidity", "Light","Fan", "1:Pump", "2:Pump", "3:Pump","Pump:run", "reset"};
int menuValues[] = {16, 19, 12, 10, 75, 12, 0, 0, 0, 0, 0, 0};
int menuMin[]    = {0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0};
int menuMax[]    = {30, 30, 24, 120, 99, 24, 1, 1, 1, 1, 1, 1};
//const char* menuUnits[] = {"C","C","%","%","%","","","",""};
int menuCount = sizeof(menuItems)/sizeof(menuItems[0]);
int currentMenu = 0;
bool editMode = false;


//display
//LiquidCrystal_I2C lcd(0x27, 16, 4);
byte tempLogo[8] = {
  B00100,
  B01010,
  B01010,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110,
};  //icon for termometer
byte humLogo[8] = {
  B00100,
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110,
};  //icon for water droplet

byte wind1[] = {
  B00000,
  B00000,
  B00000,
  B00011,
  B00111,
  B00111,
  B00111,
  B00011,
};
byte wind2[] = {
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte wind3[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B11111,
  B11111,
};
byte wind4[] = {
  B11000,
  B11100,
  B11100,
  B11100,
  B11000,
  B00000,
  B00000,
  B00000,
};

byte wind5[] = {
  B00000,
  B00000,
  B00000,
  B11000,
  B11100,
  B11100,
  B11100,
  B11000,
};
byte wind6[] = {
  B11111,
  B11111,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte wind7[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B01110,
  B11111,
  B11111,
};
byte wind8[] = {
  B00011,
  B00111,
  B00111,
  B00111,
  B00011,
  B00000,
  B00000,
  B00000,
};

byte dhtIcon[] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B00100,
  B00000,
  B00100,
  B00100,
};
byte LightIcon[] = {
  B01110,
  B10001,
  B10001,
  B10001,
  B01010,
  B01110,
  B01110,
  B00100
};
byte name0x3[] = { B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 }; // loding stemar

void setup() {
  Serial.begin(115200);
  sensors.begin();

 
//  pinMode(,);
  pinMode(WATER_LOW_PIN,INPUT_PULLUP);

  pinMode(HEATER_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT);
  pinMode(PUMP_1_PIN,OUTPUT);
  pinMode(PUMP_2_PIN,OUTPUT);
  pinMode(PUMP_3_PIN,OUTPUT);
  pinMode(VENTILATION_FAN_PIN,OUTPUT);

  pinMode(ENC_SW, INPUT_PULLUP);
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);

                  
  

  OFF_HEATER();
  ON_LED();    
  OFF_PUMP_1();
  OFF_PUMP_2();
  OFF_PUMP_3();
  OFF_VENTILATION_FAN();

  LED_deviceState = true;
  LED_previousMillis = millis();
  

  // +1/-1
  encoder.attachFullQuad(ENC_CLK, ENC_DT);
  encoder.clearCount();

  lcd.begin(20,4);
  lcd.setBacklight(255);

  EEPROM.begin(EEPROM_SIZE);
  // eeprom save
  for(int i=0;i<menuCount;i++){
    int val = EEPROM.read(i);
    if(val >= menuMin[i] && val <= menuMax[i])
      menuValues[i] = val;
  }


lcdFun();

}

////////////////////////////////////////////////////////////////////////
void factory_reset(){
lcd.clear();
lcd.setCursor(6,0);
lcd.print("StemAr");
lcd.setCursor(2,2);
lcd.print("Factory reset");
lcd.setCursor(3,3);
lcd.print("Please wait");
for(int l=2;l<=15;l++){
lcd.createChar(7, name0x3);
  lcd.setCursor(l, 1);
  lcd.write(7);
  delay(800);
}
lcd.clear();
////////////
  for (int i = 0; i < EEPROM_SIZE; i++) {
    EEPROM.write(i, 0xFF);   // clear
  }

  EEPROM.commit();          //  ESP32
  EEPROM.end();
  delay(100);
  ESP.restart();
  state=0;
  Serial.println("EEPROM RESET DONE");
}
///////////////////////////////////////////////////////////////////////////////
void data_sensor(){
  sensors.requestTemperatures();               
  tempC = sensors.getTempCByIndex(0);    // temp data
  ////////////////////////////////////
  raw_1 = analogRead(HUMIDITY_PIN_1);
  moisture_1 = map(raw_1, dryValue_1, wetValue_1, 0, 100);
  moisture_1 = constrain(moisture_1, 0, 100);
  ////////////////////////////////////
  raw_2 = analogRead(HUMIDITY_PIN_2);
  moisture_2 = map(raw_2, dryValue_2, wetValue_2, 0, 100);
  moisture_2 = constrain(moisture_2, 0, 100);
  ////////////////////////////////////
  raw_3 = analogRead(HUMIDITY_PIN_3);
  moisture_3 = map(raw_3, dryValue_3, wetValue_3, 0, 100);
  moisture_3 = constrain(moisture_3, 0, 100);
  ////////////////////////////////////
 // if(!digitalRead(WATER_LOW_PIN)){
  WATER_LCD = digitalRead(WATER_LOW_PIN);
  

  //////////////////////////////////////


}

void ON_HEATER(){digitalWrite(HEATER_PIN,HIGH);}
void OFF_HEATER(){digitalWrite(HEATER_PIN,LOW);}

void ON_LED(){digitalWrite(LED_PIN,HIGH);}
void OFF_LED(){digitalWrite(LED_PIN,LOW);}

void ON_VENTILATION_FAN(){digitalWrite(VENTILATION_FAN_PIN,HIGH);}
void OFF_VENTILATION_FAN(){digitalWrite(VENTILATION_FAN_PIN,LOW);}

void ON_PUMP_1(){digitalWrite(PUMP_1_PIN,HIGH);}
void OFF_PUMP_1(){digitalWrite(PUMP_1_PIN,LOW);}

void ON_PUMP_2(){digitalWrite(PUMP_2_PIN,HIGH);}
void OFF_PUMP_2(){digitalWrite(PUMP_2_PIN,LOW);}

void ON_PUMP_3(){digitalWrite(PUMP_3_PIN,HIGH);}
void OFF_PUMP_3(){digitalWrite(PUMP_3_PIN,LOW);}



void ON_OFF_RELAY(){  
WateringOff = menuValues[2] * 3600000UL;//3600000; //  = 60*60*1000 ms
WateringOn = menuValues[3] * 1000UL;//1000;    // 1 s

LED_ON_TIME  = menuValues[5] * 3600000UL;
LED_OFF_TIME = (24 - menuValues[5]) * 3600000UL;

  if(tempC<=menuValues[0] && tempC != DEVICE_DISCONNECTED_C){ON_HEATER();}
  if(tempC>=menuValues[1] || tempC == DEVICE_DISCONNECTED_C){OFF_HEATER();}
  if(menuValues[6]){
    if(tempC > menuValues[1]){ON_VENTILATION_FAN();}
    if(tempC <= menuValues[1]){OFF_VENTILATION_FAN();}
  }else{OFF_VENTILATION_FAN();}

  if(WATER_LCD){
    OFF_PUMP_1();
    OFF_PUMP_2();
    OFF_PUMP_3();
    }else{Watering_ON_OFF();}

  if(state == 0){
  if (LED_ON_OFF_MILIS == true){ON_LED();}
  if (LED_ON_OFF_MILIS == false){OFF_LED();}
  }else{
    OFF_LED();
  }
}
void Watering_ON_OFF(){
unsigned long WateringcurrentMillis = millis();

  if (WateringdeviceState == LOW) {
    
    if (WateringcurrentMillis - WateringMillis >= WateringOff) {
      WateringdeviceState = HIGH;          
      if(moisture_1 < menuValues[4] && menuValues[7] == true && WATER_LCD == false){ON_PUMP_1();}
      if(moisture_2 < menuValues[4] && menuValues[8] == true && WATER_LCD == false){ON_PUMP_2();}
      if(moisture_3 < menuValues[4] && menuValues[9] == true && WATER_LCD == false){ON_PUMP_3();}    // pump on off
      WateringMillis = WateringcurrentMillis;
    }
  } else {
    
    if (WateringcurrentMillis - WateringMillis >= WateringOn) {
      WateringdeviceState = LOW;        
      OFF_PUMP_1();
      OFF_PUMP_2();
      OFF_PUMP_3();
      WateringMillis = WateringcurrentMillis;
    }
  }
}







void no_water(){
  if (WATER_LCD) {
  unsigned long Water_currentMillis = millis();

  if (Water_currentMillis - Water_lcdMillis >= Water_lcdInterval) {
    Water_lcdMillis = Water_currentMillis;
    Water_Visible = !Water_Visible;

    if (Water_Visible) {
      lcd.setCursor(15,2);
    lcd.print("Water"); 
    lcd.setCursor(17,3); 
    lcd.write(char(2));
    } else {
      lcd.setCursor(15,2); 
      lcd.print("     ");
      lcd.setCursor(17,3); 
      lcd.print(" "); 
    }
  }
  }else{
      lcd.setCursor(15,2); 
      lcd.print("     ");
      lcd.setCursor(17,3); 
      lcd.print(" ");
  }
}

void LED_ON_OFF() {
  unsigned long LED_currentMillis = millis();

  if (LED_deviceState) {
    if (LED_currentMillis - LED_previousMillis >= LED_ON_TIME) {
      LED_deviceState = false;
      LED_ON_OFF_MILIS = false;
      
      LED_previousMillis = LED_currentMillis;
    }
  } else {
    if (LED_currentMillis - LED_previousMillis >= LED_OFF_TIME) {
      LED_deviceState = true;
      LED_ON_OFF_MILIS = true;
      
      LED_previousMillis = LED_currentMillis;
    }
  }
}
void LED_LCD_PRINT_ON_OFF(){
  
}

void FAN_LCD_PRINT_ON_OFF(){
  if(menuValues[6]){
    if(tempC > menuValues[1]){lcd.setCursor(17, 1); lcd.print("   "); lcd.setCursor(17, 1); lcd.print("ON");}
    if(tempC <= menuValues[1]){lcd.setCursor(17, 1); lcd.print("   "); lcd.setCursor(17, 1); lcd.print("OFF");}
  }else{lcd.setCursor(17, 1); lcd.print("   "); lcd.setCursor(17, 1); lcd.print("OFF");}
}


void lcdFun() {
 // lcd.init();
  lcd.clear();
 // lcd.backlight();
  lcd.createChar(1, tempLogo);
  lcd.createChar(2, humLogo);

  lcd.createChar(3, wind1);
  lcd.createChar(4, wind2);
  lcd.createChar(5, wind3);
  lcd.createChar(6, wind4);

  lcd.createChar(7, dhtIcon);
  lcd.createChar(8, LightIcon);
}
bool pttvel = false;
void coolerLogo(int x, int y) {
  if (pttvel) {
    lcd.createChar(3, wind5);
    lcd.createChar(4, wind6);
    lcd.createChar(5, wind7);
    lcd.createChar(6, wind8);
    lcd.setCursor(x, y);
    lcd.write(char(5));
    lcd.setCursor(x, y + 1);
    lcd.write(char(6));
    lcd.setCursor(x + 1, y);
    lcd.write(char(3));
    lcd.setCursor(x + 1, y + 1);
    lcd.write(char(4));

    pttvel = false;
  } else {
    lcd.createChar(3, wind1);
    lcd.createChar(4, wind2);
    lcd.createChar(5, wind3);
    lcd.createChar(6, wind4);
    lcd.setCursor(x, y);
    lcd.write(char(3));
    lcd.setCursor(x, y + 1);
    lcd.write(char(4));
    lcd.setCursor(x + 1, y);
    lcd.write(char(5));
    lcd.setCursor(x + 1, y + 1);
    lcd.write(char(6));
    pttvel = true;
  }
}
void logo_lcd_Print(){
  

logo_lcd_Print_currentMillis = millis();

if (logo_lcd_Print_currentMillis - logo_lcd_Print_previousMillis >= logo_lcd_Print_interval) {
    logo_lcd_Print_previousMillis = logo_lcd_Print_currentMillis;

    logoVisible = !logoVisible;

    lcd.setCursor(3, 0);

  if (logoVisible) {

      data_sensor();
      lcd.setCursor(0, 0);
      lcd.write(char(1));
      lcd.setCursor(1, 0);
      lcd.print(":Temp");
 // lcd.setCursor(0, 1);
 // lcd.print("1:");
    if (tempC == DEVICE_DISCONNECTED_C) {
      lcd.setCursor(2, 1);
      lcd.print("nan");
    } else {
      lcd.setCursor(2, 1);
      lcd.print(tempC);
      lcd.write((char)223);
      lcd.print("C");
    }

  
  lcd.setCursor(7, 0);
  lcd.write(char(2));
  lcd.setCursor(8, 0);
  lcd.print(":Hum");
  lcd.setCursor(7, 1);
  lcd.print("1:");
  lcd.setCursor(11, 1);
  lcd.print("  ");
  lcd.setCursor(9, 1);
  lcd.print(String(moisture_1) + "%");
  lcd.setCursor(7, 2);
  lcd.print("2:");
  lcd.setCursor(11, 2);
  lcd.print("  ");
  lcd.setCursor(9, 2);
  lcd.print(String(moisture_2) + "%");
  lcd.setCursor(7, 3);
  lcd.print("3:");
  lcd.setCursor(11, 3);
  lcd.print("  ");
  lcd.setCursor(9, 3);
  lcd.print(String(moisture_3) + "%");

  coolerLogo(14, 0);
  lcd.setCursor(16, 0);
  lcd.print(":Fan");
  lcd.setCursor(16, 1);
  lcd.print(":");
  FAN_LCD_PRINT_ON_OFF();
  


  lcd.setCursor(0, 2);
  lcd.print(char(8));
  lcd.setCursor(1, 2);
  lcd.print(":Lig");
  if (LED_ON_OFF_MILIS == true){lcd.setCursor(2, 3); lcd.print("   "); lcd.setCursor(2, 3); lcd.print("ON");}
  if (LED_ON_OFF_MILIS == false){lcd.setCursor(2, 3); lcd.print("   "); lcd.setCursor(2, 3); lcd.print("OFF");}
  //lcd.setCursor(0, 3);
  //lcd.print("1:");
  //lcd.setCursor(2, 3);
  //lcd.print("OFF");

  

//  lcd.setCursor(0,1);
//  lcd.print(char(7));
  
     
    } else {
      // clear display lcd.print("   ");
    }
  }

}



// fancshon print ON/OFF
bool isOnOffItem(int idx) {
  return (idx >= 6 && idx <= 11);
}

void drawMenu() {
  int startIdx = 0;
  if(currentMenu >= 4) startIdx = currentMenu - 3;

  for(int i=0; i<4; i++){
    int idx = startIdx + i;
    if(idx >= menuCount) break;

    
    lcd.setCursor(0, i);
    lcd.print(idx == currentMenu ? ">" : "  ");

    
    lcd.print(menuItems[idx]);

    
    lcd.setCursor(12, i);

if (isOnOffItem(idx)) {
  lcd.print(menuValues[idx] ? "ON " : "OFF");
} else {
  lcd.print(menuValues[idx]);

  
  if (idx == 0 || idx == 1) {
    lcd.write((char)223); // °
    lcd.print("C");
  }
  
  else if (idx == 2) {
    lcd.print("h");
  }
  else if (idx == 3) {
    lcd.print("s");
  }
  else if (idx == 4) {
    lcd.print("%");
  }
  else if (idx == 5) {
    lcd.print("h");
  }
  

  lcd.print("   "); 
}

  }
}






void print_data(){
  status_clear=true;

  bool reading = digitalRead(ENC_SW);
  if (reading == LOW && lastBtnState){
      editMode = !editMode;             // erorr ???????
      Serial.print("Mode: ");
      state++;
      if (state >=2){state=1;}
      
      Serial.println(editMode ? "EDIT" : "SELECT");
  }
  lastBtnState = reading;

  //lcd.clear();
/*  lcd.setCursor(5,0);
  lcd.print(menuValues[0]);
  lcd.setCursor(5,1);
  lcd.print(menuValues[1]);
  lcd.setCursor(5,2);
  lcd.print(menuValues[2]);
  lcd.setCursor(5,3);
  lcd.print(menuValues[3]);*/
}






void Set_menu(){
  
  if (state==1 && status_clear == true){status_clear=false; lcd.clear(); }
  bool reading = digitalRead(ENC_SW);

  // антидребезг
  if (reading == LOW && lastBtnState == HIGH && millis() - lastDebounceTime > debounceDelay) {
      unsigned long now = millis();
      if(now - lastClickTime <= doubleClickTime){
          // Double click 
          state=0;
          lcd.clear();
          lastClickTime = 0; // reset
      } else {
          
          lastClickTime = now;
      }
      lastDebounceTime = millis();
  }

  lastBtnState = reading;

  unsigned long currentMillis = millis();

  if (currentMillis - encoderMillis >= encoderInterval) {
    encoderMillis = currentMillis;  

    



  // ===== buton to clik =====
  reading = digitalRead(ENC_SW);

  if (reading != lastBtnState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading == LOW && !btnPressed) {
      editMode = !editMode; // mode switching
      btnPressed = true;
      Serial.print("Mode: "); Serial.println(editMode ? "EDIT" : "SELECT");
    } else if (reading == HIGH) {
      btnPressed = false; // button released
    }
  }
  lastBtnState = reading;

  // ===== encoder =====
  long pos = encoder.getCount();
  encoder.clearCount();

  if(pos != 0){
    if(editMode){
      
      menuValues[currentMenu] += (pos>0)?1:-1;
      if(menuValues[currentMenu] < menuMin[currentMenu]) menuValues[currentMenu] = menuMin[currentMenu];
      if(menuValues[currentMenu] > menuMax[currentMenu]) menuValues[currentMenu] = menuMax[currentMenu];
      if(menuValues[0] >= menuValues[1] && currentMenu == 0){menuValues[0]--;}
      if(menuValues[1] <= menuValues[0] && currentMenu == 1){menuValues[1]++;}
      if( currentMenu == 11){state = 2;} // factory_reset
      EEPROM.write(currentMenu, menuValues[currentMenu]);
      EEPROM.commit();

      Serial.print(menuItems[currentMenu]);
      Serial.print(" = ");
      Serial.println(menuValues[currentMenu]);
      lcd.setCursor(5,0);
        lcd.print("     ");
    } else {
        lcd.setCursor(4,0);
        lcd.print("          ");
        lcd.setCursor(4,1);
        lcd.print("          ");
        lcd.setCursor(4,2);
        lcd.print("          ");
        lcd.setCursor(4,3);
        lcd.print("          ");
        
      Serial.println(pos);
      currentMenu += (pos>0)?1:-1;
      if(currentMenu < 0) currentMenu = 0;
      if(currentMenu >= menuCount) currentMenu = menuCount-1;//Serial.print(currentMenu);
    }
  }


  //lcd.clear();
  



drawMenu();








/*
  int startIdx = 0;
  if(currentMenu >= 4) startIdx = currentMenu - 3;

  for(int i=0;i<4;i++){
    int idx = startIdx + i;
    if(idx >= menuCount) break;

    lcd.setCursor(0,i);
    lcd.print(idx == currentMenu ? ">" : "  ");
    lcd.print(menuItems[idx]);
    lcd.setCursor(12,i);
    lcd.print(menuValues[idx]); 
 
  }*/

    }

}

void Pump_run(){
  WATER_LCD = digitalRead(WATER_LOW_PIN);
  if (menuValues[10] && state == 1 && WATER_LCD == false){
    if(menuValues[7] == true){ON_PUMP_1();}else{OFF_PUMP_1();}
    if(menuValues[8] == true){ON_PUMP_2();}else{OFF_PUMP_2();}
    if(menuValues[9] == true){ON_PUMP_3();}else{OFF_PUMP_3();}
  }else{
    menuValues[10] = false;
    OFF_PUMP_1();
    OFF_PUMP_2();
    OFF_PUMP_3();
  }
}

void loop() {
  
  

/*



  bool reading = digitalRead(ENC_SW);
  if (reading == LOW && lastBtnState){
      editMode = !editMode;
      Serial.print("Mode: ");
      state++;
      if (state >=2){state=1;}
      
      Serial.println(editMode ? "EDIT" : "SELECT");
  }
  lastBtnState = reading;

  long pos = encoder.getCount();
  encoder.clearCount();

  if(pos != 0){
      
      if(pos > 0) pos = 1;
      if(pos < 0) pos = -1;

      if(editMode){
          Serial.print("Editing, step: ");
          Serial.println(pos);
      } else {
          Serial.print("Menu move, step: ");
          Serial.println(pos);
      }
  }

  delay(50);
*/


switch (state) {
        case 0:          
            print_data();
            delay(200);
            logo_lcd_Print(); 
            no_water();
            ON_OFF_RELAY();
            LED_ON_OFF();
            break;
        case 1:
            Set_menu();          
            OFF_HEATER();
            OFF_LED();
            Pump_run();
            OFF_VENTILATION_FAN();
            break;
        case 2:         
            OFF_HEATER();
            OFF_LED();
            OFF_PUMP_1();
            OFF_PUMP_2();
            OFF_PUMP_3();
            OFF_VENTILATION_FAN();
            factory_reset();
            break;
        case 3:

            break;
        default:
            
            break;
    }

}
