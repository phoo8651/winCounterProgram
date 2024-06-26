import tkinter as tk
from tkinter import ttk
import socket
import json
import threading
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import padding
from cryptography.hazmat.primitives import hashes
import base64
import select

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

class ServerMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Server Monitor")

        # 클라이언트 정보 출력 프레임
        self.output_frame = ttk.LabelFrame(root, text="Client Status")
        self.output_frame.grid(row=0, column=0, padx=10, pady=5, sticky=tk.W+tk.E)

        ttk.Label(self.output_frame, text="Client IP").grid(row=0, column=0, padx=5, pady=5)
        ttk.Label(self.output_frame, text="Server Name").grid(row=0, column=1, padx=5, pady=5)
        ttk.Label(self.output_frame, text="CPU Usage (%)").grid(row=0, column=2, padx=5, pady=5)
        ttk.Label(self.output_frame, text="Memory Usage (%)").grid(row=0, column=3, padx=5, pady=5)
        ttk.Label(self.output_frame, text="Disk Usage (%)").grid(row=0, column=4, padx=5, pady=5)

        self.client_info_labels = {}

        # 포트 번호 입력 위젯
        ttk.Label(root, text="Port Number:").grid(row=1, column=0, padx=10, pady=5, sticky=tk.W)
        self.port_entry = ttk.Entry(root)
        self.port_entry.insert(0, "12480")  # 기본 포트 설정
        self.port_entry.grid(row=1, column=0, padx=10, pady=5, sticky=tk.E)

        # Start 버튼
        self.start_button = ttk.Button(root, text="Start", command=self.start_server)
        self.start_button.grid(row=1, column=1, padx=10, pady=5)

        # Stop 버튼
        self.stop_button = ttk.Button(root, text="Stop", command=self.stop_server, state=tk.DISABLED)
        self.stop_button.grid(row=1, column=2, padx=10, pady=5)

        # 서버 소켓 초기화
        self.server_socket = None
        self.accept_thread = None
        self.clients = []

        # 개인 키 로드
        private_key_file = "private_key.pem"
        self.private_key = load_private_key(private_key_file)

    def start_server(self):
        # Close existing connections and clear client information
        self.clear_client_info()
        
        # Start server with new port number
        port_number = int(self.port_entry.get())
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.bind(("0.0.0.0", port_number))
        self.server_socket.listen(10)
        self.server_socket.setblocking(False)
        
        # Start accepting connections in a new thread
        self.accept_thread = threading.Thread(target=self.accept_connections)
        self.accept_thread.start()

        # Update button states
        self.start_button.config(state=tk.DISABLED)
        self.stop_button.config(state=tk.NORMAL)

    def stop_server(self):
        # Close server socket and stop accepting connections
        if self.server_socket:
            self.server_socket.close()
            self.server_socket = None
        if self.accept_thread and self.accept_thread.is_alive():
            self.accept_thread.join()

        # Clear existing client info labels
        self.clear_client_info()

        # Update button states
        self.start_button.config(state=tk.NORMAL)
        self.stop_button.config(state=tk.DISABLED)

    def accept_connections(self):
        while True:
            try:
                readable, _, _ = select.select([self.server_socket] + self.clients, [], [], 0.1)
                for s in readable:
                    if s is self.server_socket:
                        client_socket, client_address = self.server_socket.accept()
                        client_socket.setblocking(False)
                        self.clients.append(client_socket)
                    else:
                        data = s.recv(4096)
                        if not data:
                            self.clients.remove(s)
                            s.close()
                        else:
                            encrypted_message_base64 = data.decode('utf-8').strip()
                            decrypted_message = decrypt_message(self.private_key, encrypted_message_base64)
                            if decrypted_message:
                                client_info = json.loads(decrypted_message)
                                self.update_client_info(client_info, s.getpeername())
                            else:
                                print("Failed to decrypt message from", s.getpeername())
            except Exception as e:
                print("Error:", e)
                break

    def update_client_info(self, client_info, client_address):
        ip = client_address[0]
        server_name = client_info["name"]
        cpu_usage = client_info["cpu_usage"]
        memory_usage = client_info["memory_usage"]
        disk_usage = client_info["disk_usage"]

        if ip not in self.client_info_labels:
            row = len(self.client_info_labels) + 1
            ttk.Label(self.output_frame, text=ip).grid(row=row, column=0, padx=5, pady=5)
            self.client_info_labels[ip] = {
                "name" : tk.StringVar(value="{:s}".format(server_name)),
                "cpu": tk.StringVar(value="{:.2f}".format(cpu_usage)),
                "memory": tk.StringVar(value="{:.2f}".format(memory_usage)),
                "disk": tk.StringVar(value="{:.2f}".format(disk_usage))
            }
            ttk.Label(self.output_frame, textvariable=self.client_info_labels[ip]["name"]).grid(row=row, column=1, padx=5, pady=5)
            ttk.Label(self.output_frame, textvariable=self.client_info_labels[ip]["cpu"]).grid(row=row, column=2, padx=5, pady=5)
            ttk.Label(self.output_frame, textvariable=self.client_info_labels[ip]["memory"]).grid(row=row, column=3, padx=5, pady=5)
            ttk.Label(self.output_frame, textvariable=self.client_info_labels[ip]["disk"]).grid(row=row, column=4, padx=5, pady=5)
        else:
            self.client_info_labels[ip]["name"].set("{:s}".format(server_name))
            self.client_info_labels[ip]["cpu"].set("{:.2f}".format(cpu_usage))
            self.client_info_labels[ip]["memory"].set("{:.2f}".format(memory_usage))
            self.client_info_labels[ip]["disk"].set("{:.2f}".format(disk_usage))

    def clear_client_info(self):
        # Clear existing client information labels
        for label in self.client_info_labels.values():
            label["name"].set("")
            label["cpu"].set("")
            label["memory"].set("")
            label["disk"].set("")

if __name__ == "__main__":
    root = tk.Tk()
    app = ServerMonitorApp(root)
    root.mainloop()
