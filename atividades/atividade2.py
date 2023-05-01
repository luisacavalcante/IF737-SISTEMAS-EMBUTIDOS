import RPi.GPIO as GPIO
import time
from time import sleep
import threading

GPIO.setmode(GPIO.BOARD)

numbers = [
    [True,  True,  True,  True,  True,  True,  False], #0
    [False, True,  True,  False, False, False, False], #1
    [True,  True,  False, True,  True,  False, True ], #2
    [True,  True,  True,  True,  False, False, True ], #3
    [False, True,  True,  False, False, True,  True ], #4
    [True,  False, True,  True,  False, True,  True ], #5
    [True,  False, True,  True,  True,  True,  True ], #6
    [True,  True,  True,  False, False, False, False], #7
    [True,  True,  True,  True,  True,  True,  True ], #8
    [True,  True,  True,  True,  False, True,  True ], #9
]

displayUnidade = [
    11,
    13,
    15,
    16,
    18,
    21,
    19
]

displayDezena = [
    22,
    23,
    24,
    26,
    29,
    32,
    31
]

for i in range(7):
    GPIO.setup(displayUnidade[i], GPIO.OUT)
    GPIO.setup(displayDezena[i], GPIO.OUT)
    GPIO.output(displayDezena[i], True)
    GPIO.output(displayUnidade[i], True)

LED_VERDE = 10
LED_VERMELHO = 8
LED_AMARELO = 12
BUZZER = 33

MAXIMUM_DISTANCE = 100 # distância máxima
MEDIUM_DISTANCE = 30 #entre 100 e 30
LOW_DISTANCE = 10 #mínima distância antes da colisão

#verde
GPIO.setup(LED_VERDE, GPIO.OUT)

#amarelo
GPIO.setup(LED_AMARELO, GPIO.OUT)

#vermelho
GPIO.setup(LED_VERMELHO, GPIO.OUT)

#buzzer
GPIO.setup(BUZZER, GPIO.OUT)
GPIO.output(BUZZER, False)
global BuzzerPWM
BuzzerPWM = GPIO.PWM(BUZZER, 440)

#sensor ultrassonico
GPIO_TRIGGER = 40
GPIO_ECHO = 38
GPIO.setup(GPIO_TRIGGER, GPIO.OUT)
GPIO.setup(GPIO_ECHO, GPIO.IN)

GPIO.output(LED_VERDE, False)
GPIO.output(LED_VERMELHO, False)
GPIO.output(LED_AMARELO, False)

checa_distancia = 0
global lastBuzzerState

def off(display):
    for i in range(len(display)):
        GPIO.output(display[i], False)  

def zero(display):
    for i in range(len(display)):
        GPIO.output(display[i], numbers[0][i])

def showNumberDisplay(display, number):
    if(number < 0):
        zero(display)
    num = numbers[number % 10]
    for i in range(len(display)):
        GPIO.output(display[i], num[i])

def showNumber7Seg(number):
    if(number > 100):
        number = 99
    showNumberDisplay(displayDezena, int(number/10) % 10)
    showNumberDisplay(displayUnidade, int(number) % 10)

def getDistance():
    GPIO.output(GPIO_TRIGGER, True)
    # set Trigger after 0.01ms to LOW
    time.sleep(0.00001)
    GPIO.output(GPIO_TRIGGER, False)
    StartTime = time.time()
    StopTime = time.time()

    while GPIO.input(GPIO_ECHO) == 0:
        StartTime = time.time()

    while GPIO.input(GPIO_ECHO) == 1:
        StopTime = time.time()

    TimeElapsed = StopTime - StartTime
    # multiply with the sonic speed (34300 cm/s)
    distance = (TimeElapsed * 34300) / 2

    return distance

def ler_distancia():
    filterArray = []
    for sample in range(20) :
        filterArray.append(getDistance());
        time.sleep(0.03)
    
    sorted(filterArray)

    sum = 0;
    for sample in range(5, 15):
        sum += filterArray[sample];

    return sum / 10;

def verde():
    GPIO.output(LED_VERDE, True)
    GPIO.output(LED_VERMELHO, False)
    GPIO.output(LED_AMARELO, False)
    
def amarelo():
    GPIO.output(LED_VERDE, False)
    GPIO.output(LED_VERMELHO, False)
    GPIO.output(LED_AMARELO, True)
    
def vermelho():
    GPIO.output(LED_VERDE, False)
    GPIO.output(LED_VERMELHO, True)
    GPIO.output(LED_AMARELO, False)

def millis():
    return round(time.time() * 1000)

global lastBuzzerTime
lastBuzzerTime = millis()
lastBuzzerState = False

def beep(distance, lastBuzzerState, lastBuzzerTime):
    beepTime = 0
    beepShort = False
    if(distance > MAXIMUM_DISTANCE):
        if(lastBuzzerState):
            beepTime = 500
        else:
            beepTime = 2000
    elif(distance > MEDIUM_DISTANCE):
        beepTime = 1000
        beepShort = False
    else:
        beepTime = 500
        beepShort = False
        
    if(millis() - lastBuzzerTime > beepTime):
        BuzzerPWM.ChangeFrequency(beepTime)
        lastBuzzerTime = millis()
        if(lastBuzzerState):
            BuzzerPWM.stop()
        else:
            BuzzerPWM.start(50)
        lastBuzzerState = not lastBuzzerState
    
    return lastBuzzerState, lastBuzzerTime

def colisao():
    BuzzerPWM.stop()
    BuzzerPWM.start(50)
    state = True
    while True:
        GPIO.output(LED_VERDE, state)
        GPIO.output(LED_VERMELHO, state)
        GPIO.output(LED_AMARELO, state)
        sleep(1)
        state = not state     

lastTime = millis()
off(displayUnidade)
off(displayDezena)

while True:
    #checa_distancia = ler_distancia()
    checa_distancia = getDistance()
    #print(checa_distancia)
    lastBuzzerState, lastBuzzerTime = beep(checa_distancia, lastBuzzerState, lastBuzzerTime)

    if(millis() - lastTime > 100):
        showNumber7Seg(checa_distancia)
        if(checa_distancia > MAXIMUM_DISTANCE):
            verde()
        elif(checa_distancia > MEDIUM_DISTANCE):
            amarelo()
        elif(checa_distancia > LOW_DISTANCE):
            vermelho()
        else:
            colisao()
        lastTime = millis()
        
GPIO.cleanup()
