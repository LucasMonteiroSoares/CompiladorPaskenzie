/*
Projeto Compiladores 06N
Matheus Chediac Rodrigues       10417490
Lucas Monteiro Soares           10417881
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ID_LEN 15 // Definimos o tamanho máximo de ID aqui no começo para ser mais fácil de mudar posteriormente

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
        int  numero;    
        char id[16];    
        char ch;    
    } atributo;
} TInfoAtomo;

// Variáveis do Scanner 
char *buffer = NULL;
long buf_len = 0;
long pos = 0;
int  numero_linha = 1;

// Função que anda no buffer (conta linha também)
void avancar(int n) {
    for (int i = 0; i < n && pos < buf_len; ++i) {
        if (buffer[pos] == '\n') 
        numero_linha++;
        pos++;
    }
}

// Mostra erro léxico e encerra
void erro_lexico(const char *msg) {
    fprintf(stderr, "# %d:erro lexico: %s\n", numero_linha, msg);
    exit(1);
}

// Tabela de nomes 
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

// Retorna o nome do token para impressão
const char *nome_token(TAtomo a) {
    switch (a) {
        case IF:
            return "if";
            case WHILE: return "while";
            case PROGRAM: return "program";
        case DIV:
            return "div";
            case OR: return "or";
            case AND: return "and";
            case NOT: return "not";
        case READ:
            return "read";
            case WRITE: return "write";
            case VAR: return "var";
        case BEGIN:
            return "begin";
            case END: return "end";
            case TRUE: return "true";
            case FALSE: return "false";
        case CHAR:
            return "char";
            case INTEGER: return "integer";
            case BOOLEAN: return "boolean";
        case THEN:
            return "then";
            case ELSE: return "else";
            case DO: return "do";
        case ATRIBUICAO:
            return "atribuicao";
            case DOIS_PONTOS: return "dois_pontos";
        case PONTO_VIRGULA:
            return "ponto_virgula";
            case VIRGULA: return "virgula";
            case PONTO: return "ponto";
        case MAIS:
            return "mais";
            case MENOS: return "menos";
            case ASTERISCO: return "asterisco";
        case ABRE_PAR:
            return "abre_par";
            case FECHA_PAR: return "fecha_par";
        case DIFERENTE:
            return "diferente";
            case MENOR_IGUAL: return "menor_igual";
        case MAIOR_IGUAL:
            return "maior_igual";
            case IGUAL: return "igual";
        case MENOR:
            return "menor";
            case MAIOR: return "maior";
        case IDENTIFIER:
            return "identifier";
            case CONSTINT: return "constint";
            case CONSTCHAR: return "constchar";
        case COMENTARIO:
            return "comentario";
            case FIM_ARQUIVO: return "EOF";
        default:
            return "ERRO";
    }
}

const char *lexeme_curto(TAtomo a) {
    switch (a) {
        case ATRIBUICAO:
            return ":=";
        case DOIS_PONTOS:
            return ":";
        case PONTO_VIRGULA:
            return ";";
        case VIRGULA:
            return ",";
        case PONTO:
            return ".";
        case ABRE_PAR:
            return "(";
        case FECHA_PAR:
            return ")";
        case DIFERENTE:
            return "<>";
        case MENOR_IGUAL:
            return "<=";
        case MAIOR_IGUAL:
            return ">=";
        case IGUAL:
            return "=";
        case MENOR:
            return "<";
            case MAIOR: return ">";
        case PROGRAM:
            return "program";
            case VAR: return "var";
            case BEGIN: return "begin";
            case END: return "end";
        case READ:
            return "read";
            case WRITE: return "write";
            case IF: return "if";
            case THEN: return "then";
        case ELSE:
            return "else";
            case WHILE: return "while";
            case DO: return "do";
        case OR:
            return "or";
            case AND: return "and";
            case NOT: return "not";
        case TRUE:
            return "true";
            case FALSE: return "false";
        case CHAR:
            return "char";
            case INTEGER: return "integer";
            case BOOLEAN: return "boolean";
        case IDENTIFIER:
            return "identifier";
            case CONSTINT: return "constint";
            case CONSTCHAR: return "constchar";
        case FIM_ARQUIVO:
            return "EOF";
        default:
            return "ERRO";
    }
}

// Função que imprime tokens
void imprimir_token(const TInfoAtomo *t) {
    if (t->atomo == ERRO || t->atomo == FIM_ARQUIVO) return;
    printf("# %2d:", t->linha);
    if (t->atomo == IDENTIFIER) {
    printf("identifier : %s\n", t->atributo.id);
    return;
}
    if (t->atomo == CONSTINT) {
    printf("constint : %d\n", t->atributo.numero);
    return;
}
    if (t->atomo == CONSTCHAR) {
    printf("constchar : '%c'\n", t->atributo.ch);
    return;
}
    printf("%s\n", nome_token(t->atomo));
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
        char prox = (pos+1 < buf_len ? buffer[pos+1] : '\0');
        if (prox == '*') {
            int l0 = numero_linha;
            avancar(2);
            while (!(buffer[pos] == '*' && (pos+1 < buf_len && buffer[pos+1] == ')'))) {
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
            if (i < MAX_ID_LEN) lex[i++] = buffer[pos];
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
    val = val*10 + (buffer[pos]-'0');
    avancar(1);
}
        if (buffer[pos] == 'd') {
            avancar(1);
            if (buffer[pos] == '+') avancar(1);
            if (!isdigit((unsigned char)buffer[pos])) 
            erro_lexico("expoente em constint mal formado");
            int e=0; 
            while (isdigit((unsigned char)buffer[pos])) { 
                e = e*10 + (buffer[pos]-'0'); avancar(1); }
            for (int k=0;k<e;++k) 
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
        case ':': if ((pos+1<buf_len)&&buffer[pos+1]=='='){ 
            avancar(2);
            t.atomo=ATRIBUICAO;
            } else { avancar(1);
            t.atomo=DOIS_PONTOS;
            } return t;
        case ';':
            avancar(1);
            t.atomo=PONTO_VIRGULA;
            return t;
        case ',':
            avancar(1);
            t.atomo=VIRGULA;
            return t;
        case '.':
            avancar(1);
            t.atomo=PONTO;
            return t;
        case '(':
            avancar(1);
            t.atomo=ABRE_PAR;
            return t;
        case ')':
            avancar(1);
            t.atomo=FECHA_PAR;
            return t;
        case '+':
            avancar(1);
            t.atomo=MAIS;
            return t;
        case '-':
            avancar(1);
            t.atomo=MENOS;
            return t;
        case '*':
            avancar(1);
            t.atomo=ASTERISCO;
            return t;
        case '<':
            if ((pos+1<buf_len)&&buffer[pos+1]=='>') {
    avancar(2);
    t.atomo=DIFERENTE;
    return t;
}
            if ((pos+1<buf_len)&&buffer[pos+1]=='=') {
    avancar(2);
    t.atomo=MENOR_IGUAL;
    return t;
}
            avancar(1); t.atomo=MENOR; return t;
        case '>':
            if ((pos+1<buf_len)&&buffer[pos+1]=='=') {
    avancar(2);
    t.atomo=MAIOR_IGUAL;
    return t;
}
            avancar(1); t.atomo=MAIOR; return t;
        case '=':
            avancar(1);
            t.atomo=IGUAL;
            return t;
        default:
            erro_lexico("simbolo desconhecido");
    }

    return t;
}

// Carregar arquivo de entrada na memória
void carregar_de_stream(FILE *f) {
    size_t cap = 4096, n = 0;
    buffer = (char*)malloc(cap + 1);
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
            char *novo = (char*)realloc(buffer, cap + 1);
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

// Abre arquivo
void carregar_arquivo(const char *nome) {
    FILE *f = fopen(nome, "rb");
    if (!f) {
    fprintf(stderr, "Erro ao abrir arquivo %s\n", nome);
    exit(1);
}
    carregar_de_stream(f);
    fclose(f);
}

TInfoAtomo tokenAtual;
// Pré definição de funções para evitar problemas
void expressao(void); 
void comando(void);     

// Lê próximo token
void proximo() { tokenAtual = obter_atomo(); }

void descarta_comentarios() { while (tokenAtual.atomo == COMENTARIO) { imprimir_token(&tokenAtual); proximo(); } }

void erro_sintatico(TAtomo esp) {
    fprintf(stderr, "# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
            tokenAtual.linha, lexeme_curto(esp), lexeme_curto(tokenAtual.atomo));
    exit(1);
}

// Consome token esperado
void consome(TAtomo esp) {
    descarta_comentarios();
    if (tokenAtual.atomo == esp) {
    imprimir_token(&tokenAtual);
    proximo();
    descarta_comentarios();
}
    else erro_sintatico(esp);
}

// As próximas funções implementam a gramática recursiva
// Cada não-terminal da gramática tem sua função
void fator() {
    switch (tokenAtual.atomo) {
        case IDENTIFIER:
            consome(IDENTIFIER);
            break;
        case CONSTINT:
            consome(CONSTINT);
            break;
        case CONSTCHAR:
            consome(CONSTCHAR);
            break;
        case ABRE_PAR:
            consome(ABRE_PAR);
            expressao();
            consome(FECHA_PAR);
            break;
        case NOT:
            consome(NOT);
            fator();
            break;
        case TRUE:
            consome(TRUE);
            break;
        case FALSE:
            consome(FALSE);
            break;
        default:
            erro_sintatico(IDENTIFIER);
    }
}
void termo() {
    fator();
    while (tokenAtual.atomo == ASTERISCO || tokenAtual.atomo == DIV) {
        if (tokenAtual.atomo == ASTERISCO) consome(ASTERISCO); else consome(DIV);
        fator();
    }
}
void exp_simples() {
    termo();
    while (tokenAtual.atomo == MAIS || tokenAtual.atomo == MENOS) {
        if (tokenAtual.atomo == MAIS) 
        consome(MAIS); 
        else consome(MENOS);
        termo();
    }
}
void op_rel() {
    switch (tokenAtual.atomo) {
        case DIFERENTE:
            consome(DIFERENTE);
            break;
        case MENOR:
            consome(MENOR);
            break;
        case MENOR_IGUAL:
            consome(MENOR_IGUAL);
            break;
        case MAIOR_IGUAL:
            consome(MAIOR_IGUAL);
            break;
        case MAIOR:
            consome(MAIOR);
            break;
        case IGUAL:
            consome(IGUAL);
            break;
        case OR:
            consome(OR);
            break;
        case AND:
            consome(AND);
            break;
        default:
            erro_sintatico(IGUAL);
    }
}
void expressao() {
    exp_simples();
    if (tokenAtual.atomo==DIFERENTE||tokenAtual.atomo==MENOR||tokenAtual.atomo==MENOR_IGUAL||
        tokenAtual.atomo==MAIOR||tokenAtual.atomo==MAIOR_IGUAL||tokenAtual.atomo==IGUAL||
        tokenAtual.atomo==OR||tokenAtual.atomo==AND) {
        op_rel();
        exp_simples();
    }
}

void atribuicao() { consome(IDENTIFIER); consome(ATRIBUICAO); expressao(); }
void leitura() {
    consome(READ); consome(ABRE_PAR); consome(IDENTIFIER);
    while (tokenAtual.atomo == VIRGULA) {
    consome(VIRGULA);
    consome(IDENTIFIER);
}
    consome(FECHA_PAR);
}
void escrita() {
    consome(WRITE); consome(ABRE_PAR); consome(IDENTIFIER);
    while (tokenAtual.atomo == VIRGULA) {
    consome(VIRGULA);
    consome(IDENTIFIER);
}
    consome(FECHA_PAR);
}

void condicional() {
    consome(IF); expressao(); consome(THEN); comando();
    if (tokenAtual.atomo == ELSE) {
    consome(ELSE);
    comando();
}
}
void repeticao() {
    consome(WHILE); expressao(); consome(DO); comando();
}

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
void declaracao_variaveis() {
    consome(IDENTIFIER);
    while (tokenAtual.atomo == VIRGULA) {
    consome(VIRGULA);
    consome(IDENTIFIER);
}
    consome(DOIS_PONTOS);
    tipo();
}
void bloco() {
    if (tokenAtual.atomo == VAR) {
        consome(VAR);
        declaracao_variaveis(); consome(PONTO_VIRGULA);
        while (tokenAtual.atomo == IDENTIFIER) {
    declaracao_variaveis();
    consome(PONTO_VIRGULA);
}
    }
    consome(BEGIN);
    comando();
    while (tokenAtual.atomo == PONTO_VIRGULA) {
    consome(PONTO_VIRGULA);
    comando();
}
    consome(END);
}
void programa() {
    consome(PROGRAM);
    consome(IDENTIFIER);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }

    carregar_arquivo(argv[1]);
    tokenAtual = obter_atomo();
    descarta_comentarios();
    programa();

    descarta_comentarios();
    if (tokenAtual.atomo != FIM_ARQUIVO) {
        fprintf(stderr, "# %d:erro sintatico, esperado [%s] encontrado [%s]\n",
                tokenAtual.linha, nome_token(FIM_ARQUIVO), nome_token(tokenAtual.atomo));
        return 1;
    }

    printf("%d linhas analisadas, programa sintaticamente correto\n", numero_linha);
    return 0;
}