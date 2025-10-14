#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tarefa.h"

int main(){

    Tarefa T1 = {"Estudar algoritmos de busca", "Lucas Silva", EM_ANDAMENTO, 50.5, 1};
    Tarefa T2 = {"Revisar exercícios de ponteiros", "Joana Souza", FEITO, 100.0, 2};
    Tarefa T3 = {"Fazer resumo de listas ligadas", "Carlos Mendes", EM_ANDAMENTO, 80.0, 0};
    Tarefa T4 = {"Atualizar repositório no GitHub", "Bruna Dias", A_FAZER, 0.0, 1};
    Tarefa T5 = {"Corrigir erros do projeto", "Pedro Lima", EM_ANDAMENTO, 60.0, 2};
    Tarefa T6 = {"Montar slides para apresentação", "Marina Ferreira", EM_ANDAMENTO, 35.0, 0};
    Tarefa T7 = {"Enviar exercícios para o professor", "Ricardo Alves", A_FAZER, 0.0, 1};
    Tarefa T8 = {"Refatorar código de manipulação de arquivos", "Jéssica Torres", FEITO, 100.0, 2};
    Tarefa T9 = {"Organizar agenda de estudos", "Felipe Ramos", EM_ANDAMENTO, 20.0, 0};
    Tarefa T10 = {"Testar funções de ordenação", "Gabriel Rocha", EM_ANDAMENTO, 45.0, 1};

    Lista * Planner = CriarLista(200);

    AdicionarTarefa(Planner, T1);
    AdicionarTarefa(Planner, T2);
    AdicionarTarefa(Planner, T3);
    AdicionarTarefa(Planner, T4);
    AdicionarTarefa(Planner, T5);
    AdicionarTarefa(Planner, T6);
    AdicionarTarefa(Planner, T7);
    AdicionarTarefa(Planner, T8);
    AdicionarTarefa(Planner, T9);
    AdicionarTarefa(Planner, T10);

    for(int i = 0; i < Planner->Tamanho; i++){
        printf("%d\t", i);
        printf("%s\n", Planner->Dados[i].Titulo);
    }

    DestruirLista(Planner);

    printf("%X\n", Planner);

    return 0;
}