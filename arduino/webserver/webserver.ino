#include <Bridge.h>
#include <YunServer.h>
#include <YunClient.h>

const bool test = true;
const byte internalLED = 13;
const byte pin = 12;

YunServer server;

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
    /*
     * Bytes to signal out.
     * First and second bytes are to identify the controller and are static.
     * Last is:
     *  - 0x01 if there is a controller pack,
     *  - 0x02 if not,
     *  - 0x04 if the previous controller read/write address CRC showed an error.
    */
    byte out[] = { B00000101,B00000000,B00000000,B00000000,B00000000,B00000000,B00000010 };

    //Encoding the third byte
    //A
    if(str.indexOf("A") != -1){
      out[3] = out[3] | B10000000;
    }
    //B
    if(str.indexOf("B") != -1){
      out[3] = out[3] | B01000000;
    }
    //Start
    if(str.indexOf("S") != -1){
      out[3] = out[3] | B00100000;
    }
    //Z
    if(str.indexOf("Z") != -1){
      out[3] = out[3] | B00010000;
    }
    //D-pad
    if(str.indexOf("Du") != -1){
      out[3] = out[3] | B00001000;
    }
    if(str.indexOf("Dd") != -1){
      out[3] = out[3] | B00000100;
    }
    if(str.indexOf("Dl") != -1){
      out[3] = out[3] | B00000010;
    }
    if(str.indexOf("Dr") != -1){
      out[3] = out[3] | B00000001;
    }

    //Encoding the fourth byte
    //Joy Reset : do nothing
    //Second most significant bit: do nothing
    //L
    if(str.indexOf("L") != -1){
      out[4] = out[4] | B00100000;
    }
    //R
    if(str.indexOf("R") != -1){
      out[4] = out[4] | B00010000;
    }
    //C-Pad
    if(str.indexOf("Cu") != -1){
      out[4] = out[4] | B00001000;
    }
    if(str.indexOf("Cd") != -1){
      out[4] = out[4] | B00000100;
    }
    if(str.indexOf("Cl") != -1){
      out[4] = out[4] | B00000010;
    }
    if(str.indexOf("Cr") != -1){
      out[4] = out[4] | B00000001;
    }
    //Encoding the fifth byte
    //Joystick-X
    int index, val;
    bool isNumber = true;
    String str_val;
    if(str.indexOf("Jx") != -1){
      index = str.indexOf("Jx");
      if(str.length() > index + 3 && isInt(str.substring(index+2, index+6))){
        val = str.substring(index+2, index+6).toInt();
        if(val > 0 && val < 101){
          out[5] = (byte)(val-50);
        }
      }
    }
    //Encoding the sixth byte
    //Joystick-Y
    if(str.indexOf("Jy") != -1){
      index = str.indexOf("Jy");
      if(str.length() > index + 3 && isInt(str.substring(index+2, index+6))){
        val = str.substring(index+2, index+6).toInt();
        if(val > 0 && val < 101){
          out[6] = (byte)(val-50);
        }
      }
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
