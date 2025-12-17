# Compilador PasKenzie

Matheus Chediac Rodrigues (10417490)  
Lucas Monteiro Soares (10417881)

## Sobre o Projeto

Compilador desenvolvido para a disciplina de COMPILADORES (Universidade Presbiteriana Mackenzie), para a linguagem PasKenzie que realiza análise léxica, sintática, semântica e geração de código intermediário MEPA.

## Funcionalidades

### Análise Léxica e Sintática
- Reconhecimento de tokens (identificadores, palavras reservadas, operadores, constantes)
- Validação da estrutura do programa conforme a gramática da linguagem
- Tratamento de comentários no formato `(* ... *)`

### Análise Semântica
- **Tabela de símbolos** implementada como Hash Table (211 entradas)
- Verificação de declaração única de variáveis
- Verificação de uso de variáveis não declaradas
- Mensagens de erro com número da linha

### Geração de Código MEPA
Geração de código intermediário para:
- Expressões aritméticas (`+`, `-`, `*`, `div`)
- Expressões relacionais (`=`, `<>`, `<`, `<=`, `>`, `>=`)
- Comandos: atribuição, `read`, `write`, `if-then-else`, `while-do`
- Blocos `begin-end`

## Compilação e Uso

```bash
# Linux
gcc -g -Og -Wall compilador.c hashMack.o -o compilador

# Windows
gcc -g -Og -Wall compilador.c hashMack.obj -o compilador.exe
```

**Executar:**
```bash
./compilador programa.pas
```

## Estrutura do Código

- `compilador.c` - Código principal do compilador
- `hashMack.o` / `hashMack.obj` - Função hash para a tabela de símbolos
- `testa_hashMack.c` - Programa auxiliar para testar a função hash

## Exemplo de Saída

O compilador gera:
1. Código MEPA na saída padrão
2. Tabela de símbolos ao final da execução
3. Mensagens de erro (se houver) indicando tipo e linha

## Limitações

- Suporta apenas variáveis do tipo INTEGER, CHAR e BOOLEAN (sem distinção na geração de código)
- Programa encerra na primeira ocorrência de erro
- Identificadores limitados a 15 caracteres
