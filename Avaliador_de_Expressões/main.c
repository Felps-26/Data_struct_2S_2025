#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

/* protótipo da função pública implementada em expressao.c */
int processar_expressao(const char *input, char **converted_out, float *value_out, int *isPosOut);

int main() {
    /* selecione a expressão a testar aqui:
       - infixa pode estar sem espaços, será normalizada: "(3+4)*5" ou "( 3 + 4 ) * 5"
       - posfixa deve ter tokens separados por espaço: "3 4 + 5 *" */
    const char expressao[] = "0.5 45 sen 2 ^ +";

    printf("Expressao: %s\n", expressao);

    char *convertido = NULL;
    float resultado = 0.0f;
    int eraPos = 0;

    if (processar_expressao(expressao, &convertido, &resultado, &eraPos) != 0) {
        /* em erro mostra labels vazios conforme pedido */
        printf("convertido para %s: \n", eraPos ? "infixa" : "posfixa");
        printf("resultado: 0.000000\n");
        return 1;
    }

    if (eraPos) {
        /* entrada era posfixa -> mostramos conversão para infixa */
        if (convertido) {
            printf("convertido para infixa: %s\n", convertido);
            free(convertido);
        } else {
            printf("convertido para infixa: \n");
        }
    } else {
        /* entrada era infixa -> mostramos conversão para posfixa */
        if (convertido) {
            printf("convertido para posfixa: %s\n", convertido);
            free(convertido);
        } else {
            printf("convertido para posfixa: \n");
        }
    }

    printf("resultado: %.6f\n", resultado);
    return 0;
}
