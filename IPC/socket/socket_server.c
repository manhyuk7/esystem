import socket

HOST = '192.168.45.200'  # 서버 주소 (localhost)
PORT = 65432        # 포트 번호

# 소켓 생성
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen()

print(f"서버가 {HOST}:{PORT}에서 대기 중...")

conn, addr = server_socket.accept()
print(f"클라이언트 {addr} 연결됨.")

while True:
    data = conn.recv(1024)
    if not data:
        break
    print(f"클라이언트로부터 받은 메시지: {data.decode()}")
    conn.sendall(data)            # 받은 데이터를 다시 클라이언트에게 전송

conn.close()
server_socket.close()