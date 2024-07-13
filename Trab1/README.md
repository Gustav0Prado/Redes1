# 🖥️🔌🔌🖥️ Trabalho 1 - Raw Socket 😈😈😈
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

## ▶️ Como Rodar
* Você vai precisar de:
   * Dois computadores dos quais você tenha acesso root
   * Um cabo de rede pra conectar eles
   * Esse projeto (repositório) baixado em ambos
   * 2 Litros de água benta
   * 3 crucifixos de madeira
   * 1 (uma) unidade de Bíblia, de preferência em Latim
* Precisa rodar `make` antes, mas é tranquilo
* `sudo ./main cliente` e `sudo ./main servidor` para rodar o cliente e servidor, respectivamente
* Você pode rodar o cliente em um computador e o servidor no outro OOOOOOU trocar a interface de rede no arquivo *main* de "eno1" pra "lo" e tentar rodar localmente em loopback
   * ⚠️⚠️ ATENÇÃO!!! Se você rodar em loopback **TODOS** os pacotes vão ficar duplicados e eu não sei se tem como evitar/arrumar isso, recomendo caso tenha um computador só e queira testar, mas pra entrega/apresentação final recomendo fazer com dois computadores mesmo

## 📂 Arquivos
* Como dá pra ver, tem uma cacetada de arquivos nesse projeto porque ele é bem grande, então vou tentar fazer um resumo:
   * **main.c** -  Programa e execução principal do projeto, apenas repassa os comandos lidos do terminal para os respectivos outros arquivos
   * **socket.c** - Arquivo do professor que cria a conexão Raw Socket. Não seja burro que nem eu e pegue o certo e atualizado que ele passar, senão seu trabalho pode não funcionar corretamente 👍👍
   * **struct.c** - Arquivo com a struct que usamos para os pacotes, ela usa *bitfield*s do C com algumas maracutaias (packed e big-endian), então tem exatamente **24 bits**, nem a mais, nem a menos, na exata ordem correta pra enviar
   * **pacote.c** - Funções para ler arquivos, escrever pacotes e enviar eles com a função *envia* que tem nesse arquivo
   * **comandos.c** - Funções que executam os comandos digitados no terminal, ou seja, vê cada caso de cada comando e executa as funções necessárias pra ele
   * **utils.c** - Só uma função que trata entrada pra escolher entre servidor ou cliente

## 🧐 Outras informações
* Mais informações e detalhes maiores no PDF do relatório do trabalho!
