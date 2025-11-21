#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define MAX 256
#define PI 3.14159265358979323846

typedef struct {
    float dados[MAX];
    int topo;
} PilhaNum;

static void push(PilhaNum *p, float val) {
    if (!p) return;
    if (p->topo < MAX) p->dados[p->topo++] = val;
}

static float pop(PilhaNum *p) {
    if (!p) return 0.0f;
    if (p->topo > 0) return p->dados[--p->topo];
    return 0.0f;
}

/* utilitarios */
static int ehOperador(const char *token) {
    return (token && strlen(token) == 1 && strchr("+-*/%^", token[0])) ? 1 : 0;
}

static int ehFuncao(const char *token) {
    if (!token) return 0;
    return (strcmp(token, "sen") == 0 ||
            strcmp(token, "cos") == 0 ||
            strcmp(token, "tg") == 0 ||
            strcmp(token, "log") == 0 ||
            strcmp(token, "log10") == 0 ||
            strcmp(token, "sqrt") == 0) ? 1 : 0;
}

static int ehNumero(const char *token) {
    if (!token || strlen(token) == 0) return 0;
    int i = 0, temPonto = 0;
    if (token[0] == '-' && strlen(token) > 1) i = 1;
    for (; token[i] != '\0'; ++i) {
        if (token[i] == '.') {
            if (temPonto) return 0;
            temPonto = 1;
        } else if (!isdigit((unsigned char)token[i])) {
            return 0;
        }
    }
    return 1;
}

static float aplicarFuncao(const char *funcao, float valor) {
    if (strcmp(funcao, "sen") == 0) return sinf(valor * PI / 180.0f);
    if (strcmp(funcao, "cos") == 0) return cosf(valor * PI / 180.0f);
    if (strcmp(funcao, "tg") == 0) return tanf(valor * PI / 180.0f);
    if (strcmp(funcao, "log") == 0) return log10f(valor);
    if (strcmp(funcao, "log10") == 0) return log10f(valor);
    if (strcmp(funcao, "sqrt") == 0) return sqrtf(valor);
    return 0.0f;
}

static int precedencia(const char *op) {
    if (!op) return 0;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0;
}

/* Normaliza entrada infixa: insere espaços entre tokens, mantém nomes de funções unidos.
   Ex: "(log10(3)+4)*5" -> "( log10 ( 3 ) + 4 ) * 5" */
static char *normaliza(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    size_t cap = n * 3 + 1;
    char *tmp = malloc(cap);
    if (!tmp) return NULL;
    size_t k = 0;
    char last_non_space = '\0';
    
    for (size_t i = 0; i < n; ++i) {
        char c = s[i];
        if (c == ' ' || c == '\t') continue;
        
        /* detecta nome de função (sequência de letras) */
        if (isalpha((unsigned char)c)) {
            char word[MAX];
            int w = 0;
            while (i < n && isalpha((unsigned char)s[i])) {
                word[w++] = s[i++];
            }
            word[w] = '\0';
            i--; /* recua pois o loop vai incrementar */
            
            /* insere espaço antes da função se necessário */
            if (k > 0 && tmp[k-1] != ' ') tmp[k++] = ' ';
            /* copia a palavra (função) */
            strcpy(tmp + k, word);
            k += w;
            tmp[k++] = ' ';
            last_non_space = 'a'; /* marca como "não operador" */
            continue;
        }
        
        if (c == '-' && i + 1 < n && (isdigit((unsigned char)s[i+1]) || s[i+1] == '.')) {
            /* unary minus if at start or after '(', or after operator */
            if (last_non_space == '\0' || last_non_space == '(' || strchr("+-*/%^", last_non_space)) {
                tmp[k++] = '-';
                last_non_space = '-';
                continue;
            }
        }
        
        if (strchr("+-*/%^()", c)) {
            if (k > 0 && tmp[k-1] != ' ') tmp[k++] = ' ';
            tmp[k++] = c;
            tmp[k++] = ' ';
            last_non_space = c;
        } else {
            tmp[k++] = c;
            last_non_space = c;
        }
    }
    tmp[k] = '\0';
    
    /* collapse spaces */
    char *res = malloc(k + 1);
    if (!res) { free(tmp); return NULL; }
    size_t r = 0;
    int in_space = 0;
    for (size_t i = 0; i < k; ++i) {
        char c = tmp[i];
        if (c == ' ') {
            if (!in_space && r > 0) { res[r++] = ' '; in_space = 1; }
        } else {
            res[r++] = c;
            in_space = 0;
        }
    }
    if (r > 0 && res[r-1] == ' ') r--;
    res[r] = '\0';
    free(tmp);
    return res;
}

/* Detecta se expressão (tokens separados por espaço) está em notação pos-fixa. */
static int detectaPosFixa(const char *expr_norm) {
    if (!expr_norm) return 0;
    char *copia = strdup(expr_norm);
    if (!copia) return 0;
    char *token = strtok(copia, " ");
    PilhaNum pilha = { .topo = 0 };
    int valida = 1;
    
    while (token && valida) {
        if (ehNumero(token)) {
            push(&pilha, strtof(token, NULL));
        } else if (ehOperador(token)) {
            if (pilha.topo < 2) {
                valida = 0;
                break;
            }
            pop(&pilha);
            pop(&pilha);
            push(&pilha, 0.0f);
        } else if (ehFuncao(token)) {
            if (pilha.topo < 1) {
                valida = 0;
                break;
            }
            pop(&pilha);
            push(&pilha, 0.0f);
        } else if (strcmp(token, "(") == 0 || strcmp(token, ")") == 0) {
            valida = 0;
            break;
        } else {
            valida = 0;
            break;
        }
        token = strtok(NULL, " ");
    }
    
    int ehPos = (valida && pilha.topo == 1) ? 1 : 0;
    free(copia);
    return ehPos;
}

/* Converte infixa para posfixa */
static char *infixaParaPosfixa(char *infixa) {
    if (!infixa) return NULL;
    char *copia = strdup(infixa);
    if (!copia) return NULL;
    char *saida = malloc(strlen(infixa) * 2 + 1);
    if (!saida) { free(copia); return NULL; }
    char *pilhaOp[MAX];
    int topo = 0;
    int j = 0;
    char *token = strtok(copia, " ");
    while (token) {
        if (ehNumero(token)) {
            j += sprintf(saida + j, "%s ", token);
        } else if (ehFuncao(token)) {
            pilhaOp[topo++] = token;
        } else if (strcmp(token, "(") == 0) {
            pilhaOp[topo++] = token;
        } else if (strcmp(token, ")") == 0) {
            while (topo > 0 && strcmp(pilhaOp[topo-1], "(") != 0) {
                j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
            }
            if (topo > 0 && strcmp(pilhaOp[topo-1], "(") == 0) topo--;
            if (topo > 0 && ehFuncao(pilhaOp[topo-1])) {
                j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
            }
        } else if (ehOperador(token)) {
            int prec = precedencia(token);
            int right_assoc = (strcmp(token, "^") == 0);
            while (topo > 0 && strcmp(pilhaOp[topo-1], "(") != 0 && !ehFuncao(pilhaOp[topo-1])) {
                int precTop = precedencia(pilhaOp[topo-1]);
                if (precTop > prec || (precTop == prec && !right_assoc)) {
                    j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
                } else break;
            }
            pilhaOp[topo++] = token;
        } else {
            free(saida); free(copia); return NULL;
        }
        token = strtok(NULL, " ");
    }
    while (topo > 0) j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
    if (j > 0) saida[--j] = '\0'; else saida[0] = '\0';
    free(copia);
    return saida;
}

/* Converte pos-fixa -> infixa */
char *getFormaInFixa(char *Str) {
    if (!Str || strlen(Str) == 0) return NULL;
    char *copia = strdup(Str);
    if (!copia) return NULL;
    
    typedef struct {
        char *expr;
        int prec;
    } ExprComPrec;
    
    ExprComPrec pilha[MAX];
    int topo = 0;
    char *token = strtok(copia, " ");
    
    while (token) {
        if (ehNumero(token)) {
            pilha[topo].expr = strdup(token);
            if (!pilha[topo].expr) { 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            pilha[topo].prec = 999;
            topo++;
        } else if (ehOperador(token)) {
            if (topo < 2) { 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            
            ExprComPrec b_item = pilha[--topo];
            ExprComPrec a_item = pilha[--topo];
            char *a = a_item.expr;
            char *b = b_item.expr;
            int prec_a = a_item.prec;
            int prec_b = b_item.prec;
            int prec_op = precedencia(token);
            
            if (prec_a < prec_op) {
                char *sa = malloc(strlen(a) + 3);
                sprintf(sa, "(%s)", a);
                free(a);
                a = sa;
            }
            
            if (prec_b < prec_op || 
                (prec_b == prec_op && (strcmp(token, "-") == 0 || strcmp(token, "/") == 0 || strcmp(token, "%") == 0))) {
                char *sb = malloc(strlen(b) + 3);
                sprintf(sb, "(%s)", b);
                free(b);
                b = sb;
            }
            
            size_t len = strlen(a) + strlen(b) + 2;
            char *expr = malloc(len + 1);
            if (!expr) { 
                free(a); 
                free(b); 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            
            sprintf(expr, "%s%s%s", a, token, b);
            free(a);
            free(b);
            
            pilha[topo].expr = expr;
            pilha[topo].prec = prec_op;
            topo++;
        } else if (ehFuncao(token)) {
            if (topo < 1) { 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            
            ExprComPrec a_item = pilha[--topo];
            char *a = a_item.expr;
            size_t len = strlen(token) + strlen(a) + 3;
            char *expr = malloc(len + 1);
            if (!expr) { 
                free(a); 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            sprintf(expr, "%s(%s)", token, a);
            free(a);
            
            pilha[topo].expr = expr;
            pilha[topo].prec = 999;
            topo++;
        } else {
            while (topo>0) free(pilha[--topo].expr);
            free(copia);
            return NULL;
        }
        token = strtok(NULL, " ");
    }
    
    free(copia);
    if (topo != 1) { 
        while (topo>0) free(pilha[--topo].expr); 
        return NULL; 
    }
    
    char *res = strdup(pilha[0].expr);
    free(pilha[0].expr);
    return res;
}

/* Avalia pos-fixa */
float getValorPosFixa(char *StrPosFixa) {
    if (!StrPosFixa) return 0.0f;
    PilhaNum pilha = { .topo = 0 };
    char *copia = strdup(StrPosFixa);
    if (!copia) return 0.0f;
    char *token = strtok(copia, " ");
    while (token) {
        if (ehNumero(token)) {
            push(&pilha, strtof(token, NULL));
        } else if (ehOperador(token)) {
            float b = pop(&pilha);
            float a = pop(&pilha);
            switch (token[0]) {
                case '+': push(&pilha, a + b); break;
                case '-': push(&pilha, a - b); break;
                case '*': push(&pilha, a * b); break;
                case '/': push(&pilha, (b != 0.0f) ? a / b : 0.0f); break;
                case '%': push(&pilha, (int)a % (int)b); break;
                case '^': push(&pilha, powf(a, b)); break;
            }
        } else if (ehFuncao(token)) {
            float a = pop(&pilha);
            push(&pilha, aplicarFuncao(token, a));
        }
        token = strtok(NULL, " ");
    }
    free(copia);
    return (pilha.topo > 0) ? pilha.dados[0] : 0.0f;
}

int processar_expressao(const char *input, char **converted_out, float *value_out, int *isPosOut) {
    if (!input || !converted_out || !value_out || !isPosOut) return -1;
    *converted_out = NULL;
    *value_out = 0.0f;
    *isPosOut = 0;

    int ehPosDirect = detectaPosFixa(input);
    
    char *norm = NULL;
    if (!ehPosDirect) {
        norm = normaliza(input);
        if (!norm) return -1;
    } else {
        norm = strdup(input);
        if (!norm) return -1;
    }

    int ehPos = detectaPosFixa(norm);
    *isPosOut = ehPos;

    char *pos = NULL;
    char *conv = NULL;

    if (ehPos) {
        pos = norm;
        conv = getFormaInFixa(pos);
        if (!conv) {
            *value_out = getValorPosFixa(pos);
            *converted_out = NULL;
            free(pos);
            return 0;
        }
        *value_out = getValorPosFixa(pos);
        *converted_out = conv;
        free(pos);
        return 0;
    } else {
        pos = infixaParaPosfixa(norm);
        if (!pos) {
            free(norm);
            return -1;
        }
        *value_out = getValorPosFixa(pos);
        conv = strdup(pos);
        if (!conv) {
            free(pos);
            free(norm);
            return -1;
        }
        *converted_out = conv;
        free(pos);
        free(norm);
        return 0;
    }
}