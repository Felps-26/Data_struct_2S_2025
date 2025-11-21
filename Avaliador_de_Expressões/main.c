#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

int main() {
    Expressao exp;
    
    // Teste 1: 2 3 + log 5 /
    strcpy(exp.posFixa, "2 3 + log 5 /");
    
    printf("Expressao Pos-fixa: %s\n", exp.posFixa);
    
    exp.Valor = getValorPosFixa(exp.posFixa);
    
    char *formaInfixa = getFormaInFixa(exp.posFixa);
    
    if (formaInfixa == NULL) {
        printf("Erro na conversao!\n");
        return 1;
    }
    
    strcpy(exp.inFixa, formaInfixa);
    printf("Expressao Infixa: %s\n", exp.inFixa);
    
    printf("Resultado: %.6f\n\n", exp.Valor);
    
    free(formaInfixa);
    
    return 0;
}