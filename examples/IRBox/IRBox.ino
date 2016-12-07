/**************************************************************
 * This is an example for GadgetBox to create a Learning IR Remote control using Blynk
 * 
 * There are up to 10 IR commands that can be learned. Raw IR commands not supported.
 * 
 * eCogs Used:
 *       eCog107 - Remote Control - Connect to eCog B
 *       eCog100 - Wifi - Connect to eCog C
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       IRremote
 *       Blynk
 *       IRremoteESP8266 if on NodeMCU. Install manually from https://github.com/markszabo/IRremoteESP8266
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro (Only 4 IR commands)
 *       Teensy (Be sure to install the Teensy version of IRremote)
 *       
 * Blynk App:
 *       Clone the following project in Blynk http://tinyurl.com/hjrc5pu
 *       
 * Blynk Virtual Pins:      
 *       V0:      The table to select which IR command to learn
 *       V1-V10:  Trigger a send of the IR command - connect a button or timer to send out IR command 
 *       V21-V30: Stores the IR command that is learned in the Blynk cloud
 *       V31:     LCD widget for feedback while learning
 ***************************************************************       
 * Blynk is a platform with iOS and Android apps to control
 * Arduino, Raspberry Pi and the likes over the Internet.
 * You can easily build graphic interfaces for all your
 * projects by simply dragging and dropping widgets.
 *
 *   Downloads, docs, tutorials: http://www.blynk.cc
 *   Blynk community:            http://community.blynk.cc
 *   Social networks:            http://www.fb.com/blynkapp
 *                               http://twitter.com/blynk_app
 *
 * Blynk library is licensed under MIT license
 * This example code is in public domain.
 *
 **************************************************************/
#include <GadgetBox.h>          //Install GadgetBox Library from Library Manager

//#define SKETCHDEBUG             //Uncomment to print out debug info, uses more space... Only provides one IR command

#if defined(SKETCHDEBUG)  
  #define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#endif



// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "yourauth";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "yourssid";
char pass[] = "yourpass";

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  #define EspSerial Serial2
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  #include <SoftwareSerial.h>
  SoftwareSerial EspSerial(CC0, CC1); // RX, TX (For Pro Mini)
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <BlynkSimpleEsp8266.h>
  #include <IRremoteESP8266.h>    //Install IRremoteESP8266 library from https://github.com/markszabo/IRremoteESP8266
#endif

#if defined(ESP8266)
  IRsend irsend(BC3);
  int RECV_PIN = BC2;             //Pin for IR Receiver - on eCog B by default
#else
  #include <ESP8266_Lib.h>
  #include <BlynkSimpleShieldEsp8266.h>
  #include <IRremote.h>           //Install IRremote library from Library Manager  
  // Your ESP8266 baud rate:
  #define ESP8266_BAUD 9600 
  ESP8266 wifi(&EspSerial); 
  IRsend irsend;
  int RECV_PIN = BC2;             //Pin for IR Receiver - on eCog B by default 
#endif

IRrecv irrecv(RECV_PIN);

decode_results results;

WidgetLCD lcd(V31);

void setup()
{
  irrecv.enableIRIn(); // Start the receiver
#if defined(ESP8266)
  Blynk.begin(auth, ssid, pass);
  irsend.begin();
#else
  // Set console baud rate
  Serial.begin(9600);
  delay(10);
  EspSerial.begin(115200);
  EspSerial.println("AT+CIOBAUD=9600");
  delay(1000);
  Blynk.begin(auth, wifi, ssid, pass);
  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);  
#endif
}

void loop()
{
  Blynk.run();
}

// Storage for the recorded code
int codeType = -1; // The type of code
unsigned long codeValue; // The code value if not raw
//unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int toggle = 0; // The RC5/6 toggle state

// Stores the code for later playback
// Most of this code is just logging
void storeCode(decode_results *results) {
  codeType = results->decode_type;
  int count = results->rawlen;
//  if (codeType == UNKNOWN) {
//  }
//  else {
//    lcd.clear();
//    if (codeType == NEC) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received NEC: ");
//      #endif
//      if (results->value == REPEAT) {
//        return;
//      }
//    } 
//    else if (codeType == SONY) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received SONY: ");
//      #endif      
//    } 
//    else if (codeType == PANASONIC) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received PANASONIC: ");
//      #endif  
//    }
//    else if (codeType == JVC) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received JVC: ");
//      #endif        
//    }
//    else if (codeType == RC5) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received RC5: ");
//      #endif        
//    } 
//    else if (codeType == RC6) {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Received RC6: ");
//      #endif        
//    } 
//    else {
//      #if defined(SKETCHDEBUG)
//        Serial.print("Unexpected codeType ");
//        Serial.print(codeType, DEC);
//        Serial.println("");
//      #endif        
//    }
    lcd.print(5,1,results->value);
    lcd.print(14,1,results->bits);
    codeValue = results->value;
    codeLen = results->bits;
//  }
}

void sendCode(int repeat) {
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
  } 
  else if (codeType == PANASONIC) {
    irsend.sendPanasonic(codeValue, codeLen);
  }
  else if (codeType == JVC) {
    irsend.sendPanasonic(codeValue, codeLen);
  }
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    //irsend.sendRaw(rawCodes, codeLen, 38);    Raw is disabled to save space...
  }
}

void learnIR(int rowId) {
  lcd.clear();
  lcd.print(0,0,"Waiting for IR");
  int found = 0;
  irrecv.enableIRIn();
  for(int i = 0;i<50;i++){
    if(irrecv.decode(&results)){
      storeCode(&results);
      found = 1;
      switch (rowId)
      {
        case 1:
          Blynk.virtualWrite(V0, "pick", 1);
          Blynk.virtualWrite(V21,codeType,codeValue,codeLen);
          break;
#if !defined(SKETCHDEBUG)            
        case 2:
          Blynk.virtualWrite(V0, "pick", 2);
          Blynk.virtualWrite(V22,codeType,codeValue,codeLen);
          break;
        case 3:
          Blynk.virtualWrite(V0, "pick", 3);
          Blynk.virtualWrite(V23,codeType,codeValue,codeLen);
          break;
        case 4:
          Blynk.virtualWrite(V0, "pick", 4);
          Blynk.virtualWrite(V24,codeType,codeValue,codeLen);
          break;  
#if !defined(ARDUINO_AVR_PRO)            
        case 5:
          Blynk.virtualWrite(V0, "pick", 5);
          Blynk.virtualWrite(V25,codeType,codeValue,codeLen);
          break;          
        case 6:
          Blynk.virtualWrite(V0, "pick", 6);
          Blynk.virtualWrite(V26,codeType,codeValue,codeLen);
          break;    
        case 7:
          Blynk.virtualWrite(V0, "pick", 7);
          Blynk.virtualWrite(V27,codeType,codeValue,codeLen);
          break;  
        case 8:
          Blynk.virtualWrite(V0, "pick", 8);
          Blynk.virtualWrite(V28,codeType,codeValue,codeLen);
          break;  
        case 9:
          Blynk.virtualWrite(V0, "pick", 9);
          Blynk.virtualWrite(V29,codeType,codeValue,codeLen);
          break;  
        case 10:
          Blynk.virtualWrite(V0, "pick", 10);
          Blynk.virtualWrite(V30,codeType,codeValue,codeLen);
          break;                                                                       
#endif
#endif           
        default:
          break;
      }
      lcd.print(0,0,"IR Found");
      break; 
    }
    delay(100);
  }
  if (!found) {
    lcd.clear(); 
    lcd.print(0,0,"IR Timeout");
  }
}

BLYNK_WRITE(V0)
{
  //Learn Mode
   String cmd = param[0].asStr();
   if (cmd == "select") {
       //row in table was selected.        
        learnIR(param[1].asInt());
   }
   if (cmd == "deselect") {
       //row in table was deselected. 
       learnIR(param[1].asInt());
   }
}

BLYNK_WRITE(V1)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V21); 
  }
}
#if !defined(SKETCHDEBUG)  
BLYNK_WRITE(V2)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V22); 
  }
}

BLYNK_WRITE(V3)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V23); 
  }
}

BLYNK_WRITE(V4)
{
  if(param[0].asInt()){
    Blynk.syncVirtual(V24); 
  }
}
#if !defined(ARDUINO_AVR_PRO) 
  BLYNK_WRITE(V5)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V25); 
    }
  }

  BLYNK_WRITE(V6)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V26); 
    }
  }
  
  BLYNK_WRITE(V7)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V27); 
    }
  }
  
  BLYNK_WRITE(V8)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V28); 
    }
  }

  BLYNK_WRITE(V9)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V29); 
    }
  }
  
  BLYNK_WRITE(V10)
  {
    if(param[0].asInt()){
      Blynk.syncVirtual(V30); 
    }
  }
#endif
#endif 

BLYNK_WRITE(V21)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);    
}
#if !defined(SKETCHDEBUG)  
BLYNK_WRITE(V22)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
}

BLYNK_WRITE(V23)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
}

BLYNK_WRITE(V24)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    sendCode(0);
}
#if !defined(ARDUINO_AVR_PRO) 
  BLYNK_WRITE(V25)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }

  BLYNK_WRITE(V26)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }
  
  BLYNK_WRITE(V27)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }
  
  BLYNK_WRITE(V28)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }

  BLYNK_WRITE(V29)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }

  BLYNK_WRITE(V30)
  {
      codeType = param[0].asInt();
      codeValue = param[1].asInt();
      codeLen = param[2].asInt();
      sendCode(0);
  }
#endif
#endif 

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //Setup the IR command table
  Blynk.virtualWrite(V0, "clr");
  Blynk.virtualWrite(V0, "add", 0, "Button", "Virtual Pin");
  Blynk.virtualWrite(V0, "add", 1, "IR1", "V1");
  #if !defined(SKETCHDEBUG)  
  Blynk.virtualWrite(V0, "add", 2, "IR2", "V2");
  Blynk.virtualWrite(V0, "add", 3, "IR3", "V3");
  Blynk.virtualWrite(V0, "add", 4, "IR4", "V4");
  #if !defined(ARDUINO_AVR_PRO) 
    Blynk.virtualWrite(V0, "add", 5, "IR5", "V5"); 
    Blynk.virtualWrite(V0, "add", 6, "IR6", "V6");   
    Blynk.virtualWrite(V0, "add", 7, "IR7", "V7"); 
    Blynk.virtualWrite(V0, "add", 8, "IR8", "V8"); 
    Blynk.virtualWrite(V0, "add", 9, "IR9", "V9"); 
    Blynk.virtualWrite(V0, "add", 10, "IR10", "V10"); 
  #endif
  #endif

#if defined(CORE_TEENSY)        //Use Hardware Serial for Teensy
  lcd.clear();
  lcd.print(0,0,"Teensy");
  lcd.print(0,1,"Connected");
#elif defined(ARDUINO_AVR_PRO)  // or Software Serial on eCog C for Mini Pro
  lcd.clear();
  lcd.print(0,0,"Mini Pro");
#elif defined(ESP8266)
  lcd.clear();
  lcd.print(0,0,"NodeMCU");
  lcd.print(0,1,"Connected");
#endif

  delay(4000);
  lcd.clear();
  lcd.print(0,0,"Tap a line");
  lcd.print(0,1,"to Learn IR");
}

