# Implementação do Protocolo de Parada e Espera usando UDP

```c
#define TRABALHO 3
#define SEMESTRE "2024.2"
#define PROFESSOR "Rafael Sachetto"
#define ESTUDANTES ["Breno Esteves", "Gabriel de Paula", "Guilherme Francis"] 
```

[`> Ver documentação <`](./docs/documentation-ptbr.pdf)

&nbsp;

## 🖥 Como executar

### Compilação

Para compilar os executáveis de cliente e servidor deve-se usar os respectivos comandos:

```bash
make client
make server
```

### Execução

O servidor deve estar em execução antes que os clientes possam enviar arquivos. É necessário informar alguns parâmetros.

```bash
./bin/server -i <ip> -p <porta> -f <caminho_arquivo_saida>
```

Em seguida os clientes podem transmitir os dados. Também é necessário informar os parâmetros-base, sendo ip e porta iguais aos do servidor.

```bash
./bin/client -i <ip> -p <porta> -f <caminho_arquivo_entrada>
```

&nbsp;

### Arquivos de teste

Por padrão os arquivos são incluídos na pasta `/data/`, para que o cliente possa ler e enviar para o servidor que salva no diretório especificado. O arquivo usado nos testes é o `01mb.bin`.

#### Verificação de integridade

É interessante verificar a integridade dos arquivos usando hashs, está incluído o arquivo `01mb.md5` para realizar essa verificação usando:

```bash
md5sum -c 01mb.md5
```

> Ambos os arquivos devem estar no mesmo diretório e possuir o mesmo nome do arquivo original.
