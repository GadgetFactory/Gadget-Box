/**************************************************************
 * Use this sketch to test the Button and LED eCog
 * 
 * eCogs Used:
 *       eCog106 - Button & LED - Connect to eCog D
 *          -- Outputs Button States to Serial Port @ 9600 Baud
 * 
 * Libraries required (Install with Library Manager):
 *       GadgetBox
 *       
 * GadgetBox Motherboards supported:      
 *       Arduino Mini Pro
 *       
 *
 *  Demo by emuDrache
 *
 **************************************************************/
 
 #include <GadgetBox.h>
 
 #define eCogButton1 DC0
 #define eCogButton2 DC1 // analogRead Only Pin when in eCog C or D! (A6/GB9)
 #define eCogButton3 DC2 // analogRead Only Pin when in eCog C or D! (A7/GB10)
 #define eCogButton4 DC3
 
 #define ArduinoLED 13   // Arduino soldered to Arduino Mini Pro
 
 /*
  *
  * Handles reading from the digital/analog pin mishmash,
  * as the Atmega 328 doesn't like digitalRead from A6 / A7
  *
  */
 uint8_t gfDigitalRead(uint8_t buttonPin)
 {
   uint8_t pinState = 0;
   if (buttonPin == A6 || buttonPin == A7)
   {
     return analogRead(buttonPin) > 10 ? 1 : 0;
   } else {
     return digitalRead(buttonPin);
   }
 }
 
 void setup() {
 
  // init serial port
  Serial.begin(9600);
  
  // init Arduino LED
  pinMode(ArduinoLED, OUTPUT);
  
  // Set proper pins to INPUT (INPUT_PULLUP NOT - eCog has pulldowns)
  pinMode(eCogButton1, INPUT);
  pinMode(eCogButton2, INPUT);
  pinMode(eCogButton3, INPUT);
  pinMode(eCogButton4, INPUT);
    
 }
 
 void loop() {
 
   digitalWrite(ArduinoLED, HIGH);
 
   Serial.print("Buttons: ");
   Serial.print(gfDigitalRead(eCogButton1), DEC);
   Serial.print(gfDigitalRead(eCogButton2), DEC);
   Serial.print(gfDigitalRead(eCogButton3), DEC);
   Serial.println(gfDigitalRead(eCogButton4), DEC);

   delay(500);

   /*
     if (gfDigitalRead(eCogButton1) == HIGH)
     {
       Serial.println("FOO");
     } else {
       Serial.println("BAR");
     }
    */
   
   digitalWrite(ArduinoLED, LOW);
   delay(500);
 
 }
 
