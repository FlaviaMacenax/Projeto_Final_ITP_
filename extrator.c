
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "funcoes.h"

// Estrutura para encapsular o resultado do carregamento da imagem
typedef struct {
    ImagemPBM *imagem; 
    bool sucesso;      
} ResultadoCarregamento;

// Função para carregar o cabeçalho de um arquivo PBM
bool carregarCabecalhoPBM(FILE *arquivo, int *largura, int *altura) {
    char tipoArquivo[3];
    if (fscanf(arquivo, "%2s", tipoArquivo) != 1 || strcmp(tipoArquivo, "P1") != 0) {
        fprintf(stderr, "Erro: formato PBM inválido ou não suportado.\n");
        return false;
    }
    if (fscanf(arquivo, "%d %d", largura, altura) != 2) {
        fprintf(stderr, "Erro ao ler dimensões da imagem.\n");
        return false;
    }
    return true;
}

// Função para carregar os pixels de uma imagem PBM
bool carregarPixelsPBM(FILE *arquivo, ImagemPBM *imagem) {
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            int pixel;
            if (fscanf(arquivo, "%d", &pixel) != 1) {
                fprintf(stderr, "Erro ao ler pixels da imagem.\n");
                return false;
            }
            imagem->pixels[i * imagem->largura + j] = pixel;
        }
    }
    return true;
}

// Função principal para carregar uma imagem PBM de um arquivo
ResultadoCarregamento carregarImagemPBM(const char *caminhoArquivo) {
    FILE *arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo PBM: %s\n", caminhoArquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    int largura, altura;
    if (!carregarCabecalhoPBM(arquivo, &largura, &altura)) {
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    ImagemPBM *imagem = malloc(sizeof(ImagemPBM));
    if (!imagem) {
        fprintf(stderr, "Erro: falha ao alocar memória para a imagem.\n");
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    imagem->largura = largura;
    imagem->altura = altura;
    imagem->pixels = malloc(largura * altura * sizeof(int));
    if (!imagem->pixels) {
        fprintf(stderr, "Erro: falha ao alocar memória para os pixels.\n");
        free(imagem);
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    if (!carregarPixelsPBM(arquivo, imagem)) {
        free(imagem->pixels);
        free(imagem);
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    fclose(arquivo);
    return (ResultadoCarregamento){.imagem = imagem, .sucesso = true};
}

// Função para extrair os bits de uma linha da imagem, representando o código de barras
void extrairBitsLinha(const ImagemPBM *imagem, int linha, int inicio, int larguraColuna, char *bitsCodificados) {
    for (int bit = 0; bit < 67; bit++) {
        int contagemPreto = 0; 
        for (int deslocamento = 0; deslocamento < larguraColuna; deslocamento++) {
            int posX = inicio + bit * larguraColuna + deslocamento; 
            int indicePixel = linha * imagem->largura + posX; 
            if (imagem->pixels[indicePixel] == 1) {
                contagemPreto++; 
            }
        }
        bitsCodificados[bit] = (contagemPreto > (larguraColuna / 2)) ? '1' : '0';
    }
    bitsCodificados[67] = '\0'; 
}

// Função para localizar o código de barras em uma imagem
bool localizarCodigoBarras(const ImagemPBM *imagem, char *codigoExtraido) {
    for (int larguraColuna = 1; larguraColuna <= 80; larguraColuna++) {
        int larguraPadrao = 67 * larguraColuna; 
        if (larguraPadrao > imagem->largura) {
            break; 
        }

        // Varre todas as linhas e posições iniciais possíveis
        for (int linha = 0; linha < imagem->altura; linha++) {
            for (int inicio = 0; inicio <= imagem->largura - larguraPadrao; inicio++) {
                char bitsCodificados[68];
                extrairBitsLinha(imagem, linha, inicio, larguraColuna, bitsCodificados);

                char codigoDecimal[9];
                // Extrai e valida o código decimal do padrão de bits
                if (extrairCodigoOriginal(bitsCodificados, codigoDecimal)) {
                    if (verificarCodigo(codigoDecimal)) {
                        strcpy(codigoExtraido, codigoDecimal);
                        return true; // Retorna o código se encontrado
                    }
                }
            }
        }
    }
    return false; 
}

// Função principal
int main(int argc, char *argv[]) {
    // Verifica se o arquivo PBM foi especificado como argumento
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivoPBM>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nomeArquivo = argv[1];
    ResultadoCarregamento resultado = carregarImagemPBM(nomeArquivo);
    if (!resultado.sucesso) {
        fprintf(stderr, "Erro: não foi possível carregar a imagem PBM.\n");
        return EXIT_FAILURE;
    }

    ImagemPBM *imagem = resultado.imagem;
    char codigoExtraido[9];

    // Localiza o código de barras na imagem
    if (!localizarCodigoBarras(imagem, codigoExtraido)) {
        fprintf(stderr, "Erro: código de barras não encontrado.\n");
        free(imagem->pixels);
        free(imagem);
        return EXIT_FAILURE;
    }

    // Exibe o código de barras extraído
    printf("Código de barras extraído: %s\n", codigoExtraido);

    free(imagem->pixels);
    free(imagem);
    return EXIT_SUCCESS;
}
