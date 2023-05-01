/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */
#define LED 5
#define BUZZER 9
#include <WiFi.h>

const char* ssid     = "CINGUESTS";
const char* password = "acessocin";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(LED, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}
float leitura =500;
float sensorReading = 700;
int value = 0;

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Clique <a href=\"/HL\">here</a> para aumentar o brilho do LED.<br>");
            client.print("Clique <a href=\"/HL\">here</a> para diminuir o brilho do LED.<br>");
            client.print("Clique <a href=\"/HB\">here</a> para aumentar a intensidade do Buzzer.<br>");
            client.print("Clique <a href=\"/LB\">here</a> para diminuir a intensidade do Buzzer.<br>");
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /HL")) {
          leitura = leitura*1,1;
          int pwm = map(leitura,0,1023,0,255);
                              
          analogWrite(LED, pwm);              // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /LL")) {
          leitura = leitura*0,9;
          int pwm = map(leitura,0,1023,0,255);
          
  
          analogWrite(LED, pwm);          // GET /L turns the LED off
        }
         if (currentLine.endsWith("GET /HB")) {
          sensorReading = sensorReading*1,1;
          int thisPitch = map(sensorReading, 400, 1000, 120, 1500);
        
          // play the pitch:
          tone(9, thisPitch, 10);
                    // GET /L turns the LED off

        }
         if (currentLine.endsWith("GET /LB")) {
               // GET /L turns the LED off
              sensorReading = sensorReading*0,9;
              int thisPitch = map(sensorReading, 400, 1000, 120, 1500);
            
              // play the pitch:
              tone(BUZZER, thisPitch, 10);
               
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
