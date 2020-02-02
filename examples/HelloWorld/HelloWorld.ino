#include <HT1622.h>
#include <displays/DM8BA10.h>
#include <SPI.h>

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
  
  displayOn = millis();
}


void loop() {
  if (millis() - displayOn > 1000) {
    htDriver.seekLeft();
    htDriver.print("He");
    delay(2000);
    htDriver.print("l");
    delay(2000);
    htDriver.print("l");
    delay(2000);
    htDriver.print("o");
    delay(2000);
    //htDriver.print(millis() / 1000);

    displayOn = millis();
  }
}
