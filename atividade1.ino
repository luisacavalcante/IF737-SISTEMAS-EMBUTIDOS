// C++ code
//
#define SEV_SEG_DISPLAY_A 2
#define SEV_SEG_DISPLAY_B 3
#define SEV_SEG_DISPLAY_C 4
#define SEV_SEG_DISPLAY_D 5
#define SEV_SEG_DISPLAY_E 6
#define SEV_SEG_DISPLAY_F 7
#define SEV_SEG_DISPLAY_G 8
#define SEV_SEG_DISPLAY_DP 9


#define LED_PIN 12
#define ACTIVATE_BUTTON_PIN 11
#define LDR_SENSOR_PIN A0
#define MOVEMENT_SENSOR_PIN 10

#define ALARM_DEACTIVATED 0
#define ALARM_ACTIVATING 1
#define ALARM_ACTIVATED 2
#define ALARM_DETECTED_MOVEMENT 3
#define TRY_PASSWORD_AGAIN 4
#define ALARM_TRIGGERED 5

#define BUZZER 13

#define LDR_PIN A0

#define LIMIAR_LDR_LIGHT 900

#define MAX_PASSWORD_LENGTH 8

String password = "12345678";
String typedPassword = "";
int posTypedPassword = 0;

int serialRead;
int newLine = 10;

int stateMachine = 0;

int activateAlarmButtonPressed = 0;
int buttonStatus;
int lastButtonState = LOW;
int sevSegNumber = 10;
int readingPassword = 0;

unsigned long sevSegDelay = 1000;
unsigned long lastMillisTime = 0;

boolean buzzerStatus = HIGH;
int beepTimes = 0;
unsigned long lastMillisBuzzer = 0;
unsigned long buzzerDelayInMs = 200;

unsigned long lastDebounceTime = 0;// the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void zero(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, LOW);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void one(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, LOW);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, LOW);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, LOW);
  digitalWrite(SEV_SEG_DISPLAY_G, LOW);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void two(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, LOW);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_F, LOW);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void three(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, LOW);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void four(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, LOW);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, LOW);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void five(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, LOW);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void six(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, LOW);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void seven(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, LOW);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, LOW);
  digitalWrite(SEV_SEG_DISPLAY_G, LOW);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void eight(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void nine(void) {
  digitalWrite(SEV_SEG_DISPLAY_A, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_B, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_C, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_D, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_G, HIGH);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void turnOffSevSeg() {
  digitalWrite(SEV_SEG_DISPLAY_A, LOW);
  digitalWrite(SEV_SEG_DISPLAY_B, LOW);
  digitalWrite(SEV_SEG_DISPLAY_C, LOW);
  digitalWrite(SEV_SEG_DISPLAY_D, LOW);
  digitalWrite(SEV_SEG_DISPLAY_E, LOW);
  digitalWrite(SEV_SEG_DISPLAY_F, LOW);
  digitalWrite(SEV_SEG_DISPLAY_G, LOW);
  digitalWrite(SEV_SEG_DISPLAY_DP, LOW);
}

void displaySevSegNumber(int number) {
  switch(number) {
    case 0:
      zero();
      break;
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
    default:
      zero();
      break;        
  }
}

int movementDetected(){
  int pirState = digitalRead(MOVEMENT_SENSOR_PIN);
  int ldrValue = analogRead(LDR_PIN);
  if(pirState == HIGH || ldrValue >= LIMIAR_LDR_LIGHT){
//    Serial.print("Pir state: ");
//    Serial.print(pirState);
//    Serial.print(" ldrValue: ");
//    Serial.print(ldrValue);
//    Serial.print("\n");
    return 1;
  }
  return 0;
}

int readPassword(){
  if(Serial.available() > 0){
    serialRead = Serial.read();
    if(serialRead == newLine) {
      return 1;
    }
    typedPassword += (char) serialRead;
    //Serial.print("Typed password: ");
    //Serial.println(typedPassword);
    posTypedPassword++;

  }
  
  return 0;
}

int correctPasswordInserted() {
  
  if (posTypedPassword > MAX_PASSWORD_LENGTH) return 0;
  
  for(int i = 0; i < MAX_PASSWORD_LENGTH; i++) {
    if(typedPassword[i] != password[i]){
      return 0;
    }
  }
  return 1;
}

int debounceButton() {

  int read_ = digitalRead(ACTIVATE_BUTTON_PIN);                       //A variável leitura recebe a leitura do pino do botão: HIGH (pressionado) ou LOW (Desacionado)

  if (read_ != lastButtonState) {                     //Se a leitura atual for diferente da leitura anterior
    lastDebounceTime = millis();                   //Reseta a variável tempoUltimoDebounce atribuindo o tempo atual para uma nova contagem
    //Serial.println("Debouncing");
  }

  if ((millis() - lastDebounceTime) > debounceDelay) { //Se o resultado de (tempo atual - tempoUltimoDebounce) for maior que o tempo que determinamos (tempoDebounce), ou seja, já passou os 50 milissegundos que é o tempo que o botão precisa ficar pressionado para ter a certeza de que ele realmente foi pressionado? Se sim faça:
    if (read_ != buttonStatus) {                         //Verifica se a leitura do botão mudou, ou seja, se é diferente do status que o botão tinha da última vez. Se sim, faça:
      buttonStatus = read_;                              //statusBotao recebe o que foi lido na variável leitura (pressionado = 1 e não pressionado = 0)
      if (buttonStatus == HIGH) {                          //Se o statusBotao é igual a HIGH significa que o botão foi pressionado, então faça:
        return 1;                                       //Incrementa +1 na variável contador. (contador++ é o mesmo que: contador = contador +1)
      }
    }
  }
  lastButtonState = read_; 
  return 0;

}

void blinkLed(){
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

void beep(int times) {
  if(millis() - lastMillisBuzzer > buzzerDelayInMs){
     lastMillisBuzzer = millis();
     if(beepTimes < times * 2) {
      beepTimes++;
      digitalWrite(BUZZER, buzzerStatus);
      if(buzzerStatus) {
        buzzerStatus = LOW;  
      } else {
        buzzerStatus = HIGH;
      }
     }
  }
  
}

void resetVariables(){
  turnOffSevSeg();
  typedPassword = "";
  posTypedPassword = 0;
  sevSegNumber = 10;
  beepTimes = 0;
  buzzerStatus = HIGH;
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ACTIVATE_BUTTON_PIN, INPUT);
  pinMode(MOVEMENT_SENSOR_PIN, INPUT);
  
  pinMode(SEV_SEG_DISPLAY_A, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_B, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_C, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_D, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_E, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_F, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_G, OUTPUT);
  pinMode(SEV_SEG_DISPLAY_DP, OUTPUT);
  
  lastButtonState = digitalRead(ACTIVATE_BUTTON_PIN);
  
  Serial.begin(9600);
  //Serial.println(password);
  Serial.println("System started");
}

void loop() {
  //Debounce button
    
  //int correctPasswordInserted = 0;
  int timerHasExpired = 0;
  int passwordTyped_ = 0;
  
  switch(stateMachine) {
    case ALARM_DEACTIVATED:
      digitalWrite(LED_PIN, LOW);
        if(activateAlarmButtonPressed > 0){
          stateMachine = ALARM_ACTIVATING;
          activateAlarmButtonPressed = 0;
          //delay(2000); // Simulando o tempo para fechar uma porta, por exemplo
        } else {
          activateAlarmButtonPressed = debounceButton();
          resetVariables();
        }
      break;
    case ALARM_ACTIVATING:
      Serial.println("Activating alarm");
      for(int i = 0; i < 5; i++){
        blinkLed();
      }
      stateMachine = ALARM_ACTIVATED;
      Serial.println("Alarm activated!");
      break;
    case ALARM_ACTIVATED:
        if(movementDetected()){
          stateMachine = ALARM_DETECTED_MOVEMENT;
          Serial.println("Movement detected!");
          lastMillisTime = millis();
        }
      break;
    case ALARM_DETECTED_MOVEMENT:
    
      if(sevSegNumber == 0){
        timerHasExpired = 1;
      }

      beep(2);
      
      passwordTyped_ = readPassword();
      displaySevSegNumber(sevSegNumber);
    
        if(!passwordTyped_ && !timerHasExpired) {
          
          if(millis() - lastMillisTime >= sevSegDelay) {
            sevSegNumber--;
            lastMillisTime = millis();
          }
        } else {
          if(correctPasswordInserted()) {
            stateMachine = ALARM_DEACTIVATED;
            Serial.println("Alarm deactivated :)");
          } else if(timerHasExpired) {
            stateMachine = ALARM_TRIGGERED;
            Serial.println("Timer has expired!");
          } else {
            stateMachine = TRY_PASSWORD_AGAIN;
            Serial.println("Try password again");
            sevSegNumber = 10;
            typedPassword = "";
            passwordTyped_ = 0;
            posTypedPassword = 0;
          }
        }
      break;
    case TRY_PASSWORD_AGAIN:
        if(sevSegNumber == 0){
          timerHasExpired = 1;
        }
      
      passwordTyped_ = readPassword();
      displaySevSegNumber(sevSegNumber % 10);
    
        if(!passwordTyped_ && !timerHasExpired) {
          if(millis() - lastMillisTime >= sevSegDelay) {
            sevSegNumber--;
            lastMillisTime = millis();
          }
        } else {
          if(correctPasswordInserted()) {
            stateMachine = ALARM_DEACTIVATED;
            Serial.println("Alarm deactivated :)");
          } else {
            stateMachine = ALARM_TRIGGERED;
            Serial.println("!!!Alarm TRIGGERED!!! D:");
            passwordTyped_ = 0;
            readingPassword = 0;
            posTypedPassword = 0;
          } 
        }
      break;
   case ALARM_TRIGGERED:
      turnOffSevSeg();
      Serial.println("Alarm triggered!");
      digitalWrite(BUZZER, HIGH);
      blinkLed();
      break;
  }  
}
