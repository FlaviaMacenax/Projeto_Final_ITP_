
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "funcoes.h"  // Arquivo com funções auxiliares, presumivelmente fornecido à parte

// Função para criar uma imagem PBM com largura e altura especificadas
ImagemPBM criarImagem(int largura, int altura) {
    ImagemPBM imagem;
    imagem.largura = largura;
    imagem.altura = altura;
    
    // Aloca memória para as linhas da matriz de pixels
    imagem.pixels = malloc(altura * sizeof(int *));
    if (imagem.pixels == NULL) {
        printf("Erro: falha ao alocar memória.\n");
        exit(1);  // Encerra o programa se houver erro
    }

    // Aloca memória para as colunas de cada linha
    for (int i = 0; i < altura; i++) {
        imagem.pixels[i] = calloc(largura, sizeof(int));
        if (imagem.pixels[i] == NULL) {
            printf("Erro: falha ao alocar memória.\n");
            
            // Libera a memória já alocada em caso de erro
            for (int j = 0; j < i; j++) {
                free(imagem.pixels[j]);
            }
            free(imagem.pixels);
            exit(1);  // Encerra o programa se houver erro
        }
    }
    return imagem;
}

// Função para liberar a memória alocada para a imagem PBM
void liberarImagem(ImagemPBM *imagem) {
    for (int i = 0; i < imagem->altura; i++) {
        free(imagem->pixels[i]);  // Libera cada linha
    }
    free(imagem->pixels);  // Libera a matriz de pixels
    imagem->pixels = NULL;  // Evita acessos futuros a memória já liberada
}

// Função para preencher a matriz de pixels com base no código codificado
void preencherImagem(ImagemPBM *imagem, int largura, int altura, const char *codificado) {
    int comprimentoCodificado = strlen(codificado);  // Tamanho da string codificada
    int larguraPixel = 1;  // Define a largura de cada pixel (padrão 1)
    int margemSuperior = altura / 4;  // Margem superior da imagem
    int margemInferior = altura / 4;  // Margem inferior da imagem

    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            if (i < margemSuperior || i >= altura - margemInferior) {
                // Define as margens como pixels pretos (0)
                imagem->pixels[i][j] = 0;
            } else {
                // Determina o índice no código codificado com base na largura do pixel
                int indiceCodificado = j / larguraPixel;
                if (indiceCodificado < comprimentoCodificado && codificado[indiceCodificado] == '1') {
                    imagem->pixels[i][j] = 1;  // Define barras como 1
                } else {
                    imagem->pixels[i][j] = 0;  // Define espaços como 0
                }
            }
        }
    }
}

// Função para salvar a imagem PBM em um arquivo no formato correto
void salvarImagem(ImagemPBM *imagem, const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "w");  // Abre o arquivo no modo escrita
    if (arquivo == NULL) {
        printf("ERRO: Não foi possível abrir o arquivo.\n");
        return;
    }

    // Cabeçalho do arquivo PBM
    fprintf(arquivo, "P1\n");
    fprintf(arquivo, "%d %d\n", imagem->largura, imagem->altura);

    // Escreve os pixels no arquivo
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            fprintf(arquivo, "%d", imagem->pixels[i][j]);
            if (j < imagem->largura - 1) {
                fprintf(arquivo, " ");
            }
        }
        fprintf(arquivo, "\n");
    }
    fclose(arquivo);  // Fecha o arquivo
    printf("Imagem salva com sucesso em: %s\n", nomeArquivo);
}

// Função para verificar se um arquivo já existe
bool arquivoExiste(char *filename) {
    struct stat infoImg;
    if (stat(filename, &infoImg) == 0) {
        return true;  // Arquivo existe
    } else {
        return false;  // Arquivo não existe
    }
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Valida se o identificador foi informado
        printf("ERRO: identificador nao informado. \n");
        printf("É necessario informar o identificador, os demais argumentos são opcionais.\n");
        fprintf(stderr, "Informe na seguinte ordem: %s [identificador EAN-8] [espacamento] [pixels_por_area] [altura] [arquivoSaida]\n", argv[0]);
        return 1;
    }
    
    char *CodigoOriginal = argv[1];  // Primeiro argumento: código EAN-8
    if (!verificarDigitos(CodigoOriginal)) {
        fprintf(stderr, "ERRO: o identificador possui caracteres nao numericos.\n");
        return 1;
    }
    if (!validarTamanho(CodigoOriginal)) {
        fprintf(stderr, "ERRO: o identificador nao possui 8 digitos.\n");
        return 1;
    }
    if (!verificarCodigo(CodigoOriginal)) {
        fprintf(stderr, "ERRO: codigo inválido.\n");
        return 1;
    }

    // Define os parâmetros padrão
    int espacamento = ESPACAMENTO_PADRAO;
    int area = LARGURA_PADRAO;
    int altura = ALTURA_PADRAO;
    char nome[256];
    strcpy(nome, NOME_PADRAO);

    // Sobrescreve os parâmetros com os valores fornecidos na linha de comando
    if (argc >= 3) {
        espacamento = atoi(argv[2]);
    }
    if (argc >= 4) {
        area = atoi(argv[3]);
    }
    if (argc >= 5) {
        altura = atoi(argv[4]);
    }
    if (argc >= 6) {
        strncpy(nome, argv[5], 255);
        nome[255] = '\0';
    }

    // Verifica se o arquivo já existe e solicita confirmação do usuário
    if (arquivoExiste(nome)) {
        printf("O arquivo \"%s\" ja existe. Deseja sobrescreve-lo? (s/n) ", nome);
        char respostaUser[10];
        if (!fgets(respostaUser, sizeof(respostaUser), stdin)) {
            fprintf(stderr, "ERRO: nao foi possivel ler o arquivo.\n");
            return 1;
        }
        if (respostaUser[0] != 's' && respostaUser[0] != 'S') {
            fprintf(stderr, "ERRO: Ja existe um arquivo com o mesmo nome.\n");
            return 1;
        }
    }

    // Codifica o código original no formato apropriado
    char CodigoCodificado[68];
    CodigoCodificado[0] = '\0';
    codificarCodigo(CodigoOriginal, CodigoCodificado);

    // Calcula as dimensões da imagem
    int novaLarguraMatriz = 67 * area;
    int larguraTotal = novaLarguraMatriz + (2 * espacamento);
    int alturaTotal = altura + (2 * espacamento);

    // Cria, preenche e salva a imagem PBM
    ImagemPBM imagem = criarImagem(larguraTotal, alturaTotal);
    preencherImagem(&imagem, larguraTotal, alturaTotal, CodigoCodificado);
    salvarImagem(&imagem, nome);
    liberarImagem(&imagem);  // Libera a memória utilizada

    return 0;
}

