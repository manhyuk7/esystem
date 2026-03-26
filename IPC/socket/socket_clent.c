import socket

HOST = '192.168.45.200'  # 서버 주소 (localhost)
PORT = 65432        # 포트 번호

# 소켓 생성
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((HOST, PORT))

message = "Hello, Server!"
print(f"서버에 보낸 메시지 %s” %(message))
client_socket.sendall(message.encode())

data = client_socket.recv(1024)
print(f"서버로부터 받은 메시지: {data.decode()}")

client_socket.close()