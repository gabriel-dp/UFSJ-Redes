# Técnicas de programação de servidores TCP

```c
#define TRABALHO 2
#define SEMESTRE "2024.2"
#define PROFESSOR "Rafael Sachetto"
#define ESTUDANTES ["Breno Estever", "Gabriel de Paula", "Guilherme Francis"]
```

[`> Ver documentação <`](./docs/documentation-ptbr.pdf)

&nbsp;

## 🪢😵 Jogo da Forca

O jogo da forca é um jogo multi-jogador colaborativo. Como jogar:

- Letra por letra (no máximo 6 erros)
- Palavra inteira (uma única chance)

Todos ganham ou todos perdem, divirta-se com seus amigos!

&nbsp;

## 🖥 Como jogar

1) Escolha um tipo de servidor

    |Servidor|Executável|
    |:-|:-|
    |Iterativo|`1-iterative-server`|
    |Threads|`2-threads-server`|
    |Fila|`3-queue-server`|
    |Select|`4-select-server`|

2) Compile o servidor escolhido e o cliente

    ```bash
    make <servidor> && make client
    ```

3) Execute o servidor

    Para executar o servidor é necessário informar o endereço de IP, a porta a qual o servidor deve escutar por conexões e o arquivo contendo as palavras.

    ```bash
    ./bin/<servidor> -i <ip> -p <porta> -f <arquivo_palavras>
    ```

    > O arquivo de palavras deve seguir o modelo [palavras_forca.txt](./tests/palavras_forca.txt)

4) Execute os clientes

    Para executar o cliente também é necessário definir o IP e a porta (devem ser iguais aos do servidor).

    ```bash
    ./bin/client -i <ip> -p <porta>
    ```
