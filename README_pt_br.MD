# MyMalloc

Simples implementação das funções `malloc` e `free` utilizando a chamada de
sistema `mmap`.

Este exercicío é proposto por Igor Zhirkov em seu livro Programação em Baixo
Nível C, Assembly e execução de programas na arquitetura Intel 64, na página 
369.

## Construíndo o Executável

```bash
autoreconf -i
mkdir build/ && cd build/
../configure
make
./test
cat memlog
```

## Algoritmo Utilizando

Foi utilizado uma lista simples para controlar a memória alocada. Inicialmente
é feito uma chamada a `mmap` para mapear uma página em memória, esta página é
utilizada pela lista, cada nó é inserido sequencialmente na ordem de entrada.

Quando a memória na página atual é excedida há novas requisição à `mmap` por 
mais páginas. Essas novas páginas obtidas são concatenadas sequencialmente.

Dada uma requisição por memória através de `malloc` a lista é iterada a fim
de encontrar uma região livre. Quando a região é encontrada, o bloco é dividido
formando um bloco para armazenar os dados e outro livre com a memória
excedente.

Cada bloco possuí um cabeçalho para comportar o *overhead* necessário para o 
controle da lista. O cabeçalho é apresentado abaixo:

```C
struct mem {
  struct mem *next;
  size_t capacity;
  bool is_free;
};
```

Ao liberar um bloco com a chamada da função `free` o campo `is_free` do
cabeçalho é alterado para `true`, ficando assim reutilizável. Ainda, se o
próximo bloco estiver livre, ambos são concatenados em um bloco maior.

## Próximos Passos

Ao passo que alocar memória em páginas consecutivas reduzem a quantidade de
chamadas de sistema há grandes probabilidades de a aplicação manter memórias 
excedentes, sendo assim um crescimento controlado pode ser mais eficáz.

Outro problema está na fragmentação de memória, não há, até então, um mecanismo
para lidar com esse problema.

- Documentar o código.

Estudar a implementação feita pela GNU Libc.
