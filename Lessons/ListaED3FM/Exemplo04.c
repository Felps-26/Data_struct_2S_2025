#include <stdio.h>
#include <stdlib.h>

typedef struct Item {
    int Chave;
    struct Item * Proximo;

} Item;

int main(){
    Item * A = (Item*) malloc (sizeof(Item));
    Item * B = (Item*) malloc(sizeof(Item));
    Item * C = (Item*) malloc(sizeof(Item));
    Item * D = (Item*) malloc(sizeof(Item));

    A->Chave = 17;
    B->Chave = 29;
    C->Chave = 41;
    D->Chave = 97;

    A->Proximo = B;
    B->Proximo = C;
    C->Proximo = D;
    D->Proximo = NULL;

    printf("%X\t %d\n", A, A->Chave);
    printf("%X\t %d\n", B, B->Chave);
    printf("%X\t %d\n", C, C->Chave);
    printf("%X\t %d\n", D, D->Chave);

    A->Proximo = B;
    B->Proximo = C;
    C->Proximo = D;
    D->Proximo = NULL;

   //percorrer a lista (visitar tds os nós da lista) -> em um for

   //printf("\nLista:\n");

   //Item * X = A;        //X aponta para o primeiro nó da lista
   //printf("%X\t %d\n", X, X->Chave);

   //X = X->Proximo;      //X aponta para o segundo nó da lista
   //printf("%X\t %d\n", X, X->Chave);

   // X = X->Proximo;     //X aponta para o terceiro nó da lista
   // printf("%X\t %d\n", X, X->Chave);

   // X = X->Proximo;     //X aponta para o quarto nó da lista
   // printf("%X\t %d\n", X, X->Chave);

   // X = X->Proximo;     //X aponta para NULL (final da lista)
   // if(X == NULL) printf("Fim da lista\n");

    printf("\nLista usando for:\n");
    
    // Percorre a lista usando for
    for(Item *X = A; X != NULL; X = X->Proximo) {
        printf("%X\t %d\n", X, X->Chave);
    }

    return 0;
}