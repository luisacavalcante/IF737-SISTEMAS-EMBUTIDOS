#include <HCSR04.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#define ECHO_PIN 18
#define TRIGGER_PIN 19
#define LDR 34
#define BUZZER 33

UltraSonicDistanceSensor distanceSensor(TRIGGER_PIN, ECHO_PIN);

int dia = 0;
int hora = 0;
int minuto = 0;
int segundo = 0;
int hora_comida = 9;
int minuto_comida = 36;
int segundo_comida = 0;
String formattedDate;
bool ja_comeu = false;

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
//const char* ssid = "APT 103_OI FIBRA";
//const char* password = "jajejo22";
// const char* ssid = "Kilner's House";
// const char* password = "kilner131813";
 const char* ssid = "CINGUESTS";
 const char* password = "acessocin";
//const char *ssid     = "LIVE TIM_1901_2G";
//const char *password = "danivalberlu";
/**
 * @brief Configurações do MQTT
 * 
 */
const char* serverMQTT = "172.22.67.158"; 
const char* topicoPublish = "pet";
const char* topicoSubscribe = "config";
const char* usernameMQTT = "teste"; // MQTT username
const char* passwordMQTT = "teste"; // MQTT password
const char* clientID = "IOTreat"; // MQTT client ID

WiFiClient wiFiClient;

PubSubClient pubSubClient(serverMQTT, 1884, wiFiClient);
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
String horario_comida = "11:32:30";
unsigned int PESO_CONFIGURACAO_MAXIMO = 2500;
unsigned int PESO_CONFIGURACAO_MINIMO = 1800;
unsigned int pesoPote = 0;

int MAX_DISTANCE_CONFIG = 10;
float distanciaSensor = 0;
int maxLdrValue = 2000;
int buzzerFrequency = 2000;

int splitT = 0;

void lerSensorDistancia() {
  distanciaSensor = distanceSensor.measureDistanceCm();
 // Serial.print("Distance(cm): ");
 // Serial.println(distanciaSensor);
}

void lerPesoPote() {
  pesoPote = analogRead(LDR);
 //Serial.print("Peso do pote de comida: ");
  //Serial.println(pesoPote);
}

void encherPote() {
  tone(BUZZER, buzzerFrequency);
  delay(5000);
}

void pararEncherPote() {
  Serial.println("O pote está cheio");
  tone(BUZZER, 0);
}

void maquina_estados() {
  switch (estado) {
    case INIT:
       
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
        Serial.println("Esperando animal para comer");
        // TODO: Pegar essa hora corretamente
        // Salva a hora que encheu o pote
        horaEncheuPote = millis();
      }
      break;
    case ESPERANDO_ANIMAL_PARA_COMER:
    
      lerSensorDistancia();
      if(distanciaSensor <= MAX_DISTANCE_CONFIG && distanciaSensor>0) {
        estado = ANIMAL_COMENDO;
        Serial.println("O animal está comendo");
      }
      break;
    case ANIMAL_COMENDO:
      lerSensorDistancia();
      //Serial.println("distanciaSensor "+String(distanciaSensor));
      if(distanciaSensor > MAX_DISTANCE_CONFIG) {
        lerPesoPote();
        Serial.println("pesoPote "+String(pesoPote));
        if(pesoPote <= PESO_CONFIGURACAO_MINIMO){
          
          estado = COMEU_TUDO;
          ja_comeu = true;
          Serial.println("O animal comeu tudo");
        } else {
          estado = ESPERANDO_ANIMAL_PARA_COMER;
          Serial.println("O animal veio no pote mas não comeu tudo ainda");
        }
      }
      break;
    case COMEU_TUDO:
      if(deuHorarioDeComer() && ja_comeu==false) {
        estado = ENCHENDO_POTE;
        encherPote();
      }
      break;
    default:
      break;
  }
}


boolean deuHorarioDeComer() {
 leitura_data_e_hora();
  if(hora>hora_comida){
    if(ja_comeu==false){
      Serial.println("Deu a hora de comer");

    }
    return true;
  }else{
      if(hora==hora_comida){
          if(minuto>minuto_comida){
            return true;
          }else{
            if(minuto==minuto_comida){
              if(segundo>=segundo_comida){
                
                return true;
              } else{
                return false;
              }
            }else{
              return false;
            }
          }
      }else{
        return false;
      }
    

  }
 
  

}

void leitura_data_e_hora() {
  //aguarda atualização
  timeClient.update();
  if (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedTime();
  dia = timeClient.getDay();
  hora = timeClient.getHours();
  minuto = timeClient.getMinutes();
  segundo =timeClient.getSeconds();
  if(hora == 0){
    ja_comeu = false;
  }
}



void iniciaMQTT(){
  // Connect to MQTT Broker
  if (pubSubClient.connect(clientID, usernameMQTT, passwordMQTT)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed…");
  }
}

void subscribeMQTT(String topic){
  if (pubSubClient.connected()) {
    pubSubClient.subscribe(topic.c_str());
    Serial.println("Subscribe successful");
  } else {
    Serial.println("MQTT Disconnected");
  }
}

void publishMQTT(String topic, String message){
  Serial.println("Entrei no publishMQTT");
  if (pubSubClient.connected()) {
    Serial.println("Conexão funcionando");
    pubSubClient.publish(topic.c_str(), message.c_str());
  }
  else {
    Serial.println("MQTT Disconnected");
  }
}

void reconnectMQTT() {
  while (!pubSubClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   if (pubSubClient.connect(clientID, usernameMQTT, passwordMQTT)) {
      Serial.println("connected");
      pubSubClient.loop();
      // Subscribe
      pubSubClient.subscribe(topicoSubscribe);
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubSubClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;


  for (int i = 0; i < length; i++) {
   // Serial.print((char)message[i]);
    messageTemp += (char) payload[i];
  }

  
  Serial.println(messageTemp);

  if(String(topic) == "config"){
    // configRecebida = true;
    //DynamicJsonDocument doc(1024);
    //DeseralizeJson(doc,messageTemp);
    Serial.println("Entrei no topic==config");
    // configRecebida = true;
    
    MAX_DISTANCE_CONFIG = (int) doc["max_distance"];
    PESO_CONFIGURACAO_MAXIMO = (int) doc["peso_max"];
    PESO_CONFIGURACAO_MINIMO = (int) doc["peso_min"];
    
    Serial.print("Distancia maxima: "); Serial.println(MAX_DISTANCE_CONFIG);
  
    Serial.print("Peso maximo: "); Serial.println(PESO_CONFIGURACAO_MAXIMO);

    Serial.print("Peso minimo: "); Serial.println(PESO_CONFIGURACAO_MINIMO);
  }
}

void sendData(int tipo){
  DynamicJsonDocument doc(1024); 
  doc["tipo"] = tipo;
  doc["peso"] = pesoPote;

  char out[256];
  int b = serializeJson(doc, out);
  Serial.print("bytes = ");
  Serial.println(b, DEC);
  Serial.print("Enviando - tipo: "); Serial.print(tipo); Serial.print(" peso "); Serial.print(pesoPote); Serial.print(" hora "); Serial.println(formattedDate);

  pubSubClient.publish("pet", out);
}


void conecta_internet() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Imprime o IP local
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Inicializa o NTPClient para captura de horário
  timeClient.begin();
  //Selecione o offset em múltiplos de 3600 (Brasília GMT-3), portanto -3x3600= -10800
  timeClient.setTimeOffset(-10800);

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  delay(50);
  Serial.println(" __  ______  ______  ______  ______  ______  ______  ");
  Serial.println("/\\ \\/\\  __ \\/\\__  _\\/\\  == \\/\\  ___\\/\\  __ \\/\\__  _\\ ");
  Serial.println("\\ \\ \\ \\ \\/\\ \\/_/\\ \\/\\ \\  __<\\ \\  __\\\\ \\  __ \\/_/\\ \\/ ");
  Serial.println(" \\ \\_\\ \\_____\\ \\ \\_\\ \\ \\_\\ \\_\\ \\_____\\ \\_\\ \\_\\ \\ \\_\\ ");
  Serial.println("  \\/_/\\/_____/  \\/_/  \\/_/ /_/\\/_____/\\/_/\\/_/  \\/_/ ");
  Serial.println();
  Serial.println("Projeto Sistemas Embutidos");

  conecta_internet();

  // Connect to MQTT

  iniciaMQTT();

  subscribeMQTT(topicoSubscribe);

  pubSubClient.setCallback(callback);
}

void loop() {
  if (!pubSubClient.connected()) {
    reconnectMQTT();
  }
  pubSubClient.loop();

  // lerSensorDistancia();
  // lerPesoPote();
  // if(distanciaSensor == -1){
  //   //Serial.println("Erro no sensor de distancia");
  // }
  maquina_estados();
}
