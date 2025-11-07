#include <stdio.h>
#include <stdlib.h>
#include "cidades.h"

int main() {
    const char *nomeArquivo = "Teste01.txt";

    Estrada *estrada = getEstrada(nomeArquivo);
    if (!estrada) {
        printf("Erro ao ler arquivo ou arquivo inválido.\n");
        return 1;
    }

    printf("Numero de cidades: %d\n", estrada->N);
    printf("Comprimento total da estrada: %d\n", estrada->T);
    printf("Cidades na estrada:\n");
    for (Cidade *p = estrada->Inicio; p != NULL; p = p->Proximo) {
        printf(" - %s em posicao %d\n", p->Nome, p->Posicao);
    }

    double menorViz = calcularMenorVizinhanca(nomeArquivo);
    if (menorViz < 0) {
        printf("Erro no calculo da menor vizinhanca.\n");
        return 1;
    }
    printf("Menor vizinhanca: %.2f\n", menorViz);

    char *nomeMenorViz = cidadeMenorVizinhanca(nomeArquivo);
    if (!nomeMenorViz) {
        printf("Erro ao obter cidade com menor vizinhanca.\n");
        return 1;
    }
    printf("Cidade com menor vizinhanca: %s\n", nomeMenorViz);
    free(nomeMenorViz);

    return 0;
}
