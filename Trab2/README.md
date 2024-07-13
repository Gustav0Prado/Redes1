# 🃏 ♠️ ♥️ ♦️ ♣️ Trabalho 2 - O Grande Dalmuti

## 💡 Ideia

* Jogo "O Grande Dalmuti"
* Não lembro como o jogo funciona exatamente, pesquisar no Google talvez ajude mais 😢😢
* A ideia é termos 4 (ou mais) máquinas em uma rede em anel que se conectam e os usuários conseguem jogar pelo terminal
* Máquinas definidas no arquivo de configuração, usando o **hostname** e uma **porta**
* Implementado em Python usando a biblioteca *socket* padrão

## ▶️ Como Rodar

* Apenas precisa ter *python3* instalado
* `pyhton3 ./Dalmuti.py`
* ⚠️⚠️ ATENÇÃO!! Para conseguir iniciar e rodar corretamente é preciso ter todas as máquinas/hosts do arquivo de configuração acessíveis por ssh e sem precisar inserir senha!
* Eu fiz isso usando `ssh-copy-id -i ~/.ssh/id_rsa.pub <nome da máquina/host>`, dado que você tenha essa chave ssh especificada

## 📦 O Pacote

| Campo              |    bytes    |
|--------------------|-------------|
| Marcador de início | 1 byte      |
| Origem             | 1 byte      | 
| Tipo               | 2 bytes     |
| Dados              | 0 a 5 bytes | 
| Confirmação        | 8 bytes     | 
| Marcador de fim    | 1 bytes     | 

## 🤔 De resto
* Mais informações e detalhes maiores no PDF do relatório do trabalho!