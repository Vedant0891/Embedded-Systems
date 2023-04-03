// BMP280 Interface with NodeMCU //
// Local Host WebServer //

// Vedant0891 //
// Instagram @TechnoLord.epg //
// Youtube @TechnoLord Programming //

#include <Adafruit_BMP280.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include "html.h"

ESP8266WebServer server(80);
Adafruit_BMP280 bmp; // I2C

const char* ssid = "ElectronicWings";         /*Enter Your SSID*/
const char* password = "ewings@123"; /*Enter Your Password*/

int pre,temp,alt;

void MainPage() {
  String _html_page = html_page;              /*Read The HTML Page*/
  server.send(200, "text/html", _html_page);  /*Send the code to the web server*/
}

void bmp280() {
  String data = "[\""+String(temp)+"\",\""+String(pre)+"\",\""+String(alt)+"\"]";
  server.send(200, "text/plane", data);
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);                  /*Set the WiFi in STA Mode*/
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  delay(1000);                         /*Wait for 1000mS*/
  while(WiFi.waitForConnectResult() != WL_CONNECTED){Serial.print(".");} /*Wait while connecting to WiFi*/
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Your Local IP address is: ");
  Serial.println(WiFi.localIP());       /*Print the Local IP*/

  server.on("/", MainPage);             /*Display the Web/HTML Page*/
  server.on("/readBMP280", bmp280); /*Display the updated data*/
  server.begin();                      /*Start Server*/
  delay(1000);                         /*Wait for 1000mS*/
  
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  unsigned status;
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); 
    Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() {
    server.handleClient();
    temp = bmp.readTemperature();
    Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" *C");

    pre = bmp.readPressure()/100.0F;
    Serial.print("Pressure = ");
    Serial.print(pre);
    Serial.println(" Pa");

    alt = bmp.readAltitude(1013.25);
    Serial.print("Approx altitude = ");
    Serial.print(alt); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
    delay(2000);
}
