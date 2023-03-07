import time
from time import sleep
import threading
import socket
import sys
import json


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
    "config": False
}


try:
    mysock.bind(('', 8081))
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

    print(json.loads(data_msg))

    cmd = json.loads(data_msg)

    if cmd["header"] == "C":
        print("config success")

        log_config["usuario"] = cmd["body"]["usuario"]
        log_config["sinal_verde"] = cmd["body"]["sinal_verde"]
        log_config["sinal_amarelo"] = cmd["body"]["sinal_amarelo"]
        log_config["sinal_vermelho"] = cmd["body"]["sinal_vermelho"]
        log_config["config"] = True

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

mysock.close()
