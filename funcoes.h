#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdbool.h>

// Definição de constantes padrão para a manipulação de imagens PBM e códigos de barras
#define ESPACAMENTO_PADRAO 4 // Espaçamento padrão
#define LARGURA_PADRAO 3     // Largura padrão 
#define ALTURA_PADRAO 50     // Altura padrão 
#define NOME_PADRAO "codigoean8.pbm" // Nome padrão 

// Estrutura para representar uma imagem PBM
typedef struct {
    int largura;    
    int altura;     
    char **pixels;  
} ImagemPBM;

/* DECLARAÇÃO DAS FUNÇÕES COMPARTILHADAS */

// Função para validar se o código tem o tamanho correto (8 caracteres)
bool validarTamanho(const char *codigo);

// Função para verificar se o código contém apenas dígitos numéricos
bool verificarDigitos(const char *codigo);

// Função para verificar a validade do código com base no dígito de controle
bool verificarCodigo(const char *codigo);

// Função para calcular o dígito de controle do código de barras
int calcularDigitoControle(const char *codigo);

// Função para codificar um código de barras EAN-8 em formato binário
void codificarCodigo(const char *codigoOriginal, char *codigoCodificado);

// Função para extrair o código original a partir de um código codificado em binário
bool extrairCodigoOriginal(const char *codigoCodificado, char *codigoExtraido);

// Função para decodificar segmentos do lado esquerdo do código de barras
int decodificarL(const char *codigoBinario);

// Função para decodificar segmentos do lado direito do código de barras
int decodificarR(const char *codigoBinario);

#endif // FUNCOES_H
