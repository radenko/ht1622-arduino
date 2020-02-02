#include <HT1622.h>
#include <displays/DM8BA10.h>
#include <SPI.h>

#define ONESHOT_DRAW

#define BLED 25

#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 22

HT1622 htDriver;

unsigned long displayOn;

void setup() {    
  Serial.begin(115200);
  DM8BA10_setup(htDriver);
  SPI.begin(SCK, MISO, MOSI, -1);
  htDriver.begin(SPI, SS, BLED);
  //htDriver.begin(SS, SCK, MOSI);
  
  displayOn = 0;
}


void loop() {
  //redraw every 60 seconds
  if (displayOn == 0 || millis() - displayOn > 60000) {
    htDriver.clear();
#ifdef ONESHOT_DRAW
    htDriver.noRedraw = true;
    htDriver.print("He");
    htDriver.print("l");
    htDriver.print("l");
    htDriver.print("o");
    htDriver.print(millis() / 60000);
    
    htDriver.wrBuffer();
#else
    htDriver.noRedraw = false;
    htDriver.print("He");
    delay(2000);
    htDriver.print("l");
    delay(2000);
    htDriver.print("l");
    delay(2000);
    htDriver.print("o");
    delay(2000);
    htDriver.print(millis() / 60000);
#endif

    displayOn = millis();    
  }
}
