# QoSDB
--------------

QoSDB é um estimator de custo de leitura em disco. Seu propósito é de auxiliar na detecção 
dos custos de leitura sequencial e aleatória de um disco rígido.

## Compilando QoSDB
--------------

QoSDB pode ser compilado em Linux. Suporta arquiteturas *big endian* e *little endian* tanto de
32 bit quanto de 64 bit.

Para compilar execute:

    % make

## Executando QoSDB
-------------

Para executar, rode o comando:

    % cd src
    % ./qosdb [FILENAME] [NUM_OF_PAGES]

O QoSDB requer dois parâmetros de entrada, o primeiro é o nome do arquivo binário de teste e o número de páginas
que esse arquivo terá. Por padrão o QoSDB define cada página como 512 bytes. Exemplos:

    % ./qosgbd file1024 200000
    % ./qosgbd file5120 1600000

## Dependências
-------------
- clang
- make


