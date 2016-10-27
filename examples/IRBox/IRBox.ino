/**************************************************************
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
 **************************************************************
 *
 * This example shows how to use ESP8266 Shield (with AT commands)
 * to connect your project to Blynk.
 *
 * Note: Ensure a stable serial connection to ESP8266!
 *       Firmware version 1.0.0 (AT v0.22) is needed.
 *       You can change ESP baud rate. Connect to AT console and call:
 *           AT+UART_DEF=9600,8,1,0,0
 *       In general, Soft Serial may be unstable.
 *       It is highly recommended to switch to Hard Serial.
 *
 * Change WiFi ssid, pass, and Blynk auth token to run :)
 * Feel free to apply it to any other example. It's simple!
 *
 **************************************************************/
#include <GadgetBox.h>

//#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

#include <IRremote.h>

int RECV_PIN = BC1;
int BUTTON_PIN = DC0;
//int STATUS_PIN = DC3;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "yourssid";
char pass[] = "yourpass";

#include <SoftwareSerial.h>


// Hardware Serial on Teensy
//#define EspSerial Serial  //For Teensy

// or Software Serial on Mini Pro
//SoftwareSerial EspSerial(GB13, CC1); // RX, TX (For Pro Mini)
SoftwareSerial EspSerial(GB13, CC1); // RX, TX   (For GB 328pb)

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;

ESP8266 wifi(&EspSerial);

WidgetLCD lcd(V31);

void setup()
{
  // Set console baud rate
  Serial.begin(9600);
  delay(10);

  irrecv.enableIRIn(); // Start the receiver

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  //pinMode(STATUS_PIN, OUTPUT);
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
  if (codeType == UNKNOWN) {
    //Serial.println("Received unknown code, saving as raw");
    //codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
//    for (int i = 1; i <= codeLen; i++) {
//      if (i % 2) {
//        // Mark
//        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
//        //Serial.print(" m");
//      } 
//      else {
//        // Space
//        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
//        //Serial.print(" s");
//      }
//      //Serial.print(rawCodes[i - 1], DEC);
//    }
    //Serial.println("");
  }
  else {
    lcd.clear();
    if (codeType == NEC) {
      //Serial.print("Received NEC: ");
      //lcd.print(0,1,"NEC ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        //Serial.println("repeat; ignoring.");
        return;
      }
    } 
    else if (codeType == SONY) {
      //Serial.print("Received SONY: ");
      //lcd.print(0,1,"SONY ");
    } 
    else if (codeType == PANASONIC) {
      //Serial.print("Received PANASONIC: ");
      //lcd.print(0,1,"PANAS ");
    }
    else if (codeType == JVC) {
      //Serial.print("Received JVC: ");
      //lcd.print(0,1,"JVC ");
    }
    else if (codeType == RC5) {
      //Serial.print("Received RC5: ");
      //lcd.print(0,1,"RC5 ");
    } 
    else if (codeType == RC6) {
      //Serial.print("Received RC6: ");
      //lcd.print(0,1,"RC6 ");
    } 
    else {
//      Serial.print("Unexpected codeType ");
//      Serial.print(codeType, DEC);
//      Serial.println("");
    }
    //Serial.print(results->value, HEX);
    lcd.print(5,1,results->value);
    lcd.print(14,1,results->bits);
    //Serial.print(" Code Length ");
    //Serial.println(results->bits, DEC);
    codeValue = results->value;
    codeLen = results->bits;
  }
}

void sendCode(int repeat) {
  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
      //Serial.println("Sent NEC repeat");
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
      //Serial.print("Sent NEC ");
      //Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
    //Serial.print("Sent Sony ");
    //Serial.println(codeValue, HEX);
  } 
  else if (codeType == PANASONIC) {
    irsend.sendPanasonic(codeValue, codeLen);
    //Serial.print("Sent Panasonic");
    //Serial.println(codeValue, HEX);
  }
  else if (codeType == JVC) {
    irsend.sendPanasonic(codeValue, codeLen);
    //Serial.print("Sent JVC");
    //Serial.println(codeValue, HEX);
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
      //Serial.print("Sent RC5 ");
      //Serial.println(codeValue, HEX);
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
      //Serial.print("Sent RC6 ");
      //Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    //irsend.sendRaw(rawCodes, codeLen, 38);
    //Serial.println("Sent raw");
  }
}

void learnIR(int rowId) {
  //Serial.println("Waiting for IR Command");
  lcd.clear();
  lcd.print(0,0,"Waiting for IR");
  int found = 0;
  irrecv.enableIRIn();
  for(int i = 0;i<50;i++){
    if(irrecv.decode(&results)){
      //digitalWrite(STATUS_PIN, HIGH);
      storeCode(&results);
      found = 1;
      //digitalWrite(STATUS_PIN, LOW);
      switch (rowId)
      {
        case 1:
          Blynk.virtualWrite(V0, "pick", 1);
          Blynk.virtualWrite(V21,codeType,codeValue,codeLen);
          break;
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
        default:
          //Serial.println("Unknown item selected");
          break;
      }
      //Serial.println("IR Command Found");
      lcd.print(0,0,"IR Found");
      break; 
    }
    delay(100);
  }
  if (!found) {
    //Serial.println("IR Command Timeout");  
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


BLYNK_WRITE(V21)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(0);
    //digitalWrite(STATUS_PIN, LOW);
    //delay(50); // Wait a bit between retransmissions      
}

BLYNK_WRITE(V22)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(0);
    //digitalWrite(STATUS_PIN, LOW);
    //delay(50); // Wait a bit between retransmissions  
}

BLYNK_WRITE(V23)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(0);
    //digitalWrite(STATUS_PIN, LOW);
    //delay(50); // Wait a bit between retransmissions  
}

BLYNK_WRITE(V24)
{
    codeType = param[0].asInt();
    codeValue = param[1].asInt();
    codeLen = param[2].asInt();
    //digitalWrite(STATUS_PIN, HIGH);
    sendCode(0);
    //digitalWrite(STATUS_PIN, LOW);
    //delay(50); // Wait a bit between retransmissions  
}

// This function will run every time Blynk connection is established
BLYNK_CONNECTED() {
  //Setup the IR command table
  Blynk.virtualWrite(V0, "clr");
  Blynk.virtualWrite(V0, "add", 0, "Button", "Virtual Pin");
  Blynk.virtualWrite(V0, "add", 1, "IR1", "V1");
  Blynk.virtualWrite(V0, "add", 2, "IR2", "V2");
  Blynk.virtualWrite(V0, "add", 3, "IR3", "V3");
  Blynk.virtualWrite(V0, "add", 4, "IR4", "V4");

  lcd.clear();
  lcd.print(0,0,"Tap a line");
  lcd.print(0,1,"to Learn IR");
}

void loop()
{
  Blynk.run();
}

