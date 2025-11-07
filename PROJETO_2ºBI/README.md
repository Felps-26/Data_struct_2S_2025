Projeto: Estrada de Wakanda
Professor: Marcelo Eustáquio
Alunos: Filipi Leite Ramos; Felipe Conrado Nemer; João Herique Faria de Castro Santana; Arthur Cardoso Lima
===========================

Este projeto implementa, em linguagem C, uma solução para o problema de cidades distribuídas linearmente em uma estrada, permitindo análise de vizinhança entre cidades de acordo com regras bem definidas.

Estrutura dos Arquivos
---------------------
- cidades.h: Declaração das structs e protótipos das funções principais do projeto.
- cidades.c: Implementação das funções de leitura, ordenação, cálculo e busca das cidades e vizinhanças.

Principais Structs
------------------
// Representa uma cidade em uma estrada
struct Cidade {
    char Nome[256];            // Nome da cidade
    int Posicao;               // Posição da cidade na estrada
    struct Cidade *Proximo;    // Ponteiro para a próxima cidade
};

// Representa a estrada com suas cidades
struct Estrada {
    int N;             // Número de cidades
    int T;             // Comprimento da estrada
    struct Cidade *Inicio;    // Ponteiro para a primeira cidade
};

Funções Principais
------------------
- Estrada *getEstrada(const char *nomeArquivo): Lê o arquivo, valida restrições, ordena cidades por posição e retorna ponteiro para a estrada. Retorna NULL se houver erro.
- double calcularMenorVizinhanca(const char *nomeArquivo): Calcula e retorna o menor valor de vizinhança entre as cidades da estrada informada. Retorna -1 em caso de erro.
- char *cidadeMenorVizinhanca(const char *nomeArquivo): Retorna o nome da cidade com menor vizinhança. A string é alocada dinamicamente e deve ser liberada por quem chama.

Como funciona a vizinhança
--------------------------
Para cada cidade, a vizinhança é o trecho da estrada próximo à cidade, calculado pelo ponto médio entre ela e suas vizinhas. Para cidades nas pontas, considera o início/fim da estrada até o ponto médio. Exemplo:

Estrada de tamanho 10, cidades nas posições 5 ("A") e 8 ("B"):
- Vizinhança de "A": do começo até o meio entre 5 e 8 (0 a 6.5)
- Vizinhança de "B": do meio entre 5 e 8 até o fim (6.5 a 10)

Formato do arquivo de entrada
-----------------------------
T
N
posicao1 NomeCidade1
posicao2 NomeCidade2
...
