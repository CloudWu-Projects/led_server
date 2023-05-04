import threading
import socket

def handle_client(conn, addr):
    print('Connection address:', addr)

    # Receive data from the client
    data = conn.recv(1024)
    print('Received data:', data)

    # Send a response back to the client
    conn.send(b'Thanks for connecting')

    # Close the connection
    conn.close()

TCP_IP = '0.0.0.0'
TCP_PORT = 10008

# Create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the socket to a specific address and port
s.bind((TCP_IP, TCP_PORT))

# Listen for incoming connections
s.listen(5)

while True:
    # Wait for a client to connect
    conn, addr = s.accept()

    # Create a new thread to handle the client connection
    client_thread = threading.Thread(target=handle_client, args=(conn, addr))
    client_thread.start()
