import RPi.GPIO as GPIO
import time
from time import sleep
import datetime
import threading
import socket
import sys
import json
import random

LED_VERDE = 7
LED_AMARELO = 8
LED_VERMELHO = 10
PIR = 12

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_VERDE, GPIO.OUT)
GPIO.output(LED_VERDE, False)

GPIO.setup(LED_AMARELO, GPIO.OUT)
GPIO.output(LED_AMARELO, False)

GPIO.setup(LED_VERMELHO, GPIO.OUT)
GPIO.output(LED_VERMELHO, False)

GPIO.setup(PIR, GPIO.IN)


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
    13,
    15,
    16,
    18,
    22,
    26,
    24
]

for i in range(7):
    GPIO.setup(displayUnidade[i], GPIO.OUT)
    GPIO.output(displayUnidade[i], True)
    
def off(display):
    for i in range(len(display)):
        GPIO.output(display[i], False)  

def zero7Seg(display):
    for i in range(len(display)):
        GPIO.output(display[i], numbers[0][i])

def showNumberDisplay(display, number):
    if(number < 0):
        zero7Seg(display)
    num = numbers[number % 10]
    for i in range(len(display)):
        GPIO.output(display[i], num[i])

def showNumber7Seg(number):
    if(number > 100):
        number = 99
    showNumberDisplay(displayUnidade, int(number) % 10)
    
    
def readPIR():
    return GPIO.input(PIR)
        
zero7Seg(displayUnidade)

mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

cmd = {
    "header": None,
    "body": {}
}


log_config = {
    "usuario": "",
    "sinal_verde": -1,
    "sinal_amarelo": -1,
    "sinal_vermelho": -1,
    "config": False,
    "horario_config": -1,
    "led_on": -1
}

log_cars = {
    0: {
        "qtd": 0,
    },
    1: {
        "qtd": 0,
    },
    2: {
        "qtd": 0,
    }
}


# def descarga():
#     GPIO.setup(LDR, GPIO.OUT)
#     GPIO.output(LDR, GPIO.LOW)
#     time.sleep(0.15)
    
# def carga():
#     GPIO.setup(LDR, GPIO.IN)
#     contador = 0
#     while(GPIO.input(LDR) == GPIO.LOW):
#         contador = contador + 1
#     time.sleep(1)
#     return contador

# def leitura_analogica():
#     descarga()
#     return carga()



def change_leds():

    t0 = 0
    time_led_verde = 1 #1 segundo
    contador_led_verde = 0
    last_pir_read = 0
    t1 = 0
    while True:
        # simulando um carro passando pelo sensor
        #number = random.randint(0, 10000000)
        pir_read = readPIR()
        
        if pir_read == 1 and last_pir_read != 1:
            print("car detected on led: ", log_config["led_on"])
            log_cars[log_config["led_on"]]["qtd"] += 1
        
        last_pir_read = pir_read

        if log_config["led_on"] == 0:
            # verde
            GPIO.output(LED_VERDE, True)
            GPIO.output(LED_VERMELHO, False)
            GPIO.output(LED_AMARELO, False)
            time_on = log_config["sinal_verde"]
            
        elif log_config["led_on"] == 1:
            # amarelo
            GPIO.output(LED_VERDE, False)
            GPIO.output(LED_VERMELHO, False)
            GPIO.output(LED_AMARELO, True)
            time_on = log_config["sinal_amarelo"]
            
        elif log_config["led_on"] == 2:
            # vermelho
            GPIO.output(LED_VERDE, False)
            GPIO.output(LED_VERMELHO, True)
            GPIO.output(LED_AMARELO, False)
            time_on = log_config["sinal_vermelho"]
            
        else:
            print("ERROR LED ON")
                    
        if time.time() - t0 >= time_on:
            if log_config["led_on"] == 0:
                log_config["led_on"] = 1
                t0 = time.time()
                print("verde -> amarelo")
            elif log_config["led_on"] == 1:
                log_config["led_on"] = 2
                t0 = time.time()
                print("amarelo -> vermelho")
            elif log_config["led_on"] == 2:
                log_config["led_on"] = 0
                t0 = time.time()
                print("vermelho -> verde")
                contador_led_verde = log_config["sinal_verde"]
                
        if time.time() - t1 >= time_led_verde:
            if log_config["led_on"] == 0:
                showNumber7Seg(contador_led_verde)
                contador_led_verde = contador_led_verde - 1
            t1 = time.time()

def main():

    print("Server started")

    try:
        mysock.bind(('127.0.0.1', 8081))
    except Exception as e:
        mysock.close()
        raise e

    mysock.listen()

    thread = threading.Thread(target=change_leds)

    time_on = -1

    run_thread = False

    recc = False
    
    leds = threading.Thread(target=change_leds)

    while True:
        
        conn, addr = mysock.accept()
        data = conn.recv(1000)
        print("receba")
        
        if data:
            data_msg = str(data.decode("utf-8"))

            print(json.loads(data_msg))

            cmd = json.loads(data_msg)

            if cmd["header"] == "C":
                print("config success")

                log_config["usuario"] = cmd["body"]["usuario"]
                log_config["sinal_verde"] = int(cmd["body"]["sinal_verde"])
                log_config["sinal_amarelo"] = int(cmd["body"]["sinal_amarelo"])
                log_config["sinal_vermelho"] = int(cmd["body"]["sinal_vermelho"])
                log_config["config"] = True
                log_config["led_on"] = 0
                log_config["horario_config"] = datetime.datetime.now()

                print(log_config)
                

            elif cmd["header"] == "L":
                if log_config["config"]:
                    print("config found")

                    ret = {
                        "header": "L",
                        "body": {
                            "usuario": log_config["usuario"],
                            "sinal_verde": log_config["sinal_verde"],
                            "sinal_amarelo": log_config["sinal_amarelo"],
                            "sinal_vermelho": log_config["sinal_vermelho"],
                            "horario_config": str(log_config["horario_config"])
                        }
                    }

                    conn.sendall(json.dumps(ret).encode("utf-8"))
                else:
                    print("config not found")
                    error = {
                        "header": "E",
                        "body": {
                            "message": "config not found"
                        }
                    }
                    conn.sendall(json.dumps(error).encode("utf-8"))
            elif cmd["header"] == "R":
                if log_config["config"]:
                    print("config found")
                    ret = {
                        "header": "R",
                        "body": {
                            "carros_verde": log_cars[0]["qtd"],
                            "carros_amarelo": log_cars[1]["qtd"],
                            "carros_vermelho": log_cars[2]["qtd"]
                        }
                    }
                    conn.sendall(json.dumps(ret).encode("utf-8"))
                else:
                    print("config not found")
                    error = {
                        "header": "E",
                        "body": {
                            "message": "config not found"
                        }
                    }
                    conn.sendall(json.dumps(error).encode("utf-8"))

            print()
        if log_config["config"] and not run_thread:
            run_thread = True
            thread.start()

        data = None
    mysock.close()


if __name__ == "__main__":
    main()
      
