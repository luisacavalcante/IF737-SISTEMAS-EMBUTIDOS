import RPi.GPIO as GPIO
import time
from time import sleep
import threading
import socket
import sys
import json

LED_VERDE = 8
LED_AMARELO = 8
LED_VERMELHO = 8
LDR = 12

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_VERDE, GPIO.OUT)
# GPIO.output(LED_VERDE, False)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_AMARELO, GPIO.OUT)
# GPIO.output(LED_AMARELO, False)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_VERMELHO, GPIO.OUT)
# GPIO.output(LED_VERMELHO, False)

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
    "led_on": -1
}

def descarga():
    GPIO.setup(LDR, GPIO.OUT)
    GPIO.output(LDR, GPIO.LOW)
    time.sleep(0.15)
    
def carga():
    GPIO.setup(LDR, GPIO.IN)
    contador = 0
    while(GPIO.input(LDR) == GPIO.LOW):
        contador = contador + 1
    time.sleep(1)
    return contador

def leitura_analogica():
    descarga()
    return carga()


def change_leds():
    if log_config["led_on"] == 0:
        # verde
        # GPIO.output(LED_VERDE, True)
        # GPIO.output(LED_VERMELHO, False)
        # GPIO.output(LED_AMARELO, False)
        print("verde")
        time_on = log_config["sinal_verde"]
        
    elif log_config["led_on"] == 1:
        # amarelo
        # GPIO.output(LED_VERDE, False)
        # GPIO.output(LED_VERMELHO, False)
        # GPIO.output(LED_AMARELO, True)
        print("amarelo")
        time_on = log_config["sinal_amarelo"]
        
    elif log_config["led_on"] == 2:
        # vermelho
        # GPIO.output(LED_VERDE, False)
        # GPIO.output(LED_VERMELHO, True)
        # GPIO.output(LED_AMARELO, False)
        print("vermelho")
        time_on = log_config["sinal_vermelho"]
        
    else:
        print("ERROR LED ON")
    
    
        
    if time.time() - t0 >= time_on:
        if log_config["led_on"] == 0:
            log_config["led_on"] = 1
            t0 = time.time()
            print()
        elif log_config["led_on"] == 1:
            log_config["led_on"] = 2
            t0 = time.time()
            print()
        elif log_config["led_on"] == 2:
            log_config["led_on"] = 0
            t0 = time.time()
            print()

def main():
    try:
        mysock.bind(('', 8080))
    except Exception as e:
        mysock.close()
        raise e

    mysock.listen()


    t0 = 0
    time_on = -1

    recc = False
    
    leds = threading.Thread(target=change_leds)

    while True:
        if not log_config["config"]:
            conn, addr = mysock.accept()
            data = conn.recv(1000)
        
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
                            "sinal_vermelho": log_config["sinal_vermelho"]
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
                    pass
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
        
        if log_config["config"]:
            # thread
            change_leds()
        data = None
    mysock.close()


if __name__ == "__main__":
    main()


