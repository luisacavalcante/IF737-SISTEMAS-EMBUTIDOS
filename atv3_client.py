
# import RPi.GPIO as GPIO
import time
from time import sleep
import threading
import socket
import sys
import json
message = ""

config = {
    "body":{
        "usuario":"",
        "sinal_verde": -1,
        "sinal_vermelho": -1,
        "sinal_amarelo": -1,
    },
    "header":"R"
       
}
while True:
    
    mysock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    
    mysock.connect(('127.0.0.1', 8080))
    print("Digite C (config), Digite L (log)  ou R (registros): ")
    inp = input()
    if(inp == "C"):
        config["header"] ="C"
        print("Insira o nome do usuario:")
        inp = input()
        message_usuario = inp 
        config["body"]["usuario"] = message_usuario
        print("Tempo led verde:")
        inp = input()
        message_tmp_verde = inp 
        config["body"]["sinal_verde"] = message_tmp_verde
        print("Tempo led vermelho:")
        inp = input()
        message_tmp_vermelho = inp 
        config["body"]["sinal_vermelho"] = message_tmp_vermelho
        print("Tempo led amarelo:")
        inp = input()
        message_tmp_amarelo = inp 
        config["body"]["sinal_amarelo"] = message_tmp_amarelo

        print(config)
        stringJ= json.dumps(config)
        mysock.sendall(bytes(stringJ, "utf-8"))
        mysock.close()


    elif(inp == "L"):
        config["header"] ="L"
        stringJ= json.dumps(config)
        mysock.sendall(bytes(stringJ, "utf-8"))
        data = mysock.recv(1000)

    
        if data:
            data_msg = str(data.decode("utf-8"))
            objetoJ= json.loads(data_msg)
            print(objetoJ)
    elif(inp == "R"):
        config["header"] ="R"
        stringJ= json.dumps(config)
        mysock.sendall(bytes(stringJ, "utf-8"))
        data = mysock.recv(1000)

        if data:
            data_msg = str(data.decode("utf-8"))
            objetoJ= json.loads(data_msg)
            print(objetoJ)
    else:
        pass
    

    print()
