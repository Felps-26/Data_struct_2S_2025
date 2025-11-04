#ifndef MUSICA_H
#define MUSICA_H

typedef struct Item {
    char Titulo[50];
    char Autor[100];
    int Ano;
    struct Item *Anterior;
    struct Item *Posterior;
} Item;

typedef struct Lista {
    int Tamanho;
    struct Item * Inicio;
    struct Item * Fim;
} Lista;

// Funções de manipulação de itens
Item * criarItem(char * Titulo, char * Autor, int Ano);
void destruirItem(Item * item);

// Funções de manipulação da lista
Lista * criarLista();
void destruirLista(Lista * L);
void inserirItem(Lista * L, Item * novo);
void removerItem(Lista * L, Item * item);

// Funções de exibição
void exibirInicio(Lista * L);
void exibirFim(Lista * L);
void exibirMusicas(Lista * L, int Ano);

#endif