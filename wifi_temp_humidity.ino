/*
 * DHT-22 and Wifi Example
 * Philip Hamilton
 */
// Libraries
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <WiFiNINA.h>
#include <DHT.h>

#include "arduino_secrets.h"

// Constants
#define DHTPIN 0     // Digital Pin Used
#define DHTTYPE DHT22   // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize

// Wifi Variables
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// Variables
char serverAddress[] = SERVER_ADDRESS;
int port = SERVER_PORT;
char roomName[] = ROOM_NAME;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
float hum;  // Humidity Value
float temp; // Temperature value



void setup()
{
  Serial.begin(9600);

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("You're connected to the network");
  Serial.println("----------------------------------------");
  printData();
  Serial.println("----------------------------------------");
  
  dht.begin();
  
}

void loop()
{
    delay(2000);
    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp= dht.readTemperature();
    //Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");
    printData();
    Serial.println("----------------------------------------");

    //Check wifi status - Future Feature
    
    String contentType = "application/json";
    
    DynamicJsonDocument jsonData(1024);
    jsonData["roomName"] = roomName;
    jsonData["roomTemp"] = String(hum,4);
    jsonData["roomHumidity"] = String(temp, 4);
    
    String postData;
    serializeJson(jsonData, postData);
    client.post("/RoomData", contentType, postData);
    
    // read the status code and body of the response
    int statusCode = client.responseStatusCode();
    String response = client.responseBody();
    
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response: ");
    Serial.println(response);
    
    delay(10000); //Delay 2 sec.
}


void printData() {
  Serial.println("Board Information:");
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}
