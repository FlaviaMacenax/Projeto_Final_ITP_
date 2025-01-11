
#include "funcoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Valida o identificador EAN-8
bool validar_identificador(const char *identificador) {
    if (strlen(identificador) != TAM_IDENTIFICADOR) return false;
    for (int i = 0; i < TAM_IDENTIFICADOR; i++) {
        if (!isdigit(identificador[i])) return false;
    }
    int digito_verificador = calcular_digito_verificador(identificador);
    return digito_verificador == (identificador[TAM_IDENTIFICADOR - 1] - '0');
}

// Calcula o dígito verificador do identificador
int calcular_digito_verificador(const char *identificador) {
    int soma = 0;
    for (int i = 0; i < TAM_IDENTIFICADOR - 1; i++) {
        int peso = (i % 2 == 0) ? 3 : 1;
        soma += (identificador[i] - '0') * peso;
    }
    int resto = soma % 10;
    return (resto == 0) ? 0 : 10 - resto;
}

// Gera o código de barras a partir do identificador
char *gerar_codigo_barras(const char *identificador, ParametrosImagem parametros) {
    // Aloca espaço para o código de barras (101 + L/R codes + marcadores)
    int largura_total = parametros.espacamento * 2 + 67 * parametros.pixels_por_area;
    char *codigo_barras = (char *)malloc(largura_total + 1);
    if (!codigo_barras) return NULL;

    // Monta o código: Marcador inicial
    strcpy(codigo_barras, "101");

    // Codifica os 4 primeiros dígitos (L-code)
    const char *l_code[] = {"0001101", "0011001", "0010011", "0111101", "0100011", "0110001", "0101111", "0111011", "0110111", "0001011"};
    for (int i = 0; i < 4; i++) {
        strcat(codigo_barras, l_code[identificador[i] - '0']);
    }

    // Marcador central
    strcat(codigo_barras, "01010");

    // Codifica os 4 últimos dígitos (R-code)
    const char *r_code[] = {"1110010", "1100110", "1101100", "1000010", "1011100", "1001110", "1010000", "1000100", "1001000", "1110100"};
    for (int i = 4; i < 8; i++) {
        strcat(codigo_barras, r_code[identificador[i] - '0']);
    }

    // Marcador final
    strcat(codigo_barras, "101");
    return codigo_barras;
}

// Salva o código de barras em formato PBM
bool salvar_imagem_pbm(const char *nome_arquivo, const char *codigo_barras, ParametrosImagem parametros) {
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) return false;

    int largura_total = parametros.espacamento * 2 + 67 * parametros.pixels_por_area;
    int altura_total = parametros.altura;

    // Cabeçalho PBM
    fprintf(arquivo, "P1\n%d %d\n", largura_total, altura_total);

    // Espaçamento superior
    for (int i = 0; i < parametros.espacamento; i++) {
        for (int j = 0; j < largura_total; j++) {
            fprintf(arquivo, "0 ");
        }
        fprintf(arquivo, "\n");
    }

    // Código de barras
    for (int i = 0; i < altura_total; i++) {
        for (int j = 0; j < parametros.espacamento; j++) {
            fprintf(arquivo, "0 ");
        }
        for (int j = 0; j < strlen(codigo_barras); j++) {
            for (int k = 0; k < parametros.pixels_por_area; k++) {
                fprintf(arquivo, "%c ", codigo_barras[j]);
            }
        }
        for (int j = 0; j < parametros.espacamento; j++) {
            fprintf(arquivo, "0 ");
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
    return true;
}

// Carrega a imagem PBM
char *carregar_imagem_pbm(const char *nome_arquivo, ParametrosImagem *parametros) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) return NULL;

    // Lê o cabeçalho
    char tipo[3];
    fscanf(arquivo, "%s", tipo);
    if (strcmp(tipo, "P1") != 0) {
        fclose(arquivo);
        return NULL;
    }

    fscanf(arquivo, "%d %d", &parametros->largura, &parametros->altura);

    // Aloca memória para o código de barras
    char *codigo_barras = (char *)malloc(parametros->largura * parametros->altura + 1);
    if (!codigo_barras) {
        fclose(arquivo);
        return NULL;
    }

    // Lê os dados da imagem
    for (int i = 0; i < parametros->largura * parametros->altura; i++) {
        fscanf(arquivo, "%s", &codigo_barras[i]);
    }
    fclose(arquivo);
    return codigo_barras;
}

