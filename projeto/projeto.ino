#include <HCSR04.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define ECHO_PIN 18
#define TRIGGER_PIN 19
#define LDR 34
#define BUZZER 33

UltraSonicDistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN);

/**
 * @brief Configurações do NTP
 * 
 */
const char* ntpServer = "south-america.pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -10800;

/**
 * @brief Configuração do Wifi
 * 
 */
const char* ssid = "CINGUESTS";
const char* password = "acessocin";

/**
 * @brief Configurações do MQTT
 * 
 */
const char* serverMQTT = "192.168.*.*"; 
const char* topicoComida = "comida";
const char* usernameMQTT = "neo"; // MQTT username
const char* passwordMQTT = "eglabs"; // MQTT password
const char* clientID = "Weather_Reporter"; // MQTT client ID

WiFiClient wiFiClient;

PubSubClient pubSubClient(serverMQTT, 1883, wiFiClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

/**
 * @brief Máquina de estados
 * 
 */
#define INIT 0
#define IDLE 1
#define ENCHENDO_POTE 2
#define ESPERANDO_ANIMAL_PARA_COMER 3
#define ANIMAL_COMENDO 4
#define COMEU_TUDO 5

unsigned int horaEncheuPote = 0;

unsigned char estado = INIT;
boolean configRecebida = false;

unsigned int PESO_CONFIGURACAO_MAXIMO = 0;
unsigned int PESO_CONFIGURACAO_MINIMO = 0;
unsigned int pesoPote = 0;

int MAX_DISTANCE_CONFIG = 25;
float distanciaSensor = 0;
int maxLdrValue = 2000;
int buzzerFrequency = 2000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  delay(50);
  Serial.println();
  Serial.println("Projeto Sistemas Embutidos");

  Serial.begin(115200);
  
  // Connect to MQTT
  iniciarMQTTeWiFi();

  // Necessário para obter a hora correta
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
}

void loop() {
  // put your main code here, to run repeatedly:
  // lerSensorDistancia();
  // checkldr();
  buzzer();
  delay(20);
}

void lerSensorDistancia() {
  distanciaSensor = distanceSensor.measureDistanceCm();
  Serial.print("Distance(cm): ");
  Serial.println(distanciaSensor);
}

void lerPesoPote() {
  pesoPote = analogRead(LDR);
  Serial.print("LDR: ");
  Serial.println(pesoPote);
}

void encherPote() {
  tone(BUZZER, buzzerFrequency);
}

void pararEncherPote() {
  tone(BUZZER, 0);
}

void maquina_estados() {
  switch (estado) {
    case INIT:
      esperaConfig();
      if(configRecebida) {
        estado = IDLE;
      }
      break;
    case IDLE:
      if(deuHorarioDeComer()) {
        estado = ENCHENDO_POTE;
        encherPote();
      }
      break;
    case ENCHENDO_POTE:
      lerPesoPote();
      if(pesoPote >= PESO_CONFIGURACAO_MAXIMO) {
        pararEncherPote();
        estado = ESPERANDO_ANIMAL_PARA_COMER;
        // TODO: Pegar essa hora corretamente
        // Salva a hora que encheu o pote
        horaEncheuPote = millis();
      }
      break;
    case ESPERANDO_ANIMAL_PARA_COMER:
      lerSensorDistancia();
      if(distanciaSensor <= MAX_DISTANCE_CONFIG) {
        estado = ANIMAL_COMENDO;
      }
      break;
    case ANIMAL_COMENDO:
      lerSensorDistancia();
      if(distanciaSensor > MAX_DISTANCE_CONFIG) {
        lerPesoPote();
        if(pesoPote <= PESO_CONFIGURACAO_MINIMO){
          estado = COMEU_TUDO;
        } else {
          estado = ESPERANDO_ANIMAL_PARA_COMER;
        }
      }
      break;
    case COMEU_TUDO:
      if(deuHorarioDeComer()) {
        estado = ENCHENDO_POTE;
        encherPote();
      }
      break;
    default:
      break;
  }
}

void esperaConfig(){
  // Recebe uma configuração a ser utilizada da fog
  configRecebida = true;
}

// Retorna true se deu horário de comer
boolean deuHorarioDeComer() {
  timeClient.update();
  return true;
}

void leitura_data_e_hora() {
  //aguarda atualização
  timeClient.update();
  if (!timeClient.update()) {
    timeClient.forceUpdate();
  }
    
  formattedDate = timeClient.getFormattedTime();
  //Separa data e imprime
  splitT = formattedDate.indexOf("T");

  //Separa hora e imprime
  timeStamp = formattedDate.substring(splitT + 1, formattedDate.length());
  //Serial.println(timeStamp);
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
}

void iniciarMQTTeWiFi(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, password);

  // Wait until the connection is confirmed
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging – Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  if (pubSubClient.connect(clientID, usernameMQTT, passwordMQTT)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed…");
  }
}

void publishMQTT(String topic, String message){
  if (pubSubClient.connected()) {
    pubSubClient.publish(topic.c_str(), message.c_str());
  }
  else {
    Serial.println("MQTT Disconnected");
  }
}