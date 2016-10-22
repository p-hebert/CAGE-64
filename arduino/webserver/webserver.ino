#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

const bool test = true;
const byte internalLED = 13;
const byte pin = 12;

YunServer server;

/*
 * Bytes to signal out.
 * First and second bytes are to identify the controller and are static.
 * Last is:
 *  - 0x01 if there is a controller pack,
 *  - 0x02 if not,
 *  - 0x04 if the previous controller read/write address CRC showed an error.
*/
const byte outBytes[] = { B00000101,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010 };
byte out[] = { B00000101,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010 };

void processInput(char* input){

  switch(input[0]){
    case 'A':    
      out[3] = out[3] | B10000000;
      break;
    case 'B':
      out[3] = out[3] | B01000000;
      break;
    case 'S':
      out[3] = out[3] | B00100000;
      break;
    case 'Z':
      out[3] = out[3] | B00010000;
      break;
    case 'D':
      switch(input[1]){
        case 'u':
          out[3] = out[3] | B00001000;
          break;
        case 'd':
          out[3] = out[3] | B00000100;
          break;
        case 'l':
          out[3] = out[3] | B00000010;
          break;
        case 'r':
          out[3] = out[3] | B00000001;
          break;
      }
      break;
    case 'L':
      out[4] = out[4] | B00100000;
      break;
    case 'r':
      out[4] = out[4] | B00010000;
      break;
    case 'C':
      switch(input[1]){
        case 'u':
          out[4] = out[4] | B00001000;
          break;
        case 'd':
          out[4] = out[4] | B00000100;
          break;
        case 'l':
          out[4] = out[4] | B00000010;
          break;
        case 'r':
          out[4] = out[4] | B00000001;
          break;
      }
      break;
    case 'J':
      char value[3] = {input[2], input[3], input[4]};
        int val = atoi(value);
        if(val > 0 && val < 101){
          switch(input[1]){
          case 'x':
            out[5] = (byte)(val-50);
            break;
          case 'y':
            out[6] = (byte)(val-50);
            break;
          }
        }
      break;
  }
}

void setup() {
  pinMode(internalLED, OUTPUT);
  pinMode(pin, INPUT);
  Bridge.begin();          // Let's activate the Yun Bridge...
  ledBlink(150);           // ...bridge activation done (user sees a blink)
  server.begin();          // Let's acrivate the Yun server...
  ledBlink(150);           // ...server activation done (user sees a blink)
}

void loop() {
  YunClient client = server.accept();  // Get clients coming from server
  
  if (client)                          // Is there a new client?
  {
    String str = client.readString();

    char* cstr = strdup(str.c_str());

    char* input = strtok (cstr,":");
    while (input != NULL)
    {
      processInput(input);
      input = strtok (NULL, ":");
    }
        
    //Signal the bytes out!
    if(test){
      for(int i = 0 ; i < sizeof(out)/sizeof(byte) ; i++){
        client.print((short)out[i]);
        if(i != sizeof(out)/sizeof(byte) - 1) client.print(",");
      }
      client.println();
      for(int i = 0 ; i < sizeof(out)/sizeof(byte) ; i++){
        for(int j = 8 ; j > 0 ; j = j-1){
          client.print((short)bitRead(out[i],j));
        }
        if(i != sizeof(out)/sizeof(byte) - 1) client.print(",");
      }
      client.stop();
      ledBlink(150);
      ledBlink(150);
      ledBlink(150);
    }else{
      for(int i = 0 ; i < sizeof(out)/sizeof(byte) ; i++){
        for(int j = 8 ; j > 0 ; j--){
          if(bitRead(out[i],j) == 1){
            //Controller 1 Bit (1us low, 3us high)
            digitalWrite(pin, LOW);
            delayMicroseconds(1);
            digitalWrite(pin, HIGH);
            delayMicroseconds(3);
          }else{
            //Controller 0 Bit (3us low, 1us high)
            digitalWrite(pin, LOW);
            delayMicroseconds(3);
            digitalWrite(pin, HIGH);
            delayMicroseconds(1);
          }
        }
      }
      //Controller Stop Bit (2us low, let high)
      digitalWrite(pin, LOW);
      delayMicroseconds(2);
      digitalWrite(pin, HIGH);
      }
    // Close connection and free resources.
    delay(10);
  }else{
    delay(50);
  }

}

bool isInt(String str){
  for(int i = 0 ; i < str.length(); i++){
    if(!isDigit(str[i])){
      return false;
    }
  }
  return true;
}

void ledBlink(int delayTime) {
  digitalWrite(internalLED, HIGH); // Turn (or keep) the LED on
  delay(delayTime);
  digitalWrite(internalLED, LOW);  // Turn (or keep) the LED off
  delay(delayTime);
}
