/*************************************************************

  This is a simple demo of sending and receiving some data.
  Be sure to check out other examples!
 *************************************************************/

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#define LED_VERMELHO 22

#define LED_AMARELO 21
#define LED_VERDE 23
#define LDR_PIN 32
#define BUZZER 33

unsigned long int tempo_led_amarelo = 5000;
unsigned long int tempo_led_verde = 5000;
unsigned long int tempo_led_vermelho = 5000;
unsigned long int tempo_sev_seg_atual = 5;
unsigned long int last_time = 0;
unsigned long int last_time_sev_seg = 0;

unsigned int buzzerFrequency = 220;

int valor_ldr_anterior = 0;
int contador_ldr = 0;
int lock_contador = 0;

int state = 0;

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include "SevSeg.h"  /*Included seven-segment library*/
SevSeg sevseg;       /*Create a seven-segment library*/

char pass[] = "";
char ssid[] = "";

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V0){
  tempo_led_verde = param.asInt() * 1000; // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Tempo led verde: ");
  Serial.println(tempo_led_verde);
  Serial.print("Tempo led amarelo: ");
  Serial.println(tempo_led_amarelo);
  Serial.print("Tempo led vermelho: ");
  Serial.println(tempo_led_vermelho);
}

BLYNK_WRITE(V1){
  tempo_led_amarelo = param.asInt() * 1000; // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Tempo led verde: ");
  Serial.println(tempo_led_verde);
  Serial.print("Tempo led amarelo: ");
  Serial.println(tempo_led_amarelo);
  Serial.print("Tempo led vermelho: ");
  Serial.println(tempo_led_vermelho);
}

BLYNK_WRITE(V2){
  tempo_led_vermelho = param.asInt() * 1000; // assigning incoming value from pin V1 to a variable
  // You can also use:
  // String i = param.asStr();
  // double d = param.asDouble();
  Serial.print("Tempo led verde: ");
  Serial.println(tempo_led_verde);
  Serial.print("Tempo led amarelo: ");
  Serial.println(tempo_led_amarelo);
  Serial.print("Tempo led vermelho: ");
  Serial.println(tempo_led_vermelho);
}

BLYNK_WRITE(V4) {
  buzzerFrequency = param.asInt();
  Serial.print("Buzzer frequency: ");
  Serial.println(buzzerFrequency);
}

void setup(){
  // Debug console
  Serial.begin(115200);

  byte sevenSegments = 1;  /*Number of connected seven-segment*/
  byte CommonPins[] = {};  /*Define Common pin of seven-segment*/
  byte LEDsegmentPins[] = {13, 17, 19, 27, 26, 14, 12};  /*Define ESP32 digital pins for seven-segment*/
  bool resistorsOnSegments = true; /*assigning Boolean type to the registers of the seven=segment*/
  sevseg.begin(COMMON_CATHODE, sevenSegments, CommonPins, LEDsegmentPins, resistorsOnSegments);/*seven-segment configuration */
  sevseg.setBrightness(80);  /*Seven segment brightness*/

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  last_time = millis();
  last_time_sev_seg = millis();
  digitalWrite(LED_VERMELHO, LOW);
  digitalWrite(LED_AMARELO, LOW);
  digitalWrite(LED_VERDE, HIGH);

  sevseg.setNumber(tempo_sev_seg_atual);
  sevseg.refreshDisplay();
}

void loop() {
  Blynk.run();

  if(state == 2){
     int valor_ldr_atual = analogRead(LDR_PIN);
     if(valor_ldr_atual <= 150 && lock_contador == 0) {
        lock_contador = 1;
        Blynk.virtualWrite(V3, ++contador_ldr);
        Serial.println(contador_ldr);
     } else if(valor_ldr_atual > 400 && lock_contador == 1) {
        lock_contador = 0; 
     }
  }

  if(millis() - last_time_sev_seg > 1000) {
    last_time_sev_seg = millis();
    tempo_sev_seg_atual--;
    sevseg.setNumber(tempo_sev_seg_atual);
    sevseg.refreshDisplay();
  }
  
  switch(state){
    case 0: // LED VERDE
      if(millis() - last_time > tempo_led_verde) {
        last_time = millis();
        digitalWrite(LED_VERMELHO, LOW);
        digitalWrite(LED_AMARELO, HIGH);
        digitalWrite(LED_VERDE, LOW);
        tempo_sev_seg_atual = tempo_led_amarelo / 1000;
        sevseg.setNumber(tempo_sev_seg_atual);
        sevseg.refreshDisplay();
        state = 1;
      }
     break;
   case 1: // LED AMARELO
      if(millis() - last_time > tempo_led_amarelo) {
        last_time = millis();
        digitalWrite(LED_VERMELHO, HIGH);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERDE, LOW);
        tempo_sev_seg_atual = tempo_led_vermelho / 1000;
        sevseg.setNumber(tempo_sev_seg_atual);
        sevseg.refreshDisplay();
        tone(BUZZER, buzzerFrequency);
        state = 2;
      }
    break;
    case 2: // LED VERMELHO
      if(millis() - last_time > tempo_led_vermelho) {
        last_time = millis();
        digitalWrite(LED_VERMELHO, LOW);
        digitalWrite(LED_AMARELO, LOW);
        digitalWrite(LED_VERDE, HIGH);
        tempo_sev_seg_atual = tempo_led_verde / 1000;
        sevseg.setNumber(tempo_sev_seg_atual);
        sevseg.refreshDisplay();
        tone(BUZZER, 0);
        state = 0;
      }
    break;
  }
}
