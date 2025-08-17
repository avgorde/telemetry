import socket

HOST = "127.0.0.1"  # localhost
PORT = 8080         # same port as server

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    while True:
        data = s.recv(1024)
        if not data:
            break
        print(data.decode(), end="")
