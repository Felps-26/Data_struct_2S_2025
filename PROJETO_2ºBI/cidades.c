#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cidades.h"

// Constantes para limites máximos
#define MAX_CIDADES 10000
#define MAX_T 1000000

// Libera toda a memória alocada para a lista de cidades
void liberaCidades(Cidade *inicio) {
    while (inicio) {
        Cidade *tmp = inicio;
        inicio = inicio->Proximo;
        free(tmp); // Libera a memória da cidade atual
    }
}

/*
    Função responsável por ler o arquivo de entrada,
    criar a lista encadeada de cidades ordenadas por posição
    e retornar um ponteiro para a estrutura Estrada.
*/
Estrada *getEstrada(const char *nomeArquivo) {
    FILE *fp = fopen(nomeArquivo, "r"); // Abre arquivo para leitura
    if (!fp) return NULL; // Verifica se o arquivo foi aberto com sucesso
    
    int T, N;
    // Lê o tamanho da estrada
    if (fscanf(fp, "%d", &T) != 1) {
        fclose(fp);
        return NULL;
    }
    // Lê o número de cidades
    if (fscanf(fp, "%d", &N) != 1) {
        fclose(fp);
        return NULL;
    }
    // Verifica se os dados estão dentro dos limites permitidos
    if (T < 3 || T > MAX_T || N < 2 || N > MAX_CIDADES) {
        fclose(fp);
        return NULL;
    }
    // Aloca a estrutura Estrada
    Estrada *estrada = (Estrada*)malloc(sizeof(Estrada));
    if (!estrada) {
        fclose(fp);
        return NULL;
    }
    estrada->N = N; // Quantidade de cidades
    estrada->T = T; // Tamanho da estrada
    estrada->Inicio = NULL; // Inicia lista vazia

    // Array temporário para armazenar as cidades lidas
    Cidade *cidades_temp[MAX_CIDADES];
    // Array auxiliar para verificar posições únicas
    int *posicoes = (int*)malloc(N * sizeof(int));
    if (!posicoes) {
        fclose(fp);
        free(estrada);
        return NULL;
    }

    // Loop para ler todas as cidades do arquivo
    for (int i = 0; i < N; i++) {
        int pos;
        char nome[256];
        // Lê posição e nome da cidade
        if (fscanf(fp, "%d %[^\n]", &pos, nome) != 2) {
            for (int j = 0; j < i; j++) free(cidades_temp[j]);
            free(estrada);
            free(posicoes);
            fclose(fp);
            return NULL;
        }
        // Verifica se a posição está dentro dos limites
        if (pos < 0 || pos > T) {
            for (int j = 0; j < i; j++) free(cidades_temp[j]);
            free(estrada);
            free(posicoes);
            fclose(fp);
            return NULL;
        }
        // Verifica se há cidades em posições repetidas
        for (int j = 0; j < i; j++) {
            if (posicoes[j] == pos) {
                for (int k = 0; k < i; k++) free(cidades_temp[k]);
                free(estrada);
                free(posicoes);
                fclose(fp);
                return NULL;
            }
        }
        posicoes[i] = pos; // Guarda posição para controle

        // Aloca e preenche os dados de uma cidade
        Cidade *nova = (Cidade*)malloc(sizeof(Cidade));
        if (!nova) {
            for (int j = 0; j < i; j++) free(cidades_temp[j]);
            free(estrada);
            free(posicoes);
            fclose(fp);
            return NULL;
        }
        strcpy(nova->Nome, nome);
        nova->Posicao = pos;
        nova->Proximo = NULL;
        cidades_temp[i] = nova; // Insere no array temporário
    }

    fclose(fp); // Fecha arquivo após leitura completa

    // Ordena as cidades pelo campo posição usando bubble sort
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - i - 1; j++) {
            if (cidades_temp[j]->Posicao > cidades_temp[j + 1]->Posicao) {
                // Troca posições das cidades
                Cidade *temp = cidades_temp[j];
                cidades_temp[j] = cidades_temp[j + 1];
                cidades_temp[j + 1] = temp;
            }
        }
    }

    // Monta a lista encadeada com as cidades ordenadas
    estrada->Inicio = cidades_temp[0];
    for (int i = 0; i < N - 1; i++) {
        cidades_temp[i]->Proximo = cidades_temp[i + 1];
    }
    cidades_temp[N - 1]->Proximo = NULL; // Última cidade aponta para NULL

    free(posicoes); // Libera array auxiliar
    return estrada;
}

/*
    Calcula a menor vizinhança entre todas as cidades da estrada.
    Uma vizinhança é definida pela distância entre seus limites
    de influência (meio do caminho para a anterior e meio do caminho para a próxima).
*/
double calcularMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo); // Lê estrada do arquivo
    if (!estrada) return -1;

    // Transfere as cidades para um vetor para facilitar indexação
    Cidade *vetor[MAX_CIDADES];
    int N = estrada->N;
    Cidade *c = estrada->Inicio;
    for (int i = 0; i < N; i++) {
        vetor[i] = c;
        c = c->Proximo;
    }

    double menorVizinhanca = 1e12; // Inicializa com valor alto

    // Loop por todas as cidades para calcular vizinhanças
    for (int i = 0; i < N; i++) {
        double ini, fim;
        // Primeiro limite (fronteira esquerda)
        if (i == 0) ini = 0;
        else ini = (vetor[i]->Posicao + vetor[i-1]->Posicao) / 2.0;
        // Segundo limite (fronteira direita)
        if (i == N-1) fim = estrada->T;
        else fim = (vetor[i]->Posicao + vetor[i+1]->Posicao) / 2.0;
        // Calcula tamanho da vizinhança
        double vizinhanca = fim - ini;
        // Atualiza caso seja menor que as anteriores
        if (vizinhanca < menorVizinhanca) menorVizinhanca = vizinhanca;
    }

    liberaCidades(estrada->Inicio); // Libera memória das cidades
    free(estrada); // Libera memória da estrada
    return menorVizinhanca;
}

/*
    Retorna o nome da cidade com menor vizinhança.
    Segue a mesma lógica do cálculo anterior, mas armazena o ponteiro para a cidade.
*/
char *cidadeMenorVizinhanca(const char *nomeArquivo) {
    Estrada *estrada = getEstrada(nomeArquivo);
    if (!estrada) return NULL;

    // Vetor para indexar facilmente as cidades
    Cidade *vetor[MAX_CIDADES];
    int N = estrada->N;
    Cidade *c = estrada->Inicio;
    for (int i = 0; i < N; i++) {
        vetor[i] = c;
        c = c->Proximo;
    }

    double menorVizinhanca = estrada->T;
    int indiceMenor = 0; // Índice da cidade com menor vizinhança

    for (int i = 0; i < N; i++) {
        double ini = (i == 0) ? 0.0 : (vetor[i]->Posicao + vetor[i-1]->Posicao) / 2.0;
        double fim = (i == N-1) ? (double)estrada->T : (vetor[i]->Posicao + vetor[i+1]->Posicao) / 2.0;
        double vizinhanca = fim - ini;

        if (vizinhanca < menorVizinhanca) {
            menorVizinhanca = vizinhanca;
            indiceMenor = i;
        }
    }

    // Aloca memória para o nome da cidade a ser retornado
    char *res = (char*)malloc(256);
    if (res) strcpy(res, vetor[indiceMenor]->Nome);

    liberaCidades(estrada->Inicio); // Limpa memória usada
    free(estrada);
    return res;
}
