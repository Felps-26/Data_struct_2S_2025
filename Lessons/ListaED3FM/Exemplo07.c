#include <stdio.h>
#include <stdlib.h>



typedef struct Item {
    int Chave;
    struct Item * Proximo;

} Item;

Item * CriarItem(int chave);

void ExibirLista(Item * inicio);
void InserirLista(Item * inicio, Item * I, int posicao);
void RemoverLista(Item * inicio, int posicao);

int main(){
    Item * A = CriarItem(17);
    Item * B = CriarItem(29);
    Item * C = CriarItem(41);
    Item * D = CriarItem(97);

    A->Proximo = B;
    B->Proximo = C;
    C->Proximo = D;
    D->Proximo = NULL;


    //inserir um novo nó com chave 67 na posição 2.
    Item * E = CriarItem(67);
    E->Proximo = D;
    C->Proximo = E;

    
    //excluir o no da posiçao 1 (29)
    A->Proximo = C;
    free(B);
    ExibirLista(A);



}

Item * CriarItem(int chave) {
    
    Item * X = (Item*) malloc(sizeof(Item));
    X->Chave = chave;
    X->Proximo = NULL;
    return X;
}

void ExibirLista(Item * inicio) {
    for(Item * X = inicio; X != NULL; X = X->Proximo) {
        printf("%d\t", X->Chave);
    }
}