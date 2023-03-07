import RPi.GPIO as GPIO
import time
from time import sleep
import threading
import socket
import sys

LED = 8
BUZZ = 10
LDR = 12

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED, GPIO.OUT)

GPIO.output(LED, False)

GPIO.setmode(GPIO.BOARD)
GPIO.setup(BUZZ, GPIO.OUT)


GPIO.setup(BUZZ, GPIO.OUT)
GPIO.output(BUZZ, False)
buzz = GPIO.PWM(BUZZ, 440)

mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

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



try:
    mysock.bind(('', 8080))
except Exception as e:
    mysock.close()
    raise e

mysock.listen()

while True:
    conn, addr = mysock.accept()
    
    print("begin")
    data = conn.recv(1000)
    print("end")
    
    if not data:
        break
    
    data_msg = str(data.decode("utf-8"))
    
    print(data_msg)
    
    message = "HTTP/1.0 200 OK\n\n"
    
    if data_msg == "LED ON\n":
        GPIO.output(LED, True)
        message += "LIGOU\n"
    elif data_msg == "LED OFF\n":
        GPIO.output(LED, False)
        message += "DESLIGOU\n"
    elif "BUZZER" in data_msg:
        data_msg_list = data_msg.split(" ")
        freq = int(data_msg_list[1])
        tmp = int(data_msg_list[2][0:-1])
        buzz.ChangeFrequency(freq)
        buzz.start(50)
        time.sleep(tmp)
        buzz.stop()
    elif data_msg =="LDR\n":
        print("LDR LEITURA")
        val = leitura_analogica()     
        message+= str(val)
    else:
        message += "ERRO\n"
        

    conn.sendall(bytes(message, "utf-8"))
    
    
    message = ""
    data = None

        
    
mysock.close()

