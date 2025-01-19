# Gerador e Leitor de Código de Barras EAN-8

Projeto desenvolvido para a disciplina de ITP (Introdução às Técnicas de Programação) 2024.2, que implementa um sistema de geração e leitura de códigos de barras EAN-8 utilizando o formato de imagem PBM (Portable Bitmap Format).

## Visão Geral

O EAN-8 é um padrão de código de barras comumente utilizado para itens pequenos onde o espaço é limitado. É composto por 8 dígitos:
- 7 dígitos para informações do produto
- 1 dígito verificador para detecção de erros

O projeto lida tanto com a codificação de números em códigos de barras quanto com a decodificação dos códigos de barras de volta para sua representação numérica.

## Funcionalidades

- Geração de códigos de barras EAN-8 em imagens PBM
- Leitura e validação de códigos de barras EAN-8 a partir de imagens PBM
- Dimensões e espaçamento personalizáveis do código de barras
- Validação de entrada e tratamento de erros
- Verificação do dígito verificador

## Estrutura do Projeto

```
├── 
│   ├── gerador.c       # Programa de geração de código de barras
│   ├── extrator.c        # Programa de leitura de código de barras
│   ├── funcoes.c  # Funcionalidades compartilhadas
│   ├── funcoes.h  # Cabeçalho para funções compartilhadas
│   └── pbm.c           # Manipulação de imagem PBM
└── README.md
```

## Compilação

Para compilar os programas separadamente:

```bash
# Compilar o gerador
gcc -o gerador gerador.c funcoes.c

# Compilar o leitor
gcc -o extrator extrator.c funcoes.c
```

## Tratamento de Erros

Gerador:
- Comprimento inválido do identificador
- Caracteres não numéricos no identificador
- Dígito verificador inválido
- Confirmação de sobrescrita de arquivo existente

Leitor:
- Arquivo de entrada inválido ou inexistente
- Formato PBM inválido
- Código de barras não encontrado na imagem
- Formato de código de barras inválido

## Detalhes Técnicos

### Estrutura EAN-8
- Marcador inicial: 101
- Primeiros 4 dígitos (L-code)
- Marcador central: 01010
- Últimos 4 dígitos (R-code)
- Marcador final: 101

### Formato PBM
- Formato baseado em ASCII
- Cabeçalho: "P1"
- Dimensões: largura altura
- Dados binários: 0 para branco, 1 para preto


## Autores

- FLÁVIA JAMILY DOS SANTOS MACENA
- LUIZ FELIPE DE ARAUJO BEZERRA

## Professores

- EDSON JACKSON DE MEDEIROS NETO
- ANNA GISELLE C. D. RIBEIRO RODRIGUES
