#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#define MAX 256
#define PI 3.14159265358979323846

/*
  Resumo geral:
  - Esse arquivo implementa as funções que manipulam expressões matemáticas.
  - Suporta: notação infixa, notação posfixa,
    e funções como sen, cos, log, sqrt.
  - Principais funcionalidades:
    1) detectar se uma string é posfixa ou infixa;
    2) normalizar infixas (inserir espaços, reconhecer funções, tratar sinal unário);
    3) converter infixa -> posfixa;
    4) converter posfixa -> infixa;
    5) Realizar os cálculos para solver as equações.
*/

/* Pilha simples para números usada na avaliação de posfixa */
typedef struct {
    float dados[MAX];
    int topo;
} PilhaNum;

/* push: empilha um valor float.
   Uso: durante a avaliação de Reverse Polish Notation ou simulação para detectar posfixa. */
static void push(PilhaNum *p, float val) {
    if (!p) return;
    if (p->topo < MAX) p->dados[p->topo++] = val;
}

/* pop: desempilha e retorna o topo; se vazia, retorna 0 */
static float pop(PilhaNum *p) {
    if (!p) return 0.0f;
    if (p->topo > 0) return p->dados[--p->topo];
    return 0.0f;
}

/* UTILITÁRIOS */

/* ehOperador: reconhece operadores básicos */
static int ehOperador(const char *token) {
    return (token && strlen(token) == 1 && strchr("+-*/%^", token[0])) ? 1 : 0;
}

/* ehFuncao: reconhece nomes de funções suportadas */
static int ehFuncao(const char *token) {
    if (!token) return 0;
    return (strcmp(token, "sen") == 0 ||
            strcmp(token, "cos") == 0 ||
            strcmp(token, "tg") == 0 ||
            strcmp(token, "log") == 0 ||
            strcmp(token, "log10") == 0 ||
            strcmp(token, "sqrt") == 0) ? 1 : 0;
}

/* ehNumero: verifica se o token é um número válido */
static int ehNumero(const char *token) {
    if (!token || strlen(token) == 0) return 0;
    int i = 0, temPonto = 0;
    if (token[0] == '-' && strlen(token) > 1) i = 1; /* permite números negativos */
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

/* aplicarFuncao: aplica a função ao valor (sen/cos/tg usam graus) */
static float aplicarFuncao(const char *funcao, float valor) {
    if (strcmp(funcao, "sen") == 0) return sinf(valor * PI / 180.0f);
    if (strcmp(funcao, "cos") == 0) return cosf(valor * PI / 180.0f);
    if (strcmp(funcao, "tg") == 0) return tanf(valor * PI / 180.0f);
    if (strcmp(funcao, "log") == 0) return log10f(valor);    /* log base 10 */
    if (strcmp(funcao, "log10") == 0) return log10f(valor);
    if (strcmp(funcao, "sqrt") == 0) return sqrtf(valor);
    return 0.0f;
}

/* precedencia: retorna valor numérico de precedência de operadores (maior => aplicado antes) */
static int precedencia(const char *op) {
    if (!op) return 0;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "^") == 0) return 3;
    return 0;
}

/* NORMALIZAÇÃO
   transforma uma string infixa em tokens separados por espaços,
   mantendo nomes de funções unidos e tratando sinal '-' unário.
   Exemplos:
   essa etapa facilita o parsing derivado, trabalha com tokens simples em vez de analisar caracter por caracter depois. */
static char *normaliza(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s);
    size_t cap = n * 3 + 1; /* "espaço" suficiente para inserir espaços */
    char *tmp = malloc(cap);
    if (!tmp) return NULL;
    size_t k = 0;
    char last_non_space = '\0';
    
    for (size_t i = 0; i < n; ++i) {
        char c = s[i];
        if (c == ' ' || c == '\t') continue; /* ignora espaços originais */
        
        /* Reconhece nomes de função: sequência de letras.
           Guarda a palavra inteira como um token único. */
        if (isalpha((unsigned char)c)) {
            char word[MAX];
            int w = 0;
            while (i < n && isalpha((unsigned char)s[i])) {
                word[w++] = s[i++];
            }
            word[w] = '\0';
            i--; /* recua um passo pois o for irá incrementar */
            
            if (k > 0 && tmp[k-1] != ' ') tmp[k++] = ' ';
            strcpy(tmp + k, word);
            k += w;
            tmp[k++] = ' ';
            last_non_space = 'a'; /* qualquer letra: marca como token não operador */
            continue;
        }
        
        /* Detecta '-' unário: quando aparece no começo, após '(' ou depois de outro operador */
        if (c == '-' && i + 1 < n && (isdigit((unsigned char)s[i+1]) || s[i+1] == '.')) {
            if (last_non_space == '\0' || last_non_space == '(' || strchr("+-*/%^", last_non_space)) {
                tmp[k++] = '-';
                last_non_space = '-';
                continue;
            }
        }
        
        /* Coloca espaços ao redor para tokenizar facilmente */
        if (strchr("+-*/%^()", c)) {
            if (k > 0 && tmp[k-1] != ' ') tmp[k++] = ' ';
            tmp[k++] = c;
            tmp[k++] = ' ';
            last_non_space = c;
        } else {
            /* dígitos e outros caracteres (parte de número) */
            tmp[k++] = c;
            last_non_space = c;
        }
    }
    tmp[k] = '\0';
    
    /* compila múltiplos espaços em um só e faz trim final.
       Resultado: tokens separados por exatamente um espaço. */
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

/* DETECÇÃO DE POSFIXA
    "simula" a avaliação em Reverse Polish Notation:
   - se durante o scan todos os tokens fazem sentido (números empilham, operadores consomem operandos)
     e no final sobra exatamente 1 elemento, é considerado posfixa.
   - se aparecem parênteses ou tokens inválidos, é infixa.
   essa é uma técnica que evita confundir sequências infixas que também têm operandos = operadores + 1 mas não são Reverse Polish Notation. */
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
            /* operador binário: precisa de 2 operandos na pilha */
            if (pilha.topo < 2) {
                valida = 0;
                break;
            }
            pop(&pilha);
            pop(&pilha);
            push(&pilha, 0.0f); /* empilha resultado */
        } else if (ehFuncao(token)) {
            /* função: precisa de 1 operando */
            if (pilha.topo < 1) {
                valida = 0;
                break;
            }
            pop(&pilha);
            push(&pilha, 0.0f);
        } else if (strcmp(token, "(") == 0 || strcmp(token, ")") == 0) {
            /* parênteses são um sinal claro de notação infixa */
            valida = 0;
            break;
        } else {
            /* token desconhecido => não é posfixa */
            valida = 0;
            break;
        }
        token = strtok(NULL, " ");
    }
    
    int ehPos = (valida && pilha.topo == 1) ? 1 : 0;
    free(copia);
    return ehPos;
}

/* INFIXA -> POSFIXA:
   - números: vão direto para saída;
   - funções/operadores: geridos por pilha de operadores com ordens estabelecidas;
   - parênteses: empurram/pop na pilha.
   essa função espera os tokens separados por espaço (por isso normaliza antes). */
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
            j += sprintf(saida + j, "%s ", token); /* número -> saída */
        } else if (ehFuncao(token)) {
            pilhaOp[topo++] = token; /* função -> pilha (será saída quando fechar parêntese) */
        } else if (strcmp(token, "(") == 0) {
            pilhaOp[topo++] = token;
        } else if (strcmp(token, ")") == 0) {
            /* descarrega até encontrar '(' */
            while (topo > 0 && strcmp(pilhaOp[topo-1], "(") != 0) {
                j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
            }
            if (topo > 0 && strcmp(pilhaOp[topo-1], "(") == 0) topo--;
            /* se antes havia uma função, ela sai agora */
            if (topo > 0 && ehFuncao(pilhaOp[topo-1])) {
                j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
            }
        } else if (ehOperador(token)) {
            int prec = precedencia(token);
            int right_assoc = (strcmp(token, "^") == 0); /* ^ é associativo à direita */
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
    /* esvazia pilha de operadores restantes */
    while (topo > 0) j += sprintf(saida + j, "%s ", pilhaOp[--topo]);
    if (j > 0) saida[--j] = '\0'; else saida[0] = '\0';
    free(copia);
    return saida;
}

/* POSFIXA -> INFIXA (reconstrução legível)
   - mantém na pilha itens do tipo {expr_string, precedencia_da_raiz}
   - ao aplicar um operador, decide se precisa de parênteses nos operandos
     com base na precedência para manter a ordem correta.
   Observação: números/funções têm "prioridade" para evitar parênteses desnecessários.
*/
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
            /* número -> empilha como expressão atômica */
            pilha[topo].expr = strdup(token);
            if (!pilha[topo].expr) { 
                while (topo>0) free(pilha[--topo].expr); 
                free(copia); 
                return NULL; 
            }
            pilha[topo].prec = 999; /* valor grande: não precisa de parênteses por padrão */
            topo++;
        } else if (ehOperador(token)) {
            /* operador binário -> pop b (direita) e a (esquerda) e montar "a op b" */
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
            
            /* Se o lado esquerdo tem precedência menor que o operador atual, precisa de parênteses */
            if (prec_a < prec_op) {
                char *sa = malloc(strlen(a) + 3);
                sprintf(sa, "(%s)", a);
                free(a);
                a = sa;
            }
            
            /* Lado direito: além do caso prec_b < prec_op, precisa de parênteses quando prec_b == prec_op e o operador não é associativo à direita */
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
            
            /* monta "a op b" (token é operador de 1 caractere) */
            sprintf(expr, "%s%s%s", a, token, b);
            free(a);
            free(b);
            
            pilha[topo].expr = expr;
            pilha[topo].prec = prec_op; /* raiz agora tem precedência do operador */
            topo++;
        } else if (ehFuncao(token)) {
            /* função unária: pop argumento e gerar func(arg) */
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
            pilha[topo].prec = 999; /* função considera-se de alta precedência */
            topo++;
        } else {
            /* token desconhecido -> erro */
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

/* AVALIAÇÃO DE POSFIXA (interpreta tokens da esquerda para direita)
   - números: empilha
   - operadores: pop b, pop a, push (a op b)
   - funções: pop a, push func(a)
   Retorna o valor final (ou 0.0 caso de erro). */
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

/* Função pública que comanda tudo:
   - input: string raw (pode ser infixa ou posfixa)
   - converted_out: string alocada com a conversão
   - value_out: valor numérico calculado
   - isPosOut: indica se a entrada era posfixa (1) ou infixa (0)
    1) Detecta se já é Reverse Polish Notation direto; se não, normaliza (infixa).
    2) Se posfixa: converte para infixa e avalia. Se infixa: converte para posfixa e avalia posfixa gerada.
*/
int processar_expressao(const char *input, char **converted_out, float *value_out, int *isPosOut) {
    if (!input || !converted_out || !value_out || !isPosOut) return -1;
    *converted_out = NULL;
    *value_out = 0.0f;
    *isPosOut = 0;

    int ehPosDirect = detectaPosFixa(input);
    
    char *norm = NULL;
    if (!ehPosDirect) {
        /* entrada parece infixa -> normaliza para tokens */
        norm = normaliza(input);
        if (!norm) return -1;
    } else {
        /* entrada já Reverse Polish Notation -> apenas duplica para trabalhar com cópia modificável */
        norm = strdup(input);
        if (!norm) return -1;
    }

    int ehPos = detectaPosFixa(norm);
    *isPosOut = ehPos;

    char *pos = NULL;
    char *conv = NULL;

    if (ehPos) {
        /* entrada é posfixa: converte para infixa (legível) e avalia */
        pos = norm;
        conv = getFormaInFixa(pos);
        if (!conv) {
            /* se não conseguiu converter para infixa legível, avalia mesmo assim */
            *value_out = getValorPosFixa(pos);
            *converted_out = NULL;
            free(pos);
            return 0;
        }
        *value_out = getValorPosFixa(pos);
        *converted_out = conv; /* retorno: forma infixa legível */
        free(pos);
        return 0;
    } else {
        /* entrada é infixa: converte para posfixa e avalia */
        pos = infixaParaPosfixa(norm);
        if (!pos) {
            free(norm);
            return -1;
        }
        *value_out = getValorPosFixa(pos);
        conv = strdup(pos); /* conv tem a posfixa gerada */
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
