/**************************************************************
 * Use this sketch to test the Button and LED eCog
 * 
 * eCogs Used:
 *       eCog106 - Button & LED - Connect to eCog D
 *          -- Holding Button 3 makes LED 4 blink fast
 *          -- Holding Button 2 makes LED 1 mimic the Arduino LED
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
 
 #define eCogLED1 DC0
 #define eCogLED2 DC1 // analogRead Only Pin when in eCog C or D! (A6/GB9)
 #define eCogLED3 DC2 // analogRead Only Pin when in eCog C or D! (A7/GB10)
 #define eCogLED4 DC3


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
 
 /*
  *
  * Handles reading from the digital/analog pin mishmash,
  * as the Atmega 328 doesn't like analogWrite to A6 / A7
  *
  */
 void gfDigitalWrite(uint8_t buttonPin, pinState)
 {
   if (buttonPin == A6 || buttonPin == A7)
   {
     // just in case - lets do nothing.  trying to write to A6/A7 pins crashed my arduino on occasion
     // pinState == 1 ? analogWrite(buttonPin, 127) : analogWrite(pin, 0);
   } else {
     digitalWrite(buttonPin, pinState);
   }
 }

 void setup() {
 
  // init Arduino LED
  pinMode(ArduinoLED, OUTPUT);
  
  // Set proper pins to INPUT/OUTPUT (INPUT_PULLUP NOT - eCog has pulldowns)
  pinMode(eCogLED1, OUTPUT);
  pinMode(eCogButton2, INPUT);
  pinMode(eCogButton3, INPUT);
  pinMode(eCogLED4, OUTPUT);
    
 }
 
 void loop() {
 
   digitalWrite(ArduinoLED, HIGH);

   // Make LED 4 blink if eCog Button 3 is held down
   while (gfDigitalRead(eCogButton3))
   {
      gfDigitalWrite(eCogLED4, HIGH);
      delay(100);
      gfDigitalWrite(eCogLED4, LOW);
      delay(100);
   }

   // Make LED 1 mimic Arduino LED if eCog Button 2 is held down
   if (gfDigitalRead(eCogButton2))
   {
      gfDigitalWrite(eCogLED1, HIGH);
   }

   delay(500);

   digitalWrite(ArduinoLED, LOW);

   // Make LED 1 mimic Arduino LED if eCog Button 2 is held down
   if (gfDigitalRead(eCogButton2))
   {
      gfDigitalWrite(eCogLED1, LOW);
   }

   delay(500);
 
 }
