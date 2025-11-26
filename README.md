**Sistema de Impressão Elgin via DLL em C**

Este projeto permite a comunicação completa com impressoras Elgin utilizando a DLL oficial E1_Impressora01.dll.
A aplicação oferece um menu interativo para impressão, abertura de gaveta, leitura de XML SAT e diversos outros comandos suportados pelas impressoras térmicas Elgin.

**Funcionalidades:**

Configuração de conexão (modelo, porta, tipo)

Abrir e fechar conexão com a impressora

Impressão de texto formatado

Impressão de QR Code

Impressão de código de barras

Impressão de XML SAT e Cancelamento SAT

Abertura de gaveta Elgin e gavetas genéricas

Emissão de sinal sonoro

Carregamento dinâmico da DLL



**Estrutura do Projeto:**

src/
 └── main.c
lib/
 └── E1_Impressora01.dll


 
**Como Compilar?**
Usando MinGW (recomendado)
gcc src/main.c -o projeto.exe -Wall



************************************Atenção!************************************
Certifique-se de que a DLL E1_Impressora01.dll esteja na mesma pasta do executável.



**Como Executar**
./projeto.exe


O menu será exibido no terminal:

1 - Configurar Conexao

2 - Abrir Conexao

3 - Impressao Texto

4 - Impressao QRCode

5 - Impressao Cod Barras

6 - Impressao XML SAT

7 - Impressao XML Canc SAT

8 - Abrir Gaveta Elgin

9 - Abrir Gaveta

10 - Sinal Sonoro

0 - Fechar Conexao e Sair



**Requisitos**

Windows (7+)

Compilador C (MinGW ou MSVC)

DLL E1_Impressora01.dll

Impressora Elgin compatível (i7, i7 Plus, i8, i9, Fitpos, MP-4200 etc)


**Desenvolvido por: João, Pedro, Matheus e Rai**
