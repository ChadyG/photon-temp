// This #include statement was automatically added by the Particle IDE.
#include "OneWire.h"
#include "DHT.h"
#include "Temperature.h"

// Use this include for Particle Dev where everything is in one directory.
// #include "OneWire.h"

// OneWire DS18S20, DS18B20, DS1822, DS2438 Temperature Example
//
// https://github.com/Hotaman/OneWireSpark
//
// Thanks to all who have worked on this demo!
// I just made some minor tweeks for the spark core
// and added support for the DS2438 battery monitor
// 6/2014 - Hotaman

// Uncomment whatever type you're using!
#define DHTTYPE DHT11    // DHT 11
//#define DHTTYPE DHT22    // DHT 22 (AM2302)
//#define DHTTYPE DHT21    // DHT 21 (AM2301)
#define DHTPIN 2     // what pin we're connected to

// Define the pins we will use
int ow = D0;    // put the onewire bus on D0

OneWire  ds(ow);  // a 1 - 4.7K resistor to 3.3v is necessary
byte addr[8];

DHT dht(DHTPIN, DHTTYPE);


void setup(void) {
  //Spark.variable("celsius", &celsius, INT);
  Serial.begin(115200);  // local hardware test only
  dht.begin();
}

void query_ds18b20() {
  byte i;
  byte present = 0;
  byte data[12];

  ds.reset_search();
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    ds.reset_search();
    delay(1000);
    return;
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");
    Spark.publish("Error", "CRC is not valid!");
    delay(2000);
    return;
  }
  Serial.println();


  /*for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }*/
  String rom(addr[0], HEX);
  Spark.publish("ROM", rom);

  // the first ROM byte indicates which chip
  // includes debug output of chip type
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      Spark.publish("Chip", "DS18S20");
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      Spark.publish("Chip", "DS18B20");
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      Spark.publish("Chip", "DS1822");
      break;
    case 0x26:
      Serial.println("  Chip = DS2438");
      Spark.publish("Chip", "DS2438");
      break;
    default:
      Serial.println("Device is not a DS18x20/DS1822/DS2438 device. Skipping...");
      Spark.publish("Error", "Device is not a DS18B20 device. Skipping...");
      delay(2000);
      return;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end

  // we might do a ds.depower() here, but the reset will take care of it.

  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad 0

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }


  Rational temp = getTemp(data);
  Serial.print(temp.sign ? "-" : "");
  Serial.print(temp.whole);
  Serial.print(".");
  Serial.print(temp.fract);
  Serial.println("C");

  String whole(temp.whole, DEC);
  String fract(temp.fract, DEC);
  Spark.publish("Celsius", whole + "." + fract + " C");
}

void query_dht11() {
  // Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a
// very slow sensor)
  float h = dht.getHumidity();
// Read temperature as Celsius
  float t = dht.getTempCelcius();
// Read temperature as Farenheit
  float f = dht.getTempFarenheit();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

// Compute heat index
// Must send in temp in Fahrenheit!
  float hi = dht.getHeatIndex();
  float dp = dht.getDewPoint();
  float k = dht.getTempKelvin();

  Serial.print("Humid: ");
  Serial.print(h);
  Serial.print("% - ");
  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print("*C ");
  Serial.print(f);
  Serial.print("*F ");
  Serial.print(k);
  Serial.print("*K - ");
  Serial.print("DewP: ");
  Serial.print(dp);
  Serial.print("*C - ");
  Serial.print("HeatI: ");
  Serial.print(hi);
  Serial.println("*C");
  Serial.println(Time.timeStr());
}

void loop(void) {
  query_ds18b20();
  query_dht11();
  delay(1000);
}
