// include the library code:
#include <LiquidCrystal.h>
#include <OneWire.h>

int DS18S20_Pin = 7; //DS18S20 Signal pin on digital 7
OneWire ds(DS18S20_Pin);

// build lcd based off pins it is hooked to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// pin buzzer is hooked to
int buzzerpin = 8;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Freezer Temp:");
  //temperature stuff
  Serial.begin(9600);
  tone(buzzerpin, 2600, 400);
}

void loop() {
  lcd.clear();
  lcd.begin(16,2);
  lcd.print("Freezer Temp:");
  lcd.setCursor(0, 1);

  //temperature stuff
  float temperature = getTemp();
  int faren = round(1.8*temperature+32);
  lcd.print(faren);
  lcd.print((char)223);
  lcd.print("F");
  if ( faren > 15 ) {
    tone(buzzerpin, 2600, 400);
  }

  delay(5000); // updates every 5 seconds
}

float getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius
  byte data[12];
  byte addr[8];
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      return -1000;
  }
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return -1000;
  }
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      return -1000;
  }
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
  
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  
  ds.reset_search();
  
  byte MSB = data[1];
  byte LSB = data[0];
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum;
  
}
