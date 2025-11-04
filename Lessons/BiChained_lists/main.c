#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "musica.h"

int main() {
    // Criar itens
    Item * D = criarItem("Karma Police", "Radiohead", 1997); 
    Item * E = criarItem("No Surprises", "Radiohead", 1997); 
    Item * F = criarItem("Thinking About You", "Radiohead", 1993); 
    Item * G = criarItem("Creep", "Radiohead", 1993); 
    Item * H = criarItem("The Bends", "Radiohead", 1995);

    // Verificar se todos os itens foram criados
    if (!D || !E || !F || !G || !H) {
        printf("Erro na criacao dos itens!\n");
        return 1;
    }

    // Criar a playlist
    Lista * PlayList = criarLista();
    if (PlayList == NULL) {
        printf("Erro na criacao da playlist!\n");
        return 1;
    }

    // Inserir itens na playlist
    inserirItem(PlayList, D);
    inserirItem(PlayList, E);
    inserirItem(PlayList, F);
    inserirItem(PlayList, G);
    inserirItem(PlayList, H);

    // Exibir a playlist em ambas as direções
    exibirInicio(PlayList);
    exibirFim(PlayList);

    // Buscar músicas por ano
    exibirMusicas(PlayList, 1997);
    exibirMusicas(PlayList, 1993);
    exibirMusicas(PlayList, 1995);

    // Limpar memória
    destruirLista(PlayList);

    return 0;
}