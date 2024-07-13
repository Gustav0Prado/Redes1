# Repositório com Trabalhos de Redes
## UFPR - CI1058 - Redes 1


## 🖥️🔌🔌🖥️ Trabalho 1 - Raw Socket
* Objetivo é mandar mensagens (pacotes) entre dois computadores fazendo os sockets ""na mão"
* Sistema de backup, ou seja, enviamos e recuperamos arquivos. Com o hash MD5 podemos verificar a integridade dos arquivos passados
* Pacotes baseados no protocolo Kermit:

| Campo                |  bits  | Outras infos               |
|----------------------|--------|----------------------------|
| Marcador de início   | 8 bits | 01111110                   |
| Tamanho              | 6 bits | Tamanho dos dados          |
| Sequência            | 6 bits | Número de Sequência        |
| Tipo                 | 4 bits | Tipo dos dados             |
| Verificação de Erros | 8 bits | Tamanho + Sequência + Tipo |

* MD5 está meio quebrado! **(Não foi corrigido)** 🥶🥶
* Pacotes estão sendo enviados com **0xff** após cada byte para evitar que placas de rede malvadas "comam" alguns bytes específicos, coisa que acontece no labóratorio do Dinf, por exemplo. Logo, caso queiramos enviar a mensagem "Hello", é como se mandassemos os seguintes bytes:
   * | 'H' | 0xff | 'e' | 0xff | 'l' | 0xff | 'l' | 0xff | 'o' | 0xff |
* Não foi feito do jeito mais claro e óbvio possível, mas foi o que deu pra fazer em 2 semanas 🤷🤷 
* Implementado em C usando Raw Socket para mandar os pacotes


## 🃏 ♠️ ♥️ ♦️ ♣️ Trabalho 2 - Dalmuti
* Jogo "O Grande Dalmuti"
* Não lembro como o jogo funciona exatamente, pesquisar no Google talvez ajude mais 😢😢
* A ideia é termos 4 (ou mais) máquinas em uma rede em anel que se conectam e os usuários conseguem jogar pelo terminal
* Máquinas definidas no arquivo de configuração, usando o **hostname** e uma **porta**
* Implementado em Python usando a biblioteca *socket* padrão

## ☝️🤓 Mais informações
* A pasta de cada trabalho tem um README e Relatório próprio sobre o mesmo! Tem mais detalhes de cada um deles lá se quiser olhar!