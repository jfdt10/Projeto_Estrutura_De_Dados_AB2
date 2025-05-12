#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

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

void inicializar_lista(LISTA *l){

    l->inicio = NULL;
    l->tam = 0;
}

void inicializar_arvore(ARVORE *a){

    a->raiz = NULL;
    a->tam_arvore = 0;
}

void zerar_frequencia(unsigned int frequencia[]){

    for(int i = 0; i < 256; i++){ 

        frequencia[i] = 0;
    }

}

void imprimir_lista(LISTA *l){

    NO_LISTA *aux = l->inicio;

    while (aux){
        
        printf("freq: %u byte: %u\n", (aux->sub_arvore->frequencia), (aux->sub_arvore->byte));
        aux = aux->proximo;
    }
    
}

void inserir_na_lista_ordenado(LISTA *l, unsigned char valor_byte, unsigned int valor_frequencia, NO_HUFFMAN *no_novo){

    NO_LISTA *aux, *novo = malloc(sizeof(NO_LISTA));

    if(novo){
        novo->sub_arvore = malloc(sizeof(NO_HUFFMAN));
        
        if(novo->sub_arvore){

            novo->sub_arvore->byte = valor_byte;
            novo->sub_arvore->frequencia = valor_frequencia;
            novo->sub_arvore->direita = NULL;
            novo->sub_arvore->esquerda = NULL;

            if(no_novo != NULL){

                novo->sub_arvore->direita = no_novo->direita;
                novo->sub_arvore->esquerda = no_novo->esquerda;
            }

        }

        if(l->inicio == NULL){

            novo->proximo = NULL;
            l->inicio = novo;
        } else if(novo->sub_arvore->frequencia <= l->inicio->sub_arvore->frequencia){
            novo->proximo = l->inicio;
            l->inicio = novo;

        } else {
            aux = l->inicio;
            while(aux->proximo != NULL && novo->sub_arvore->frequencia > aux->proximo->sub_arvore->frequencia){
                aux = aux->proximo;
            }
            novo->proximo = aux->proximo;
            aux->proximo = novo;
        }

        l->tam++;
    }
    
}

NO_HUFFMAN* remover_do_inicio(LISTA *l){

    NO_HUFFMAN *removido = NULL;

    if(l->inicio == NULL){

        return NULL;
    } else {
        removido = l->inicio->sub_arvore;
        l->inicio = l->inicio->proximo;
        l->tam--;
        return removido;
    }
}

void criar_arvore_huffman(LISTA *l, ARVORE *a){

    if(l->inicio == NULL) return; // Lista vazia não retorna nada

    NO_LISTA *aux_lista = NULL; 

    a->tam_arvore = l->tam;

    while(l->tam > 1){

        aux_lista = malloc(sizeof(NO_LISTA));
        aux_lista->sub_arvore = malloc(sizeof(NO_HUFFMAN));

        aux_lista->sub_arvore->esquerda = remover_do_inicio(l); 

        aux_lista->sub_arvore->direita = remover_do_inicio(l);

        if(aux_lista->sub_arvore->esquerda == NULL || aux_lista->sub_arvore->direita == NULL){

            perror("Erro ao remover nós da lista");
            return;
        }

        unsigned int soma = (aux_lista->sub_arvore->esquerda->frequencia) + (aux_lista->sub_arvore->direita->frequencia);

        inserir_na_lista_ordenado(l, (unsigned char)'*', soma, aux_lista->sub_arvore);
        a->tam_arvore++;

    }

    a->raiz = l->inicio->sub_arvore; // tamanho da lista = 1 a raiz e igual ao NO que sobrou;
    return;

}

void imprimir_arvore(NO_HUFFMAN *no_arvore, FILE *saida){

    if(no_arvore == NULL) return; // Caso arvore esteja vazia

    if(no_arvore->esquerda == NULL && no_arvore->direita == NULL){ // Verifica se o NO é folha, NO folha: NO esquerdo e NO direito estão vazios 
        // É uma folha
        if(no_arvore->byte == '*' || no_arvore->byte == '\\'){
            fputc('\\', saida);
            fputc(no_arvore->byte, saida);
            
        } else {

            fputc(no_arvore->byte, saida);
        }
    } else {
        // Nó interno
        fputc('*', saida);
        imprimir_arvore(no_arvore->esquerda, saida);
        imprimir_arvore(no_arvore->direita, saida);
    }
}

char* converte_para_binario(unsigned int num, int tam){
    
    int resto = num;
    char *bits = calloc(tam, sizeof(char));
    
    for(int i = 0; i < tam; i++){
        
        bits[i] = '0';
    }
    
    int j = tam - 1;
    
    while(resto > 0){
        
        bits[j] =  (resto % 2) + '0';
        
        resto = resto / 2;
        
        j--;
    }
    
    return bits;
}

void inserir_bytes_no_arquivo(char *texto, int quantidade_bits, FILE *saida){

    unsigned char byte = 0;

    int aux = 7;

    for(int i = 0; i < quantidade_bits; i++){

        byte += (texto[i] - '0') * (int)(pow(2, aux));

        aux--;

        if(aux == -1){
            //printf("%u ", byte);
            fputc(byte, saida); // insere o byte na saida
            aux = 7;
            byte = 0;
        }

    }
    
}

int altura_arvore(NO_HUFFMAN *raiz){

    if(raiz == NULL){
        
        return -1;
    } else {
        
        int altura_esq = altura_arvore(raiz->esquerda) + 1;
        int altura_dir = altura_arvore(raiz->direita) + 1;

        if(altura_esq > altura_dir){

            return altura_esq;
        } else {

            return altura_dir;
        }
        
    }
}

char** aloca_tabela(int colunas){ // alloca memoria para tabela | colunas = altura da arvore 
    
    char **tabela;

    tabela = malloc(sizeof(char*) * 256);

    for(int i = 0; i < 256; i++){

        tabela[i] = calloc(colunas, sizeof(char));
    }

    return tabela;
}

void gerar_tabela(NO_HUFFMAN *no_arvore, char **tabela, char *caminho, int colunas){     

    if(no_arvore->esquerda == NULL && no_arvore->direita == NULL){ // Verifica se o NO é folha, NO folha: NO esquerdo e NO direito estão vazios 
        // É uma folha
        strcpy(tabela[no_arvore->byte], caminho); // copia o caminho para id do byte 
    } else {
        // Nó interno

        char esquerda[colunas];
        char direita[colunas];

        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_tabela(no_arvore->esquerda, tabela, esquerda, colunas);
        gerar_tabela(no_arvore->direita, tabela, direita, colunas);
        
    }

}

void imprimir_tabela(char **tabela){

    printf("\n\tTabela:\n");

    for(int i = 0; i < 256; i++){
        
        if(strlen(tabela[i]) > 0){

            printf("\t%c: %s\n", i, tabela[i]);
        }
    }
}

void liberar_tabela(char **tabela) {
    
    for (int i = 0; i < 256; i++) {
        
        free(tabela[i]);
    }

    free(tabela);
}

void escrever_arquivo_compactado(FILE *saida, int trash, unsigned int tam_arvore, NO_HUFFMAN *raiz, char *texto, unsigned int quant_bits_texto){

    char *bits_trash_mais_tam_arv = malloc(16 * sizeof(char)); // 3 bits para o lixo + 13 bits para tamanho da arvore | 16 bits = 2 bytes
    bits_trash_mais_tam_arv[0] = '\0';

    strcat(bits_trash_mais_tam_arv, converte_para_binario(trash, 3));

    //printf("%s ", bits_trash_mais_tam_arv);

    strcat(bits_trash_mais_tam_arv, converte_para_binario(tam_arvore, 13));

    //printf("%s ", bits_trash_mais_tam_arv);

    inserir_bytes_no_arquivo(bits_trash_mais_tam_arv, 16, saida);

    imprimir_arvore(raiz, saida);

    inserir_bytes_no_arquivo(texto, quant_bits_texto, saida);

    free(bits_trash_mais_tam_arv);
}

int main(int argc, char *argv[]){ // EX: ./argumentos.exe arquivo02.png | argc = 2 e argv[2] = {"C:\Users\...\argumentos.exe", "arquivo02.png"}

    if(argc < 2){ // tentar executar sem passar pelo menos um arquivo retorna erro

        perror("nenhum arquivo enviado");
        return 1;
    }

    FILE *arquivo; // Conteudo do arquivo
    LISTA lista; // Conteudo da lista
    ARVORE arv_huffman; // Conteudo da arvore

    unsigned int frequencia[256] = {0}; // unsigned int: inteiros maiores que zero
    unsigned char byte; // unsigned char vai de 0 a 255 

    char **tabela;
    char *texto = malloc(sizeof(char));
    texto[0] = '\0';

    for(int i = 1; i < argc; i++){

        arquivo = fopen(argv[i], "rb"); // Abre o arquivo e guarda na variavel

        if (!arquivo) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        inicializar_lista(&lista);
        inicializar_arvore(&arv_huffman);

        if(strstr(".huff", argv[i])){ // se .huff descompactar

            // descompactar();

        } else { // compactar

            // ==== INICIO ======================== FREQUENCIA DE CADA BYTE ==================================

            while (fread(&byte, 1, 1, arquivo) == 1) { // Ler e contabiliza a frequencia de cada byte 
                frequencia[byte]++;
            }

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR LISTA ENCADEADA ORDENADA ===========================

            for(int i = 0; i < 256; i++){ 

                if(frequencia[i] > 0){

                    inserir_na_lista_ordenado(&lista, i, frequencia[i], NULL);

                    //printf("freq: %u byte: %u\n", frequencia[i], i);
                }

                //printf("byte(%d): %d\n", i, frequencia[i]);
            }

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR ARVORE DE HUFFMAN =================================

            criar_arvore_huffman(&lista, &arv_huffman);

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR TABELA DE BITS ====================================

            int altura_arv = altura_arvore(arv_huffman.raiz);

            tabela = aloca_tabela(altura_arv);

            gerar_tabela(arv_huffman.raiz, tabela, "", altura_arv);

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR TEXTO COMPACTADO ====================================
            
            int tam_texto = 0; // tamanho do texto compactado

            fclose(arquivo);

            arquivo = fopen(argv[i], "rb"); // abre o arquivo novamente do inicio

            while (fread(&byte, 1, 1, arquivo) == 1) { // Ler e contabiliza a frequencia de cada byte 
                
                tam_texto += strlen(tabela[byte]);

                texto = realloc(texto, (tam_texto + 1) * sizeof(char));

                strcat(texto, tabela[byte]);
            }

            int trash = 8 - (tam_texto % 8); // bits lixo 

            if(trash == 8) trash = 0;

            tam_texto += trash;

            texto = realloc(texto, (tam_texto + 1) * sizeof(char));

            for(int i = trash; i > 0; i--){

                strcat(texto, "0");
            }

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR O ARQUIVO COMPACTADO ================================

            FILE *saida = fopen("Saida/arquivo_compactado.huff", "wb");

            escrever_arquivo_compactado(saida, trash, arv_huffman.tam_arvore, arv_huffman.raiz, texto, tam_texto);

            // ==== FIM =====================================================================================
            
            fclose(saida);
        }

        zerar_frequencia(frequencia);
        liberar_tabela(tabela);
        free(lista.inicio);
        free(arv_huffman.raiz);
        fclose(arquivo);
        

        //printf("\n");
    }

    return 0;
}