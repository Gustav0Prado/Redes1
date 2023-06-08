import socket

class Mensagem:
  def __init__(self, inicio, origem, jogada, confirmacao, fim):
      self.inicio = inicio
      self.origem = origem
      self.jogada = jogada
      self.confirmacao = confirmacao
      self.fim = fim
      return
  def __str__(self):
      return self.inicio + str(self.origem) + str(self.jogada) + str(self.confirmacao) + self.fim


m1 = Mensagem("(", 0, 35, 0, ")")
print(str(m1))

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.sendto(str(m1).encode(), ("127.0.0.1", 5000) )