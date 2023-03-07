

import RPi.GPIO as GPIO
import time
from time import sleep
import threading
import socket
import sys

message = ""
inp = input()
while True:
    
    mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    mysock.connect(('', 8080))
    message += inp + "\n"
    
    print("mensagem: ", message)
    
    mysock.sendall(bytes(message, "utf-8"))
    
    data = mysock.recv(1000)
    
    if data:
        data_msg = str(data.decode("utf-8"))
        print(data_msg)

    
    message = ""
    inp = input()

    mysock.close()

