import socket  

#def send():
#    UDP_IP = "127.0.0.1"
#    UDP_PORT = 5005
#    MESSAGE = b"Hello, World!"
#
#    print("UDP target IP: %s" % UDP_IP)
#    print("UDP target port: %s" % UDP_PORT)
#    print("message: %s" % MESSAGE)
#
#    sock = socket.socket(socket.AF_INET, # Internet
#                         socket.SOCK_DGRAM) # UDP
#    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

#def receive():

UDP_IP = socket.gethostbyname("j3") 
# UDP_IP = "127.0.0.1"
UDP_ID_J2 = socket.gethostbyname("j2")


UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # INternet, UDP
sock.bind((UDP_IP, UDP_PORT))
print("preparing to receive")

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    print("receiving")
    print("received message: %s" % data)
    sock.sendto(data, (UDP_ID_J2, UDP_PORT))


