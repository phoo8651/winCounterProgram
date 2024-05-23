import socket
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import hashes
import base64

def load_private_key(private_key_file):
    with open(private_key_file, "rb") as key_file:
        private_key = serialization.load_pem_private_key(
            key_file.read(),
            password=None,
        )
    return private_key

def decrypt_message(private_key, encrypted_message_base64):
    try:
        encrypted_message_bytes = base64.b64decode(encrypted_message_base64)

        decrypted_message = private_key.decrypt(
            encrypted_message_bytes,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA1()),
                algorithm=hashes.SHA1(),
                label=None
            )
        )
        return decrypted_message.decode('utf-8')
    except Exception as e:
        print("복호화 중 오류 발생:", e)
        return None

private_key_file = "private_key.pem"
private_key = load_private_key(private_key_file)

host = '0.0.0.0'
port = 5078

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(5)

print(f"서버가 {port}번 포트에서 수신 대기 중입니다...")

try:
    while True:
        client_socket, addr = server_socket.accept()
        print(f"연결 수립: {addr}")

        try:
            while True:
                data = client_socket.recv(4096)
                if not data:
                    break

                encrypted_message_base64 = data.decode('utf-8').strip()  # 수신 데이터 디코딩 및 공백 제거
                print(f"수신된 Base64 인코딩 메시지: {encrypted_message_base64}")

                decrypted_message = decrypt_message(private_key, encrypted_message_base64)
                print(f"복호화된 메시지: {decrypted_message}")
        finally:
            client_socket.close()
            print(f"연결 종료: {addr}")

except KeyboardInterrupt:
    print("서버를 종료합니다...")

server_socket.close()
