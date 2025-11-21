#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define MAX 256
#define PI 3.14159265359

typedef struct {
    float dados[MAX];
    int topo;
} Pilha;

void push(Pilha *p, float val) {
    if (p->topo < MAX) p->dados[p->topo++] = val;
}

float pop(Pilha *p) {
    if (p->topo > 0) return p->dados[--p->topo];
    return 0;
}

int ehOperador(const char *token) {
    if (strlen(token) == 1 && strchr("+-*/%^", token[0])) return 1;
    return 0;
}

int ehFuncao(const char *token) {
    if (strcmp(token, "sen") == 0) return 1;
    if (strcmp(token, "cos") == 0) return 1;
    if (strcmp(token, "tg") == 0) return 1;
    if (strcmp(token, "log") == 0) return 1;
    if (strcmp(token, "sqrt") == 0) return 1;
    return 0;
}

int ehNumero(const char *token) {
    if (!token || strlen(token) == 0) return 0;
    
    int temPonto = 0;
    int i = (token[0] == '-') ? 1 : 0;
    
    if (i >= strlen(token)) return 0;
    
    for (; i < strlen(token); i++) {
        if (token[i] == '.') {
            if (temPonto) return 0;
            temPonto = 1;
        } else if (!isdigit(token[i])) {
            return 0;
        }
    }
    return 1;
}

float aplicarFuncao(const char *funcao, float valor) {
    if (strcmp(funcao, "sen") == 0) {
        return sin(valor * PI / 180.0);
    }
    if (strcmp(funcao, "cos") == 0) {
        return cos(valor * PI / 180.0);
    }
    if (strcmp(funcao, "tg") == 0) {
        return tan(valor * PI / 180.0);
    }
    if (strcmp(funcao, "log") == 0) {
        return log10(valor);
    }
    if (strcmp(funcao, "sqrt") == 0) {
        return sqrt(valor);
    }
    return 0;
}

int precedencia(const char *op) {
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0;
}

int ehFuncaoPura(const char *expr) {
    if (strlen(expr) < 3) return 0;
    int i = 0;
    while (i < strlen(expr) && isalpha(expr[i])) i++;
    return (expr[i] == '(');
}

char *getFormaInFixa(char *Str) {
    if (!Str || strlen(Str) == 0) return NULL;
    
    char *infixa = (char*)malloc(strlen(Str) * 4 + 1);
    char *pilha[MAX];
    int topo = 0;
    char *copia = (char*)malloc(strlen(Str) + 1);
    strcpy(copia, Str);
    
    char *token = strtok(copia, " ");
    while (token) {
        if (ehNumero(token)) {
            pilha[topo] = (char*)malloc(strlen(token) + 1);
            strcpy(pilha[topo++], token);
        }
        else if (ehOperador(token)) {
            if (topo < 2) {
                for (int i = 0; i < topo; i++) free(pilha[i]);
                free(copia);
                free(infixa);
                return NULL;
            }
            char *b = pilha[--topo];
            char *a = pilha[--topo];
            
            char *expr = (char*)malloc(strlen(a) + strlen(b) + strlen(token) + 5);
            
            int needParenA = 0;
            int needParenB = 0;
            
            if (strchr(a, '+') || strchr(a, '-') || strchr(a, '*') || strchr(a, '/') || strchr(a, '%') || strchr(a, '^')) {
                needParenA = 1;
            }
            
            if (strchr(b, '+') || strchr(b, '-') || strchr(b, '*') || strchr(b, '/') || strchr(b, '%') || strchr(b, '^')) {
                needParenB = 1;
            }
            
            if (needParenA && !ehFuncaoPura(a)) {
                sprintf(expr, "(%s)%s%s", a, token, needParenB && !ehFuncaoPura(b) ? "(" : "");
                if (needParenB && !ehFuncaoPura(b)) strcat(expr, b), strcat(expr, ")");
                else strcat(expr, b);
            } else if (needParenB && !ehFuncaoPura(b)) {
                sprintf(expr, "%s%s(%s)", a, token, b);
            } else {
                sprintf(expr, "%s%s%s", a, token, b);
            }
            
            pilha[topo++] = expr;
            free(a);
            free(b);
        }
        else if (ehFuncao(token)) {
            if (topo < 1) {
                for (int i = 0; i < topo; i++) free(pilha[i]);
                free(copia);
                free(infixa);
                return NULL;
            }
            char *a = pilha[--topo];
            char *expr = (char*)malloc(strlen(a) + strlen(token) + 3);
            sprintf(expr, "%s(%s)", token, a);
            pilha[topo++] = expr;
            free(a);
        }
        else {
            for (int i = 0; i < topo; i++) free(pilha[i]);
            free(copia);
            free(infixa);
            return NULL;
        }
        token = strtok(NULL, " ");
    }
    
    if (topo != 1) {
        for (int i = 0; i < topo; i++) free(pilha[i]);
        free(copia);
        free(infixa);
        return NULL;
    }
    
    strcpy(infixa, pilha[0]);
    free(pilha[0]);
    free(copia);
    return infixa;
}

float getValorPosFixa(char *StrPosFixa) {
    Pilha pilha = {0};
    char *copia = (char*)malloc(strlen(StrPosFixa) + 1);
    strcpy(copia, StrPosFixa);
    
    char *token = strtok(copia, " ");
    while (token) {
        if (ehNumero(token)) {
            push(&pilha, atof(token));
        }
        else if (ehOperador(token)) {
            float b = pop(&pilha);
            float a = pop(&pilha);
            switch(token[0]) {
                case '+': push(&pilha, a + b); break;
                case '-': push(&pilha, a - b); break;
                case '*': push(&pilha, a * b); break;
                case '/': push(&pilha, (b != 0) ? a / b : 0); break;
                case '%': push(&pilha, (int)a % (int)b); break;
                case '^': push(&pilha, pow(a, b)); break;
            }
        }
        else if (ehFuncao(token)) {
            float a = pop(&pilha);
            push(&pilha, aplicarFuncao(token, a));
        }
        token = strtok(NULL, " ");
    }
    free(copia);
    return pilha.dados[0];
}