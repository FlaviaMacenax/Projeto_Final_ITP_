#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdbool.h>

// Constantes
#define TAM_IDENTIFICADOR 8
#define LARGURA_PADRAO 3
#define ALTURA_PADRAO 50
#define ESPACAMENTO_PADRAO 4

// Estrutura para armazenar os parâmetros da imagem
typedef struct {
    int largura;
    int altura;
    int espacamento;
    int pixels_por_area;
} ParametrosImagem;

// Funções compartilhadas
bool validar_identificador(const char *identificador); // Valida o identificador EAN-8
int calcular_digito_verificador(const char *identificador); // Calcula o dígito verificador
char *gerar_codigo_barras(const char *identificador, ParametrosImagem parametros); // Gera o código de barras
bool salvar_imagem_pbm(const char *nome_arquivo, const char *codigo_barras, ParametrosImagem parametros); // Salva a imagem PBM
char *carregar_imagem_pbm(const char *nome_arquivo, ParametrosImagem *parametros); // Carrega a imagem PBM
#endif
