// From Tools > Board, select board LOL1N(WEMOS) D1 R2 & MINI


// Reference libraries
#include <DHT_U.h>                              // The library package that knows how to talk to the DHT sensor
#include <OneWire.h>                            // The library package that understands how the DS18B20 sensor communicates
#include <DallasTemperature.h>                  // The library package that knows how to interpret what the DS18B20 sensor does

#include <ESP8266WebServer.h>                   // The library package that can turn this D1 into a webserver over wifi
#include <ESP8266mDNS.h>                        // The library package that can broadcast the name of this device over wifi


//Definitions, Object and Variables
#define DHTPIN D2                               // Name a communication pin for the DHT Air Sensor
#define DHTTYPE DHT11                           // Set the DHT sensor type to DHT-11

DHT dht_sensor(DHTPIN, DHTTYPE);                // Create an "object" named DHT_SENSOR that we can refer to later

#define ONE_WIRE_BUS D1                         // Name a communication pin for the Water Sensor

OneWire oneWire(ONE_WIRE_BUS);                  // Create an "object" named ONEWIRE that manage Water Sensor communication
DallasTemperature ds18b20_sensor(&oneWire);     // Create an "object" named DS18B20_SENSOR that uses the ONEWIRE object

float airTemp=0;                                // Name a set of variables 
float airHumi=0;                                //  That hold a floating point decimal number
float waterC=0;                                 //  Which we can use to capture temperature
float waterF=0;                                 //  and humidity values for air and water

#define WIFI_SSID     "YOUR SSID"               // The name of the WIFI Network to connect to
#define WIFI_PSK      "YOUR PASSWORD"           // Not So Stupid That I Show You The Wifi Password
#define WIFI_HOSTNAME "watersense"              // The name that this device can be identified by on the network

ESP8266WebServer webserver(80);                 // Create an "object" named server WEBSERVER to handle requests for web pages

// Built in Arduino Function called once when the device is powered on.
void setup(void)
{
  Serial.begin(9600);                           // Create a connection to the Serial Monitor Screen
  setupSensors();                               // Call a function to initialize the sensors
  setupWifi();                                  // Call a function to connect to Wifi
  broadcastName();                              // Call a function to tell the Wifi network the name of this machine 
  setupWebserver();                             // Call a function to configure what requests the webserver can handle
} 

//Built in Arduino Function that is called repeatedly while the power is on.
void loop() {
  webserver.handleClient();                     // Get the webserver to look wait for a request to come in and deal with it
  MDNS.update();                                // Make sure the name of this device persists on the Wifi network
}

// A Custom function that holds the code to initialize the sensors
void setupSensors(){
  ds18b20_sensor.begin();                       // Tell the DS18B20_SENSOR object that power has just been turned on
  dht_sensor.begin();                           // Tell the DHT_SENSOR object that power has just been turned on
}

// A Custom function that holds the code to connect to a Wifi network
void setupWifi(){
  WiFi.mode(WIFI_STA);                          // Set this device as a "STA"TION, meaning it can be access over Wifi
  WiFi.begin(WIFI_SSID, WIFI_PSK);              // Try to connect this station to the Wifi network
    
  Serial.println("");                           // Make sure the Serial Display is on a new line  
  while (WiFi.status() != WL_CONNECTED) {       // While the device is not connected to wifi
    delay(500);                                 // Wait half a second
    Serial.print(".");                          // Print a period character to the Serial Display to show time is passing
  }                                             // Repeat this loop until connected
  
  Serial.println("");                           // Make sure the Serial Display is on a new line
  Serial.print("Connected to ");                // Assume everything works and show on the Serial Display
  Serial.println(WIFI_SSID);                    //  the device is connected to this Wifi network
  Serial.print("IP address: ");                 //  and the IP address is
  Serial.println(WiFi.localIP());               //  the one allocated as the local ip address
}

// A Custom function that holds the code to give this device a readable name on the Wifi network
void broadcastName() {
  if (MDNS.begin(WIFI_HOSTNAME)) {              // Try broadcasting the name of this device and if that worked
    Serial.println("MDNS responder started");   //   Print a message to the Serial Monitor Window
    Serial.print("Connect to http://");         //     that the Domain Name Service responder has started and you can use
    Serial.print(WIFI_HOSTNAME);                //     http://THISNAME.local
    Serial.println(".local");                   //     to connect to it
  }  
}

// A Custom function that holds the code to configure and start a webserver on this device
void setupWebserver(){
  webserver.on("/", handleRoot);                // Give the webserver the name of a function to call when a request is made
  webserver.on("/api", handleJSON);             // Tell the webserver to use a different function when this page is requested
  webserver.onNotFound(handleNotFound);         // Tell the webserver to use this function when anything else is requested - 404 Not Found
  webserver.begin();                            // Start the webserver
  Serial.println("HTTP server started");        //Print a message to the Serial Display to say the webserver has started
}


// A Custom function that holds the code deal with a request for the "/" page on the local webserver
void handleRoot() {
  refreshNumbers();                             // Call a function to update the temperature numbers from the sensors
  char RESPONSE[50] = "";                       // Create a memory buffer to hold a response to this request
                                                // Fill that buffer with text contaning
                                                //    The Water Temperature, the Air Temperature and the Air Humidity
                                                // Tell the webserver to send that response back with an HTTP-OK status (200)
                                                
  sprintf(RESPONSE, "Water: %fC\nTemp : %fC\nHumi : %f", waterC, airTemp, airHumi);
  webserver.send(200, "text/plain", RESPONSE);      
}

// A Custom function that holds the code deal with a request for the "/api" page on the local webserver
void handleJSON() {
  refreshNumbers();                             // Call a function to update the temperature numbers from the sensors
  char RESPONSE[50] = "";                       // Create a memory buffer to hold a response to this request
                                                // Fill that buffer with "Javascript Structured Object Notation" contaning
                                                //    The Water Temperature, the Air Temperature and the Air Humidity
                                                // Tell the webserver to send that response back with an HTTP-OK status (200)
  
  sprintf(RESPONSE, "{ \"w\":%f, \"t\":%f, \"h\":%f }", waterC, airTemp, airHumi);
  webserver.send(200, "application/json", RESPONSE); 
}

// A Custom function that holds the code deal with all other requests on the local webserver
void handleNotFound() {
  String message = "Data Not Found\n\n";        // Create a variable to hold a response message
  message += "URI: ";                           // Fill it with information about the request that was made
  message += webserver.uri();                   //   and explain that no data could be found to match the request
  message += "\nMethod: ";
  message += (webserver.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webserver.args();
  message += "\n";
  for (uint8_t i = 0; i < webserver.args(); i++) {
    message += " " + webserver.argName(i) + ": " + webserver.arg(i) + "\n";
  }
  webserver.send(404, "text/plain", message);   // Tell the webserver to send that message back with an HTTP-NOT-FOUND status (404)
}

// A Custom function that holds the code to get the latest temperature readings
void refreshNumbers(){
  Serial.println("");                           // Make sure the Serial Monitor starts on a new line
  readDht();                                    // Invoke the function that reads values from the DHT Sensor
  readDs18b20();                                // Invoke the function that reads values from the DS18B20 Sensor
}

// Custom function to deal with reading and displaying of values from the DHT11 Sensor
void readDht() {
  airTemp = dht_sensor.readTemperature();       // Assign the current temperature to the variable AIRTEMP from the DHT Sensor
  airHumi = dht_sensor.readHumidity();          // Assign the current humidity to the variable AIRHUMI from the DHT Sensor
  Serial.print("Air Temp: ");                   // Print a temperature message to the Serial Monitor Screen
  Serial.print(airTemp);                        // Print the value of AIRTEMP next to that last message
  Serial.println(" C     ");                    // Print a CELCIUS indicator next to the value and then move to a new line
  Serial.print("Air Humidity: ");               // Print a humidity message to the Serial Monitor Screen
  Serial.print(airHumi);                        // Print the value of AIRHUMI next to that last message
  Serial.println(" %     ");                    // Print a PERCENT indictor next to the value and then move to a new line
}

// Custom function to deal with reading and displaying of values from the DS18B20 Sensor
void readDs18b20() {
  ds18b20_sensor.requestTemperatures();         // Tell DS18B20 Sensor (via the ONEWIRE object) that a temperature is needed
  waterC=ds18b20_sensor.getTempCByIndex(0);     // Assign the temperature (in CELCIUS), sent down the ONEWIRE line to the variable WATERC
  waterF=ds18b20_sensor.toFahrenheit(waterC);   // Use a built in function to convert WATERC to Fahrenheit and assign to variable WATERF
  Serial.println("Water Temp:     ");           // Print a Water temperature message to the Serial Monitor Screen
  Serial.print(waterC);                         // Print the value of WATERC next to the last message
  Serial.print("C / ");                         // Print a CELCIUS indicator next to the value and a divider too
  Serial.print(waterF);                         // Print the value of WATERF next to the last message
  Serial.println("F          ");                // Print a FARHENHEIT indictor next to the value and then move to a new line
}
