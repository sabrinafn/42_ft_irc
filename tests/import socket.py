import socket
import threading
import time

SERVER = 'localhost'
PORT = 6667
NUM_CLIENTS = 10

# Função que simula um cliente IRC
def irc_client(n):
	try:
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.connect((SERVER, PORT))
		s.sendall(f"PASS pass123\r\nNICK teste{n}\r\nUSER teste{n} 0 * : teste\r\nJOIN #canal\r\n".encode())
		# time.sleep(2)  # Aguarda o JOIN ser processado
		while True:
			#  msg = f"privmsg #canal : teste\r\n"
			msg = "!time\r\n"
			s.sendall(msg.encode())
			time.sleep(0.2)

		# for i in range(100):
		# 	msg = f"bot #canal\r\n"
		# 	s.sendall(msg.encode())
		# 	time.sleep(0.1)

	except Exception as e:
		print(f"Cliente {n} erro: {e}")

threads = []
for i in range(NUM_CLIENTS):
	t = threading.Thread(target=irc_client, args=(i,))
	t.daemon = True  # Permite encerrar todos com Ctrl+C
	t.start()
	threads.append(t)
	time.sleep(0.1)  # Pequeno delay para não conectar todos ao mesmo tempo

try:
	for t in threads:
		t.join()
except KeyboardInterrupt:
	print("Interrompendo todos os clientes...")

print("Todos os clientes finalizaram.")
