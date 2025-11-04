#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

Item * criarItem(char * Titulo, char * Autor, int Ano) {
    Item * X = (Item *) malloc(sizeof(Item));
    if (X == NULL) {
        printf("Erro: nao foi possivel alocar memoria para o Item.\n");
        return NULL;
    }

    strcpy(X->Titulo, Titulo);
    strcpy(X->Autor, Autor);
    X->Ano = Ano;
    X->Anterior = NULL;
    X->Posterior = NULL;
    
    return X;
}

void destruirItem(Item * item) {
    if (item != NULL) {
        free(item);
    }
}

Lista * criarLista() {
    Lista * L = (Lista *) malloc(sizeof(Lista));
    if (L == NULL) {
        printf("Erro: nao foi possivel alocar memoria para a Lista.\n");
        return NULL;
    }
    
    L->Tamanho = 0;
    L->Inicio = NULL;
    L->Fim = NULL;
    
    return L;
}

void destruirLista(Lista * L) {
    if (L != NULL) {
        Item * atual = L->Inicio;
        while (atual != NULL) {
            Item * temp = atual->Posterior;
            destruirItem(atual);
            atual = temp;
        }
        free(L);
    }
}

void inserirItem(Lista * L, Item * novo) {
    if (L == NULL || novo == NULL) return;
    
    if (L->Inicio == NULL) {
        L->Inicio = novo;
        L->Fim = novo;
    } else {
        L->Fim->Posterior = novo;
        novo->Anterior = L->Fim;
        L->Fim = novo;
    }
    L->Tamanho++;
}

void exibirInicio(Lista * L) {
    if (L == NULL) return;
    
    Item * Atual = L->Inicio;
    printf("\nLista do inicio ao fim:\n");
    for (int i = 0; i < L->Tamanho; i++) {
        printf("%d\t%s - %s (%d)\n", i, Atual->Titulo, Atual->Autor, Atual->Ano);
        Atual = Atual->Posterior;
    }
    printf("\n");
}

void exibirFim(Lista * L) {
    if (L == NULL) return;
    
    Item * Atual = L->Fim;
    printf("\nLista do fim ao inicio:\n");
    for (int i = L->Tamanho - 1; i >= 0; i--) {
        printf("%d\t%s - %s (%d)\n", i, Atual->Titulo, Atual->Autor, Atual->Ano);
        Atual = Atual->Anterior;
    }
    printf("\n");
}

void exibirMusicas(Lista * L, int Ano) {
    if (L == NULL || L->Inicio == NULL) {
        printf("Lista vazia ou invalida!\n");
        return;
    }

    printf("\nMusicas lancadas em %d:\n", Ano);
    Item * Atual = L->Inicio;
    int encontrou = 0;

    while (Atual != NULL) {
        if (Atual->Ano == Ano) {
            printf("%s - %s\n", Atual->Titulo, Atual->Autor);
            encontrou = 1;
        }
        Atual = Atual->Posterior;
    }

    if (!encontrou) {
        printf("Nenhuma musica encontrada para o ano %d\n", Ano);
    }
}