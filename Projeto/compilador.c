/*
Matheus Chediac Rodrigues       10417490
Lucas Monteiro Soares           10417881
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ID_LEN 15 // Definimos o tamanho máximo de ID aqui no começo para ser mais fácil de mudar posteriormente
#define PRIME_NUMER 211 // Tamanho da Tabela Hash

// Declaração da função externa hashMack
int hashMack(char *s);

// Tipos de tokens
typedef enum {
    IDENTIFIER, 
    CONSTINT, 
    CONSTCHAR,

    // Palavras reservadas
    IF, 
    WHILE, 
    PROGRAM, 
    DIV, 
    OR, 
    AND, 
    NOT,
    READ, 
    WRITE, 
    VAR, 
    BEGIN, 
    END, 
    TRUE, 
    FALSE,
    CHAR, 
    INTEGER, 
    BOOLEAN, 
    THEN, 
    ELSE, 
    DO,

    // Símbolos e operadores
    ATRIBUICAO, 
    DOIS_PONTOS, 
    PONTO_VIRGULA, 
    VIRGULA, 
    PONTO,
    MAIS, 
    MENOS, 
    ASTERISCO, 
    ABRE_PAR, 
    FECHA_PAR,
    DIFERENTE, 
    MENOR_IGUAL, 
    MAIOR_IGUAL, 
    IGUAL, 
    MENOR, 
    MAIOR,

    // Utilidades
    COMENTARIO, FIM_ARQUIVO, ERRO
} TAtomo;

// Struct para armazenar info do token
typedef struct {
    TAtomo atomo;
    int linha;
    union {
        int numero;    
        char id[16];    
        char ch;    
    } atributo;
} TInfoAtomo;

// Struct da Tabela de Símbolos 
typedef struct _TNo {
    char ID[16];
    int endereco;
    struct _TNo *prox;
} TNo;

typedef struct {
    TNo *entradas[PRIME_NUMER];
} TTabelaSimbolos;

// Variáveis do Scanner e do Parser/Semântico
char *buffer = NULL;
long buf_len = 0;
long pos = 0;
int numero_linha = 1;
TInfoAtomo tokenAtual;
TTabelaSimbolos tabelaSimbolos;
int proxEndereco = 0;
int contadorRotulo = 0;

// Protótipos das funções do Parser/Semântico
void expressao(void);
void comando(void);
void erro_lexico(const char *msg);
void erro_sintatico(TAtomo esp);
void erro_semantico(const char *msg);
const char *lexeme_curto(TAtomo a);
// Pré definição de funções para evitar problemas

// Inicializa a tabela de símbolos (Hash Table)
void inicializar_tabela() {
    for (int i = 0; i < PRIME_NUMER; i++) {
        tabelaSimbolos.entradas[i] = NULL;
    }
}

// Insere variável na tabela de símbolos (com verificação de duplicidade)
void inserir_tabela_simbolos(char *id) {
    int entrada = hashMack(id);
    
    // Verifica se já existe (erro semântico)
    TNo *atual = tabelaSimbolos.entradas[entrada];
    while (atual != NULL) {
        if (strcmp(atual->ID, id) == 0) {
            char msg[100];
            sprintf(msg, "variavel '%s' ja declarada", id);
            erro_semantico(msg);
        }
        atual = atual->prox;
    }
    
    // Insere novo nó
    TNo *novo = (TNo *)malloc(sizeof(TNo));
    if (!novo) {
        fprintf(stderr, "Erro de alocacao de memoria\n");
        exit(1);
    }
    strncpy(novo->ID, id, 15);
    novo->ID[15] = '\0';
    novo->endereco = proxEndereco++;
    novo->prox = tabelaSimbolos.entradas[entrada];
    tabelaSimbolos.entradas[entrada] = novo;
}

// Busca variável na tabela de símbolos e retorna o endereço
int busca_tabela_simbolos(char *id) {
    int entrada = hashMack(id);
    TNo *atual = tabelaSimbolos.entradas[entrada];
    
    while (atual != NULL) {
        if (strcmp(atual->ID, id) == 0) {
            return atual->endereco;
        }
        atual = atual->prox;
    }
    
    // Não encontrou - erro semântico
    char msg[100];
    sprintf(msg, "variavel '%s' nao declarada", id);
    erro_semantico(msg);
    return -1; // Nunca chega aqui
}

// Imprime a tabela de símbolos para debug/saída
void imprimir_tabela_simbolos() {
    printf("\nTABELA DE SIMBOLOS\n");
    for (int i = 0; i < PRIME_NUMER; i++) {
        TNo *atual = tabelaSimbolos.entradas[i];
        while (atual != NULL) {
            printf("Entrada Tabela Simbolos: [%d] => %s | Endereco: %d\n", 
                   i, atual->ID, atual->endereco);
            atual = atual->prox;
        }
    }
}

// Libera memória alocada pela tabela de símbolos
void liberar_tabela_simbolos() {
    for (int i = 0; i < PRIME_NUMER; i++) {
        TNo *atual = tabelaSimbolos.entradas[i];
        while (atual != NULL) {
            TNo *temp = atual;
            atual = atual->prox;
            free(temp);
        }
    }
}

// Retorna o próximo rótulo único (L1, L2, ...)
int proximo_rotulo() {
    return ++contadorRotulo;
}

// Retorna o lexeme curto para impressão em erro sintático
const char *lexeme_curto(TAtomo a) {
    switch (a) {
        case ATRIBUICAO: return ":=";
        case DOIS_PONTOS: return ":";
        case PONTO_VIRGULA: return ";";
        case VIRGULA: return ",";
        case PONTO: return ".";
        case ABRE_PAR: return "(";
        case FECHA_PAR: return ")";
        case DIFERENTE: return "<>";
        case MENOR_IGUAL: return "<=";
        case MAIOR_IGUAL: return ">=";
        case IGUAL: return "=";
        case MENOR: return "<";
        case MAIOR: return ">";
        case MAIS: return "+";
        case MENOS: return "-";
        case ASTERISCO: return "*";
        case PROGRAM: return "program";
        case VAR: return "var";
        case BEGIN: return "begin";
        case END: return "end";
        case READ: return "read";
        case WRITE: return "write";
        case IF: return "if";
        case THEN: return "then";
        case ELSE: return "else";
        case WHILE: return "while";
        case DO: return "do";
        case DIV: return "div";
        case OR: return "or";
        case AND: return "and";
        case NOT: return "not";
        case TRUE: return "true";
        case FALSE: return "false";
        case CHAR: return "char";
        case INTEGER: return "integer";
        case BOOLEAN: return "boolean";
        case IDENTIFIER: return "identifier";
        case CONSTINT: return "constint";
        case CONSTCHAR: return "constchar";
        case FIM_ARQUIVO: return "EOF";
        default: return "ERRO";
    }
}

// Mostra erro léxico e encerra
void erro_lexico(const char *msg) {
    fprintf(stderr, "# %d:erro lexico: %s\n", numero_linha, msg);
    exit(1);
}

// Mostra erro sintático e encerra
void erro_sintatico(TAtomo esp) {
    fprintf(stderr, "# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
            tokenAtual.linha, lexeme_curto(esp), lexeme_curto(tokenAtual.atomo));
    exit(1);
}

// Mostra erro semântico e encerra
void erro_semantico(const char *msg) {
    fprintf(stderr, "# %d:erro semantico: %s\n", tokenAtual.linha, msg);
    exit(1);
}

// Função que anda no buffer (conta linha também)
void avancar(int n) {
    for (int i = 0; i < n && pos < buf_len; ++i) {
        if (buffer[pos] == '\n') 
            numero_linha++;
        pos++;
    }
}

// Tabela de nomes (palavras reservadas)
TAtomo palavra_reservada(const char *lex) {
    if (strcmp(lex,"if")==0) return IF;
    if (strcmp(lex,"while")==0) return WHILE;
    if (strcmp(lex,"program")==0) return PROGRAM;
    if (strcmp(lex,"div")==0) return DIV;
    if (strcmp(lex,"or")==0) return OR;
    if (strcmp(lex,"and")==0) return AND;
    if (strcmp(lex,"not")==0) return NOT;
    if (strcmp(lex,"read")==0) return READ;
    if (strcmp(lex,"write")==0) return WRITE;
    if (strcmp(lex,"var")==0) return VAR;
    if (strcmp(lex,"begin")==0) return BEGIN;
    if (strcmp(lex,"end")==0) return END;
    if (strcmp(lex,"true")==0) return TRUE;
    if (strcmp(lex,"false")==0) return FALSE;
    if (strcmp(lex,"char")==0) return CHAR;
    if (strcmp(lex,"integer")==0) return INTEGER;
    if (strcmp(lex,"boolean")==0) return BOOLEAN;
    if (strcmp(lex,"then")==0) return THEN;
    if (strcmp(lex,"else")==0) return ELSE;
    if (strcmp(lex,"do")==0) return DO;
    return IDENTIFIER;
}

// Função que lê próximo átomo do buffer
TInfoAtomo obter_atomo(void) {
    TInfoAtomo t; 
    t.atomo = ERRO; 
    t.linha = numero_linha;

    // Ignora espaços, quebras de linha, tabulação...
    while (pos < buf_len && isspace((unsigned char)buffer[pos])) 
        avancar(1);

    if (pos >= buf_len || buffer[pos] == '\0') {
        t.atomo = FIM_ARQUIVO;
        t.linha = numero_linha;
        return t;
    }

    // Trata comentário
    if (buffer[pos] == '(') {
        char prox = (pos + 1 < buf_len ? buffer[pos + 1] : '\0');
        if (prox == '*') {
            int l0 = numero_linha;
            avancar(2);
            while (!(buffer[pos] == '*' && (pos + 1 < buf_len && buffer[pos + 1] == ')'))) {
                if (pos >= buf_len || buffer[pos] == '\0') 
                    erro_lexico("comentario nao terminado");
                avancar(1);
            }
            avancar(2);
            t.atomo = COMENTARIO; 
            t.linha = l0; 
            return t;
        }
    }

    t.linha = numero_linha;

    // Identificadores
    if (isalpha((unsigned char)buffer[pos]) || buffer[pos] == '_') {
        char lex[MAX_ID_LEN + 1]; 
        int i = 0;
        while (isalnum((unsigned char)buffer[pos]) || buffer[pos] == '_') {
            if (i < MAX_ID_LEN) 
                lex[i++] = buffer[pos];
            else {
                avancar(1);
                while (isalnum((unsigned char)buffer[pos]) || buffer[pos] == '_') 
                    avancar(1);
                erro_lexico("identifier com mais de 15 caracteres");
            }
            avancar(1);
        }
        lex[i] = '\0';
        // Copia no campo de 16 bytes (15 + '\0')
        strncpy(t.atributo.id, lex, MAX_ID_LEN);
        t.atributo.id[MAX_ID_LEN] = '\0';
        t.atomo = palavra_reservada(lex);
        return t;
    }

    // Números inteiros (com notação d)
    if (isdigit((unsigned char)buffer[pos])) {
        long long val = 0;
        while (isdigit((unsigned char)buffer[pos])) {
            val = val * 10 + (buffer[pos] - '0');
            avancar(1);
        }
        if (buffer[pos] == 'd') {
            avancar(1);
            if (buffer[pos] == '+') avancar(1);
            if (!isdigit((unsigned char)buffer[pos])) 
                erro_lexico("expoente em constint mal formado");
            int e = 0; 
            while (isdigit((unsigned char)buffer[pos])) { 
                e = e * 10 + (buffer[pos] - '0'); 
                avancar(1); 
            }
            for (int k = 0; k < e; ++k) 
                val *= 10;
        }
        t.atomo = CONSTINT; 
        t.atributo.numero = (int)val; 
        return t;
    }

    // Constante char
    if (buffer[pos] == '\'') {
        avancar(1);
        if (pos >= buf_len) erro_lexico("constchar mal formado");
        char c = buffer[pos]; 
        avancar(1);
        if (buffer[pos] != '\'') erro_lexico("constchar mal formado");
        avancar(1);
        t.atomo = CONSTCHAR; 
        t.atributo.ch = c; 
        return t;
    }

    // Operadores e símbolos
    switch (buffer[pos]) {
        case ':': 
            if ((pos + 1 < buf_len) && buffer[pos + 1] == '=') { 
                avancar(2);
                t.atomo = ATRIBUICAO;
            } else { 
                avancar(1);
                t.atomo = DOIS_PONTOS;
            } 
            return t;
        case ';': avancar(1); t.atomo = PONTO_VIRGULA; return t;
        case ',': avancar(1); t.atomo = VIRGULA; return t;
        case '.': avancar(1); t.atomo = PONTO; return t;
        case '(': avancar(1); t.atomo = ABRE_PAR; return t;
        case ')': avancar(1); t.atomo = FECHA_PAR; return t;
        case '+': avancar(1); t.atomo = MAIS; return t;
        case '-': avancar(1); t.atomo = MENOS; return t;
        case '*': avancar(1); t.atomo = ASTERISCO; return t;
        case '<':
            if ((pos + 1 < buf_len) && buffer[pos + 1] == '>') {
                avancar(2); t.atomo = DIFERENTE; return t;
            }
            if ((pos + 1 < buf_len) && buffer[pos + 1] == '=') {
                avancar(2); t.atomo = MENOR_IGUAL; return t;
            }
            avancar(1); t.atomo = MENOR; return t;
        case '>':
            if ((pos + 1 < buf_len) && buffer[pos + 1] == '=') {
                avancar(2); t.atomo = MAIOR_IGUAL; return t;
            }
            avancar(1); t.atomo = MAIOR; return t;
        case '=': avancar(1); t.atomo = IGUAL; return t;
        default: erro_lexico("simbolo desconhecido");
    }

    return t;
}

// Carrega o arquivo de entrada na memória (buffer)
void carregar_de_stream(FILE *f) {
    size_t cap = 4096, n = 0;
    buffer = (char *)malloc(cap + 1);
    if (!buffer) {
        fprintf(stderr, "Erro de alocacao de memoria\n");
        exit(1);
    }
    while (1) {
        size_t r = fread(buffer + n, 1, cap - n, f);
        n += r;
        if (r == 0) break;
        if (n == cap) {
            cap *= 2;
            char *novo = (char *)realloc(buffer, cap + 1);
            if (!novo) {
                fprintf(stderr, "Erro de alocacao de memoria\n");
                free(buffer);
                exit(1);
            }
            buffer = novo;
        }
    }
    buffer[n] = '\0';
    buf_len = (long)n;
    pos = 0;
    numero_linha = 1;
}

// Abre o arquivo de entrada e chama a função de carregamento
void carregar_arquivo(const char *nome) {
    FILE *f = fopen(nome, "rb");
    if (!f) {
        fprintf(stderr, "Erro ao abrir arquivo %s\n", nome);
        exit(1);
    }
    carregar_de_stream(f);
    fclose(f);
}

// Lê próximo token
void proximo() { 
    tokenAtual = obter_atomo(); 
}

// Descarta comentários
void descarta_comentarios() { 
    while (tokenAtual.atomo == COMENTARIO) { 
        proximo(); 
    } 
}

// Consome token esperado
void consome(TAtomo esp) {
    descarta_comentarios();
    if (tokenAtual.atomo == esp) {
        proximo();
        descarta_comentarios();
    } else {
        erro_sintatico(esp);
    }
}

// As próximas funções implementam a gramática recursiva
// Cada não-terminal da gramática tem sua função

// Analisa um fator (ID, constante, expressão entre parênteses)
void fator() {
    switch (tokenAtual.atomo) {
        case IDENTIFIER:
            // Análise Semântica e Geração de Código
            {
                int endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
                printf("\tCRVL %d\n", endereco);
            }
            consome(IDENTIFIER);
            break;
        case CONSTINT:
            // Geração de Código
            printf("\tCRCT %d\n", tokenAtual.atributo.numero);
            consome(CONSTINT);
            break;
        case ABRE_PAR:
            consome(ABRE_PAR);
            expressao();
            consome(FECHA_PAR);
            break;
        default:
            erro_sintatico(IDENTIFIER);
    }
}

// Analisa um termo (sequência de fatores multiplicados ou divididos)
void termo() {
    fator();
    while (tokenAtual.atomo == ASTERISCO || tokenAtual.atomo == DIV) {
        if (tokenAtual.atomo == ASTERISCO) {
            consome(ASTERISCO);
            fator();
            // Geração de Código
            printf("\tMULT\n");
        } else {
            consome(DIV);
            fator();
            // Geração de Código
            printf("\tDIVI\n");
        }
    }
}

// Analisa uma expressão simples (sequência de termos somados ou subtraídos)
void exp_simples() {
    termo();
    while (tokenAtual.atomo == MAIS || tokenAtual.atomo == MENOS) {
        if (tokenAtual.atomo == MAIS) {
            consome(MAIS);
            termo();
            // Geração de Código
            printf("\tSOMA\n");
        } else {
            consome(MENOS);
            termo();
            // Geração de Código
            printf("\tSUBT\n");
        }
    }
}

// Analisa um operador relacional (>, <, =, etc.)
void op_rel() {
    switch (tokenAtual.atomo) {
        case DIFERENTE: consome(DIFERENTE); break;
        case MENOR: consome(MENOR); break;
        case MENOR_IGUAL: consome(MENOR_IGUAL); break;
        case MAIOR_IGUAL: consome(MAIOR_IGUAL); break;
        case MAIOR: consome(MAIOR); break;
        case IGUAL: consome(IGUAL); break;
        default: erro_sintatico(IGUAL);
    }
}

// Analisa uma expressão (expressão simples op_relacional expressão simples, ou apenas expressão simples)
void expressao() {
    exp_simples();
    if (tokenAtual.atomo == DIFERENTE || tokenAtual.atomo == MENOR || 
        tokenAtual.atomo == MENOR_IGUAL || tokenAtual.atomo == MAIOR || 
        tokenAtual.atomo == MAIOR_IGUAL || tokenAtual.atomo == IGUAL) {
        
        TAtomo op = tokenAtual.atomo; // Salva o operador para geração de código
        op_rel();
        exp_simples();
        
        // Geração de instrução de comparação
        switch (op) {
            case IGUAL: printf("\tCMIG\n"); break;
            case DIFERENTE: printf("\tCMDG\n"); break;
            case MENOR: printf("\tCMME\n"); break;
            case MENOR_IGUAL: printf("\tCMEG\n"); break;
            case MAIOR: printf("\tCMMA\n"); break;
            case MAIOR_IGUAL: printf("\tCMAG\n"); break;
            default: break;
        }
    }
}

// Analisa o comando de atribuição
void atribuicao() { 
    // Análise Semântica e Geração de Código
    int endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
    consome(IDENTIFIER); 
    consome(ATRIBUICAO); 
    expressao();
    printf("\tARMZ %d\n", endereco);
}

// Analisa o comando de leitura
void leitura() {
    consome(READ); 
    consome(ABRE_PAR); 
    
    // Análise Semântica e Geração de Código (primeiro ID)
    int endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
    consome(IDENTIFIER);
    printf("\tLEIT\n");
    printf("\tARMZ %d\n", endereco);
    
    while (tokenAtual.atomo == VIRGULA) {
        consome(VIRGULA);
        // Análise Semântica e Geração de Código (IDs seguintes)
        endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
        consome(IDENTIFIER);
        printf("\tLEIT\n");
        printf("\tARMZ %d\n", endereco);
    }
    consome(FECHA_PAR);
}

// Analisa o comando de escrita
void escrita() {
    consome(WRITE); 
    consome(ABRE_PAR); 
    
    // Análise Semântica e Geração de Código (primeiro ID)
    int endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
    consome(IDENTIFIER);
    printf("\tCRVL %d\n", endereco);
    printf("\tIMPR\n");
    
    while (tokenAtual.atomo == VIRGULA) {
        consome(VIRGULA);
        // Análise Semântica e Geração de Código (IDs seguintes)
        endereco = busca_tabela_simbolos(tokenAtual.atributo.id);
        consome(IDENTIFIER);
        printf("\tCRVL %d\n", endereco);
        printf("\tIMPR\n");
    }
    consome(FECHA_PAR);
}

// Analisa o comando condicional (IF-THEN-ELSE)
void condicional() {
    int L1 = proximo_rotulo();
    int L2 = proximo_rotulo();
    
    consome(IF); 
    expressao(); 
    consome(THEN);
    
    // Geração de Código: Desvio em caso de Falso
    printf("\tDSVF L%d\n", L1);
    
    comando();
    
    // Geração de Código: Salto para o fim do IF/ELSE
    printf("\tDSVS L%d\n", L2);
    printf("L%d:\tNADA\n", L1);
    
    if (tokenAtual.atomo == ELSE) {
        consome(ELSE);
        comando();
    }
    
    // Rótulo do fim do IF/ELSE
    printf("L%d:\tNADA\n", L2);
}

// Analisa o comando de repetição (WHILE-DO)
void repeticao() {
    int L1 = proximo_rotulo(); // Início do loop (avaliação)
    int L2 = proximo_rotulo(); // Fim do loop
    
    consome(WHILE); 
    
    // Rótulo do teste
    printf("L%d:\tNADA\n", L1);
    
    expressao(); 
    consome(DO); 
    
    // Geração de Código: Desvio para o fim se Falso
    printf("\tDSVF L%d\n", L2);
    
    comando();
    
    // Geração de Código: Volta para o teste
    printf("\tDSVS L%d\n", L1);
    
    // Rótulo do fim do loop
    printf("L%d:\tNADA\n", L2);
}

// Analisa um comando (atribuição, leitura, escrita, if, while, begin-end)
void comando() {
    switch (tokenAtual.atomo) {
        case IDENTIFIER:
            atribuicao();
            break;
        case READ:
            leitura();
            break;
        case WRITE:
            escrita();
            break;
        case IF:
            condicional();
            break;
        case WHILE:
            repeticao();
            break;
        case BEGIN:
            consome(BEGIN);
            comando();
            while (tokenAtual.atomo == PONTO_VIRGULA) {
                consome(PONTO_VIRGULA);
                comando();
            }
            consome(END);
            break;
        default:
            erro_sintatico(IDENTIFIER);
    }
}

// Analisa o tipo de dado (CHAR, INTEGER, BOOLEAN)
void tipo() {
    if (tokenAtual.atomo == CHAR) {
        consome(CHAR);
        return;
    }
    if (tokenAtual.atomo == INTEGER) {
        consome(INTEGER);
        return;
    }
    if (tokenAtual.atomo == BOOLEAN) {
        consome(BOOLEAN);
        return;
    }
    erro_sintatico(CHAR);
}

// Analisa uma linha ou bloco de declaração de variáveis
void declaracao_variaveis() {
    // Análise Semântica: Insere primeiro ID
    inserir_tabela_simbolos(tokenAtual.atributo.id);
    consome(IDENTIFIER);
    
    while (tokenAtual.atomo == VIRGULA) {
        consome(VIRGULA);
        // Análise Semântica: Insere IDs seguintes
        inserir_tabela_simbolos(tokenAtual.atributo.id);
        consome(IDENTIFIER);
    }
    consome(DOIS_PONTOS);
    tipo();
}

// Analisa o bloco principal do programa (declarações e comandos)
void bloco() {
    // Geração de Código: Início do programa
    printf("\tINPP\n");
    
    if (tokenAtual.atomo == VAR) {
        consome(VAR);
        declaracao_variaveis(); consome(PONTO_VIRGULA);
        while (tokenAtual.atomo == IDENTIFIER) {
            declaracao_variaveis();
            consome(PONTO_VIRGULA);
        }
    }
    
    // Geração de Código: Alocação de memória para variáveis
    printf("\tAMEM %d\n", proxEndereco);
    
    consome(BEGIN);
    comando();
    while (tokenAtual.atomo == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        comando();
    }
    consome(END);
    
    // Geração de Código: Fim do programa
    printf("\tPARA\n");
}

// Função principal de análise sintática (Regra P)
void programa() {
    consome(PROGRAM);
    consome(IDENTIFIER);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

// Função principal do compilador
int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }

    inicializar_tabela();
    carregar_arquivo(argv[1]);
    tokenAtual = obter_atomo();
    descarta_comentarios();
    programa();

    descarta_comentarios();
    if (tokenAtual.atomo != FIM_ARQUIVO) {
        erro_sintatico(FIM_ARQUIVO);
    }

    // Saída de debug: Tabela de Símbolos
    imprimir_tabela_simbolos();
    
    liberar_tabela_simbolos();
    free(buffer);
    
    return 0;
}