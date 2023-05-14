// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------

#include <NewPing.h> //Ultrasonic sensor

#include <SPI.h> //ConectWifi, etc.
#include <WiFiNINA.h>
#include "arduino_secrets.h" 

#define TRIGGER_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     8  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). 

float filterArray[10]; // array to store data samples from sensor

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;     // the WiFi radio's status



// Vars for HTTP Request
char server[] = "46.101.152.229";
String postData;

String postVariable = "sensorId=start&distanceValue=";
WiFiClient client;


void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.



/**
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
**/

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}


void loop() 
{
  delay(50);                     // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  int distance = sonar.ping_cm();
  
  if (distance > 0) 
  {
    int n = 0;
    int sample;
    for (sample; distance > 0; n++) 
    {
        sample = n;
                
        filterArray[sample] = sonar.ping_cm();
                        
        //Serial.print("Sample: ");  //Comprobacion de los registros y las distancias
        //Serial.println(sample);
        //Serial.print("distance: ");
        //Serial.println(filterArray[sample]);
        delay(50); // to avoid untrasonic interfering

        distance = sonar.ping_cm(); //para salir del for
            
    }
    //Serial.println("fuera del for"); //comprobacion sale del for
    //Serial.println(sample); //comprobacion Nº de sample

    int minVal = filterArray[0]; //Buscar el valor minimo
    
    for (int i = 1; i <= sample; i++) 
    {
      minVal = min(filterArray[i],minVal); //valor minimo****************############
      
    }

    Serial.print("Distance: ");
    Serial.print(minVal); // Escribe el valor minimo de todos los registros
    Serial.println("cm");

    postData = postVariable + minVal;

    if (client.connect(server, 4000)) {
        client.println("POST /post.php HTTP/1.1");
        client.println("Host: 46.101.152.229");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(postData.length());
        client.println();
        client.print(postData);
      }

      if (client.connected()) {
        client.stop();
      }
      Serial.println(postData);

      Serial.print("Distance: ");
      Serial.print(minVal); // Escribe el valor minimo de todos los registros
      Serial.println("cm");
      delay(5000); //delay despues de medir print distancia
    
  }
}
void printWifiData() 
{
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  //Serial.print("MAC address: ");
}





void printCurrentNet() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");


  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

}