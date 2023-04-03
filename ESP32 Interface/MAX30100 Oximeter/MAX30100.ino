// MAX30100 PulseOximeter Interface with ESP32//

// Vedant0891 //
// Instagram @TechnoLord.epg //
// Youtube @ TechnoLord Programming //


#include <WiFi.h>
#include <WebServer.h>
#include "MAX30100_PulseOximeter.h"
#include "html.h"

WebServer server(80);

const char* ssid = "*Your SSID";    /*Enter Your SSID*/
const char* password = "*Your Password*"; /*Enter Your Password*/


#define REPORTING_PERIOD_MS     1000

// Creating object for PulseOximeter class as pox
PulseOximeter pox;

uint32_t tsLastReport = 0;
float HeartRate,SpO;

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void MainPage() {
  String _html_page = html_page;              /*Read The HTML Page*/
  server.send(200, "text/html", _html_page);  /*Send the code to the web server*/
}

void MAX30100() {
  String data = "[\""+String(HeartRate)+"\",\""+String(SpO)+"\"]";
  server.send(200, "text/plane", data);
}

void setup()
{
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);                  /*Set the WiFi in STA Mode*/
    WiFi.begin(ssid, password);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    delay(1000);                          /*Wait for 1000mS*/
    while(WiFi.waitForConnectResult() != WL_CONNECTED){Serial.print(".");} /*Wait while connecting to WiFi*/
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("Your Local IP address is: ");
    Serial.println(WiFi.localIP());       /*Print the Local IP*/
   
    server.on("/", MainPage);             /*Display the Web/HTML Page*/
    server.on("/readmax30100", MAX30100); /*Display the updated Distance value(CM and INCH)*/
    server.begin();                       /*Start Server*/
    delay(1000);                          /*Wait for 1000mS*/


    Serial.print("Initializing pulse oximeter..");

    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }

    // The default current for the IR LED is 50mA and it could be changed
    //   by uncommenting the following line. Check MAX30100_Registers.h for all the
    //   available options.
    // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Register a callback for the beat detection
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop()
{
    // Make sure to call update as fast as possible
    pox.update();
    server.handleClient();

    // Asynchronously dump heart rate and oxidation levels to the serial
    // For both, a value of 0 means "invalid"
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
      
        HeartRate = pox.getHeartRate();
        Serial.print("Heart rate:");
        Serial.print(HeartRate);
        
        SpO = pox.getSpO2();
        Serial.print("bpm / SpO2:");
        Serial.print(SpO);
        Serial.println("%");

        tsLastReport = millis();
    }
}
