
#include "funcoes.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Arrays para representar a codificação dos dígitos
// CODIFICACAO_L: Representa os dígitos 0-9 para o lado esquerdo do código de barras
const char *CODIFICACAO_L[] = {"0001101", "0011001", "0010011", "0111101", "0100011", 
                                  "0110001", "0101111", "0111011", "0110111", "0001011"};

// CODIFICACAO_R: Representa os dígitos 0-9 para o lado direito do código de barras
const char *CODIFICACAO_R[] = {"1110010", "1100110", "1101100", "1000010", "1011100", 
                                  "1001110", "1010000", "1000100", "1001000", "1110100"};

// Verifica se o código possui exatamente 8 caracteres
bool validarTamanho(const char *codigo) {
    int comprimento = strlen(codigo);
    return (comprimento == 8);
}

// Verifica se todos os caracteres do código são dígitos numéricos
bool verificarDigitos(const char *codigo) {
    for (int i = 0; codigo[i] != '\0'; i++) {
        if (!isdigit(codigo[i])) {
            return false;
        }
    }
    return true;
}

// Calcula o dígito de controle do código de barras
int calcularDigitoControle(const char *codigo) {
    int somaTotal = 0;
    for (int i = 0; i < 7; i++) {
        int valorDigito = codigo[i] - '0';
        somaTotal += (i % 2 == 0) ? valorDigito * 3 : valorDigito;
    }
    int modulo = somaTotal % 10;
    return (modulo == 0) ? 0 : 10 - modulo;
}

// Verifica se o código de barras é válido
bool verificarCodigo(const char *codigo) {
    if (!verificarDigitos(codigo) || !validarTamanho(codigo)) {
        return false;
    }
    int digitoCalculado = calcularDigitoControle(codigo);
    int ultimoDigito = codigo[7] - '0';
    return (ultimoDigito == digitoCalculado);
}

// Codifica o código de barras em binário usando as tabelas de codificação L e R
void codificarCodigo(const char *codigoOriginal, char *codigoCodificado) {
    strcpy(codigoCodificado, "101"); // Delimitador inicial
    for (int i = 0; i < 4; i++) {
        strcat(codigoCodificado, CODIFICACAO_L[codigoOriginal[i] - '0']); 
    }
    strcat(codigoCodificado, "01010"); // Delimitador central
    for (int i = 4; i < 8; i++) {
        strcat(codigoCodificado, CODIFICACAO_R[codigoOriginal[i] - '0']); 
    }
    strcat(codigoCodificado, "101"); // Delimitador final
}

// Decodifica um segmento de 7 bits do lado esquerdo do código de barras
int decodificarL(const char *codigoBinario) {
    for (int i = 0; i < 10; i++) {
        if (strncmp(codigoBinario, CODIFICACAO_L[i], 7) == 0) {
            return i;
        }
    }
    return -1; 
}

// Decodifica um segmento de 7 bits do lado direito do código de barras
int decodificarR(const char *codigoBinario) {
    for (int i = 0; i < 10; i++) {
        if (strncmp(codigoBinario, CODIFICACAO_R[i], 7) == 0) {
            return i;
        }
    }
    return -1; 
}

// Extrai o código original a partir do código codificado em binário
bool extrairCodigoOriginal(const char *codigoCodificado, char *codigoExtraido) {
    if (strlen(codigoCodificado) != 67) {
        return false;
    }

    if (strncmp(codigoCodificado, "101", 3) != 0) return false;
    if (strncmp(codigoCodificado + 31, "01010", 5) != 0) return false;
    if (strncmp(codigoCodificado + 64, "101", 3) != 0) return false;

    char buffer[8]; 
    int indice = 3; 

    // Decodifica os primeiros 4 dígitos (lado esquerdo)
    for (int i = 0; i < 4; i++) {
        strncpy(buffer, codigoCodificado + indice, 7);
        buffer[7] = '\0';
        int decodificado = decodificarL(buffer);
        if (decodificado < 0) return false;
        codigoExtraido[i] = '0' + decodificado;
        indice += 7;
    }

    indice += 5; 

    // Decodifica os últimos 4 dígitos (lado direito)
    for (int i = 4; i < 8; i++) {
        strncpy(buffer, codigoCodificado + indice, 7);
        buffer[7] = '\0';
        int decodificado = decodificarR(buffer);
        if (decodificado < 0) return false;
        codigoExtraido[i] = '0' + decodificado;
        indice += 7;
    }

    codigoExtraido[8] = '\0'; 
    return true;
}

