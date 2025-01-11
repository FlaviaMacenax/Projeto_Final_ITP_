#include "funcoes.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <identificador> [espaçamento] [pixels por área] [altura] [nome do arquivo]\n", argv[0]);
        return 1;
    }

    // Identificador
    char *identificador = argv[1];
    if (strlen(identificador) != TAM_IDENTIFICADOR) {
        printf("Erro: Identificador deve conter exatamente 8 dígitos.\n");
        return 1;
    }

    for (int i = 0; i < TAM_IDENTIFICADOR; i++) {
        if (!isdigit(identificador[i])) {
            printf("Erro: Identificador contém valores não numéricos.\n");
            return 1;
        }
    }

    if (!validar_identificador(identificador)) {
        printf("Erro: Dígito verificador inválido.\n");
        return 1;
    }

    // Parâmetros opcionais
    ParametrosImagem parametros;
    parametros.espacamento = (argc > 2) ? atoi(argv[2]) : ESPACAMENTO_PADRAO;
    parametros.pixels_por_area = (argc > 3) ? atoi(argv[3]) : LARGURA_PADRAO;
    parametros.altura = (argc > 4) ? atoi(argv[4]) : ALTURA_PADRAO;

    // Nome do arquivo
    char *nome_arquivo = argv[5];

    // Verifica se o arquivo já existe
    FILE *arquivo_teste = fopen(nome_arquivo, "r");
    if (arquivo_teste) {
        fclose(arquivo_teste);
        char resposta;
        printf("O arquivo '%s' já existe. Deseja sobrescrevê-lo? (s/n): ", nome_arquivo);
        scanf(" %c", &resposta);
        if (resposta != 's' && resposta != 'S') {
            printf("Erro: Arquivo resultante já existe.\n");
            return 1;
        }
    }

    // Gera o código de barras
    char *codigo_barras = gerar_codigo_barras(identificador, parametros);
    if (!codigo_barras) {
        printf("Erro ao gerar o código de barras.\n");
        return 1;
    }

    // Salva a imagem PBM
    if (!salvar_imagem_pbm(nome_arquivo, codigo_barras, parametros)) {
        printf("Erro ao salvar a imagem PBM.\n");
        free(codigo_barras);
        return 1;
    }

    printf("Imagem PBM gerada com sucesso: %s\n", nome_arquivo);
    free(codigo_barras);
    return 0;
}
