
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "funcoes.h"

// Estrutura para encapsular o resultado do carregamento da imagem
typedef struct {
    ImagemPBM *imagem; // Ponteiro para a imagem carregada
    bool sucesso;      // Indica se o carregamento foi bem-sucedido
} ResultadoCarregamento;

// Função para carregar o cabeçalho de um arquivo PBM
bool carregarCabecalhoPBM(FILE *arquivo, int *largura, int *altura) {
    char tipoArquivo[3];
    // Lê o tipo do arquivo e verifica se é um arquivo PBM válido
    if (fscanf(arquivo, "%2s", tipoArquivo) != 1 || strcmp(tipoArquivo, "P1") != 0) {
        fprintf(stderr, "Erro: formato PBM inválido ou não suportado.\n");
        return false;
    }
    // Lê as dimensões da imagem (largura e altura)
    if (fscanf(arquivo, "%d %d", largura, altura) != 2) {
        fprintf(stderr, "Erro ao ler dimensões da imagem.\n");
        return false;
    }
    return true;
}

// Função para carregar os pixels de uma imagem PBM
bool carregarPixelsPBM(FILE *arquivo, ImagemPBM *imagem) {
    // Itera sobre as linhas e colunas da imagem
    for (int i = 0; i < imagem->altura; i++) {
        for (int j = 0; j < imagem->largura; j++) {
            int pixel;
            // Lê o valor do pixel (0 ou 1) do arquivo
            if (fscanf(arquivo, "%d", &pixel) != 1) {
                fprintf(stderr, "Erro ao ler pixels da imagem.\n");
                return false;
            }
            // Armazena o pixel no array de pixels
            imagem->pixels[i * imagem->largura + j] = pixel;
        }
    }
    return true;
}

// Função principal para carregar uma imagem PBM de um arquivo
ResultadoCarregamento carregarImagemPBM(const char *caminhoArquivo) {
    // Abre o arquivo PBM para leitura
    FILE *arquivo = fopen(caminhoArquivo, "r");
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir o arquivo PBM: %s\n", caminhoArquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    int largura, altura;
    // Carrega o cabeçalho do arquivo PBM
    if (!carregarCabecalhoPBM(arquivo, &largura, &altura)) {
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    // Aloca memória para a estrutura da imagem
    ImagemPBM *imagem = malloc(sizeof(ImagemPBM));
    if (!imagem) {
        fprintf(stderr, "Erro: falha ao alocar memória para a imagem.\n");
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    // Inicializa as dimensões da imagem
    imagem->largura = largura;
    imagem->altura = altura;
    // Aloca memória para os pixels da imagem
    imagem->pixels = malloc(largura * altura * sizeof(int));
    if (!imagem->pixels) {
        fprintf(stderr, "Erro: falha ao alocar memória para os pixels.\n");
        free(imagem);
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    // Carrega os pixels do arquivo PBM
    if (!carregarPixelsPBM(arquivo, imagem)) {
        free(imagem->pixels);
        free(imagem);
        fclose(arquivo);
        return (ResultadoCarregamento){.imagem = NULL, .sucesso = false};
    }

    // Fecha o arquivo e retorna a estrutura com a imagem carregada
    fclose(arquivo);
    return (ResultadoCarregamento){.imagem = imagem, .sucesso = true};
}

// Função para extrair os bits de uma linha da imagem, representando o código de barras
void extrairBitsLinha(const ImagemPBM *imagem, int linha, int inicio, int larguraColuna, char *bitsCodificados) {
    // Itera sobre os 67 bits (tamanho padrão de um código de barras EAN-8)
    for (int bit = 0; bit < 67; bit++) {
        int contagemPreto = 0; // Conta o número de pixels pretos na área
        for (int deslocamento = 0; deslocamento < larguraColuna; deslocamento++) {
            int posX = inicio + bit * larguraColuna + deslocamento; // Posição do pixel na linha
            int indicePixel = linha * imagem->largura + posX; // Índice do pixel no array de pixels
            if (imagem->pixels[indicePixel] == 1) {
                contagemPreto++; // Incrementa se o pixel for preto
            }
        }
        // Define o bit como '1' ou '0' dependendo da proporção de pixels pretos
        bitsCodificados[bit] = (contagemPreto > (larguraColuna / 2)) ? '1' : '0';
    }
    bitsCodificados[67] = '\0'; // Finaliza a string de bits
}

// Função para localizar o código de barras em uma imagem
bool localizarCodigoBarras(const ImagemPBM *imagem, char *codigoExtraido) {
    // Testa diferentes larguras de coluna para localizar o padrão
    for (int larguraColuna = 1; larguraColuna <= 80; larguraColuna++) {
        int larguraPadrao = 67 * larguraColuna; // Largura total do padrão do código de barras
        if (larguraPadrao > imagem->largura) {
            break; // Se o padrão for maior que a largura da imagem, interrompe o loop
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
    return false; // Retorna false se o código de barras não for encontrado
}

// Função principal
int main(int argc, char *argv[]) {
    // Verifica se o arquivo PBM foi especificado como argumento
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivoPBM>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nomeArquivo = argv[1];
    // Carrega a imagem PBM
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

    // Libera a memória alocada para a imagem
    free(imagem->pixels);
    free(imagem);
    return EXIT_SUCCESS;
}
