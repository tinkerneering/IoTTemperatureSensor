// Select TOOLS > BOARD > ARDUINO / GENUINO UNO

// Reference libraries
#include <DHT_U.h>                              // The library package that knows how to talk to the DHT sensor
#include <OneWire.h>                            // The library package that understands how the DS18B20 sensor works
#include <DallasTemperature.h>                  // The library package that knows how to interpret what the DS18B20 sensor does
#include <LiquidCrystal.h>                      // The library package that knows how to control the LCD display (Tautology Rob!)

//Definitions, Object and Variables
#define DHTPIN 2                                // Name a communication pin for the DHT Air Sensor
#define DHTTYPE DHT11                           // Set the DHT sensor type to DHT-11

DHT dht_sensor(DHTPIN, DHTTYPE);                // Create an "object" named DHT_SENSOR that we can refer to later

#define ONE_WIRE_BUS 3                          // Name a communication pin for the Water Sensor

OneWire oneWire(ONE_WIRE_BUS);                  // Create an "object" named ONEWIRE that manage Water Sensor communication
DallasTemperature ds18b20_sensor(&oneWire);     // Create an "object named DS18B20_SENSOR using the ONEWIRE object

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);        // Create an object call LCD that we can refer to later using reserved communication pins 

float airTemp=0;                                // Name a set of variables 
float airHumi=0;                                //  That hold a floating point decimal number
float waterC=0;                                 //  Which we can use to capture temperature
float waterF=0;                                 //  and humidity values for air and water

// Built in Arduino Function called when the device is powered on.
void setup(void)                    
{  
  ds18b20_sensor.begin();                       // Tell the DS18B20_SENSOR object that power has just been turned on
  dht_sensor.begin();                           // Tell the DHT_SENSOR object that power has just been turned on
  lcd.begin(16,2);                              // Initialize the LCD and specify its dimensions (16 Characters wide, 2 Lines deep)
}

//Built in Arduino Function that is called repeatedly while the power is on.
void loop() {
  readDht();                                    // Invoke the function that reads values from the DHT Sensor
  delay(2000);                                  // Wait 2 seconds
  readDs18b20();                                // Invoke the function that reads values from the DS18B20 Sensor
  delay(2000);                                  // Wait 2 more seconds
}

// Custom function to deal with reading and displaying of values from the DHT11 Sensor
void readDht() {
  airTemp = dht_sensor.readTemperature();       // Assign the current temperature to the variable AIRTEMP from the DHT Sensor
  airHumi = dht_sensor.readHumidity();          // Assign the current humidity to the variable AIRHUMI from the DHT Sensor
  lcd.setCursor(0,0);                           // Position the text cursor on the top line of the LCD
  lcd.print("Temp: ");                          // Print a temperature message to the LCD
  lcd.print(airTemp);                           // Print the value of AIRTEMP next to that last message
  lcd.print(" C     ");                         // Print a CELCIUS indicator next to the value
  lcd.setCursor(0,1);                           // Position the text cursor on the bottom line of the LCD
  lcd.print("Humi: ");                          // Print a humidity message to the LCD
  lcd.print(airHumi);                           // Print the value of AIRHUMI next to that last message
  lcd.print(" %     ");                         // Print a PERCENT indictor next to the value 
}

// Custom function to deal with reading and displaying of values from the DS18B20 Sensor
void readDs18b20() {
  ds18b20_sensor.requestTemperatures();         // Tell DS18B20 Sensor (via the ONEWIRE object) that a temperature is needed
  waterC=ds18b20_sensor.getTempCByIndex(0);     // Assign the temperature (in CELCIUS), sent down the ONEWIRE line to the variable WATERC
  waterF=ds18b20_sensor.toFahrenheit(waterC);   // Use a built in function to convert WATERC to Fahrenheit and assign to variable WATERF
  lcd.setCursor(0,0);                           // Position the text cursor on the top line of the LCD
  lcd.print("Water Temp:     ");                // Print a Water temperature message to the LCD
  lcd.setCursor(0,1);                           // Position the text cursor on the bottom line of the LCD
  lcd.print(waterC);                            // Print the value of WATERC next to the last message
  lcd.print("C / ");                            // Print a CELCIUS indicator next to the value and a divider too
  lcd.print(waterF);                            // Print the value of WATERF next to the last message
  lcd.print("F          ");                     // Print a FARHENHEIT indictor next to the value
}
