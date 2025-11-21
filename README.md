# COMPILADOR PASKENZIE 

Turma 06N
- Matheus Chediac Rodrigues (10417490)
- Lucas Monteiro Soares (10417881)

Este projeto implementa a segunda fase do compilador para a linguagem PasKenzie, adicionando:

1. ANÁLISE SEMÂNTICA
   - Verificação de declaração única de variáveis
   - Verificação de uso de variáveis declaradas
   - Implementação de tabela de símbolos usando Hash Table

2. GERAÇÃO DE CÓDIGO INTERMEDIÁRIO
   - Geração de código MEPA para todas as construções da linguagem
   - Comandos: atribuição, leitura, escrita, condicional, repetição
   - Expressões aritméticas e relacionais

===============================================================================
IMPLEMENTAÇÃO
===============================================================================

TABELA DE SÍMBOLOS:
- Implementada como Hash Table com 211 entradas (número primo)
- Cada entrada é uma lista encadeada para tratar colisões
- Usa a função hashMack() fornecida (hashMack.o ou hashMack.obj)
- Armazena: identificador da variável e seu endereço na memória

ANÁLISE SEMÂNTICA:
- Verifica declaração única na seção VAR
- Verifica se variáveis usadas foram declaradas
- Emite erros semânticos informativos com número da linha

GERAÇÃO DE CÓDIGO:
- Código MEPA gerado durante a análise sintática
- Instruções implementadas: INPP, AMEM, LEIT, ARMZ, CRVL, CRCT, SOMA, 
  SUBT, MULT, DIVI, CMIG, CMDG, CMME, CMEG, CMMA, CMAG, DSVF, DSVS, 
  NADA, IMPR, PARA
- Rótulos gerados automaticamente (L1, L2, ...)

===============================================================================
COMPILAÇÃO E EXECUÇÃO
===============================================================================

gcc -g -Og -Wall compilador.c hashMack.o -o compilador

./compilador <arquivo.pas>

===============================================================================
ESTRUTURAS DE DADOS PRINCIPAIS
===============================================================================

1. TInfoAtomo: Armazena informações do token atual
2. TTabelaSimbolos: Hash table com 211 entradas
3. TNo: Nó da lista encadeada para cada entrada da hash table

===============================================================================
FUNCIONALIDADES IMPLEMENTADAS
===============================================================================

Análise léxica (mantida da Fase 1)
Análise sintática (mantida da Fase 1)
Análise semântica:
  Verificação de declaração única
  Verificação de variáveis não declaradas
  Tabela de símbolos com Hash Table
Geração de código intermediário:
  Expressões aritméticas (+, -, *, div)
  Expressões relacionais (=, <>, <, <=, >, >=)
  Comando de atribuição
  Comando de leitura (read)
  Comando de escrita (write)
  Comando condicional (if-then-else)
  Comando de repetição (while-do)
  Blocos begin-end

===============================================================================
OBSERVAÇÕES
===============================================================================

- O compilador trata apenas variáveis do tipo INTEGER
- Não há distinção entre expressões inteiras e lógicas (conforme especificação)
- Todos os erros (léxicos, sintáticos e semânticos) são reportados
- O programa finaliza na primeira ocorrência de erro
- A memória da tabela de símbolos é liberada ao final da execução