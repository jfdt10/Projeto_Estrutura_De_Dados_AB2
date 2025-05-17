#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED

typedef struct no_huffman{
    unsigned char byte;
    unsigned int frequencia;
    struct no_huffman *esquerda;
    struct no_huffman *direita;
}NO_HUFFMAN;

typedef struct{
    NO_HUFFMAN *raiz;
    unsigned int tam_arvore;
}ARVORE;

typedef struct no{
    NO_HUFFMAN *sub_arvore;
    struct no *proximo;
}NO_LISTA;

typedef struct{
    NO_LISTA *inicio;
    int tam; 
}LISTA;

void inicializar_lista(LISTA *l);

void inicializar_arvore(ARVORE *a);

void inserir_na_lista_ordenado(LISTA *l, unsigned char valor_byte, unsigned int valor_frequencia, NO_HUFFMAN *no_novo);

NO_HUFFMAN* remover_do_inicio(LISTA *l);

void criar_arvore_huffman(LISTA *l, ARVORE *a);

void escrever_arvore_no_arquivo(NO_HUFFMAN *no_arvore, FILE *saida);

int altura_arvore(NO_HUFFMAN *raiz);

// Funções de converção (unsigned int para binario) e (binario para usigned int)

char* converte_para_binario(unsigned int num, int tam);

unsigned int binario_para_unsigned_int(char *texto, int quantidade_bits);

// ========== Função para inserir byte a byte no arquivo ===================

void inserir_bytes_no_arquivo(char *texto, int quantidade_bits, FILE *saida);

// =====  Tabela dos caminhos ate um byte da arvore de huffman =============

char** aloca_tabela(int colunas);

void gerar_tabela(NO_HUFFMAN *no_arvore, char **tabela, char *caminho, int colunas);

void imprimir_tabela(char **tabela);

void liberar_tabela(char **tabela);

// ==================== Escrever o Arquivo Compactado =======================

void escrever_arquivo_compactado(FILE *saida, int trash, unsigned int tam_arvore, NO_HUFFMAN *raiz, char *texto, unsigned int quant_bits_texto);

// ==================== Reconstruir Arvore =======================

NO_HUFFMAN* reconstruir_arvore(FILE *arquivo, int tam_arvore);

// ==================== Escrever o Arquivo Descompactado =======================

void escrever_arquivo_descompactado(char *texto_comprimido, NO_HUFFMAN *raiz, int trash, FILE *saida);

#endif // HUFFMAN_H_INCLUDED