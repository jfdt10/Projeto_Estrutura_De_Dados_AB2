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

void escrever_arvore_no_arquivo(NO_HUFFMAN *no_arvore, FILE *saida){

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
        escrever_arvore_no_arquivo(no_arvore->esquerda, saida);
        escrever_arvore_no_arquivo(no_arvore->direita, saida);
    }
}

char* converte_para_binario(unsigned int num, int tam){
    
    int resto = num;
    char *bits = calloc((tam + 1), sizeof(char));
    
    for(int i = 0; i < tam; i++){
        
        bits[i] = '0';
    }
    
    int j = tam - 1;
    
    while(resto > 0){
        
        bits[j] =  (resto % 2) + '0';
        
        resto = resto / 2;
        
        j--;
    }

    bits[tam] = '\0';
    
    return bits;
}

unsigned int binario_para_unsigned_int(char *texto, int quantidade_bits){

    unsigned int soma = 0;

    int aux = quantidade_bits - 1;

    for(int i = 0; i < quantidade_bits; i++){

        soma += (texto[i] - '0') * (int)(pow(2, aux));

        aux--;
    }

    return soma;

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

    char *bits_trash_mais_tam_arv = malloc(17 * sizeof(char)); // 3 bits para o lixo + 13 bits para tamanho da arvore | 16 bits = 2 bytes || 1 espaço para '\0'
    bits_trash_mais_tam_arv[0] = '\0';

    char *bits_lixo_03 = converte_para_binario(trash, 3);
    char *bits_arvore_13 = converte_para_binario(tam_arvore, 13);

    strcat(bits_trash_mais_tam_arv, bits_lixo_03);
    strcat(bits_trash_mais_tam_arv, bits_arvore_13);

    inserir_bytes_no_arquivo(bits_trash_mais_tam_arv, 16, saida);

    escrever_arvore_no_arquivo(raiz, saida);

    inserir_bytes_no_arquivo(texto, quant_bits_texto, saida);

    free(bits_trash_mais_tam_arv);
    free(bits_lixo_03);
    free(bits_arvore_13);
}

NO_HUFFMAN* reconstruir_arvore(FILE *arquivo, int tam_arvore){

    unsigned char byte = 0;

    if(tam_arvore == 0){

        return NULL;
    }

    tam_arvore--;

    fread(&byte, 1, 1, arquivo);

    if(byte == '*') { // NO interno
        NO_HUFFMAN *no = malloc(sizeof(NO_HUFFMAN));
        no->byte = '*';
        no->esquerda = reconstruir_arvore(arquivo, tam_arvore);
        no->direita = reconstruir_arvore(arquivo, tam_arvore);
        return no;
    }

    if (byte == '\\') { // se encontrar o caracter de escape '\' lê o proximo caracter que pode ser a '\' ou '*'

        fread(&byte, 1, 1, arquivo);
    }

    NO_HUFFMAN *folha = malloc(sizeof(NO_HUFFMAN)); // se não for NO interno é NO folha
    folha->byte = byte;
    folha->esquerda = NULL;
    folha->direita = NULL;
    return folha;

}

void imprimir_pre_ordem(NO_HUFFMAN *raiz) {
    if (raiz == NULL) return;

    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        // Nó folha
        if (raiz->byte == '*' || raiz->byte == '\\') {
            printf("\\%c", raiz->byte);  // caractere de escape
        } else {
            printf("%c", raiz->byte);
        }
    } else {
        // Nó interno
        printf("*");
    }

    imprimir_pre_ordem(raiz->esquerda);
    imprimir_pre_ordem(raiz->direita);
}

void descompactar_texto(char *texto_comprimido, NO_HUFFMAN *raiz, int trash, FILE *saida){
    
    NO_HUFFMAN *aux = raiz;

    for(int i = 0; i < (strlen(texto_comprimido) - trash); i++){

        if(texto_comprimido[i] == '0'){

            aux = aux->esquerda;

        } else { // então texto_comprimido[i] == '1'

            aux = aux->direita;
        }

        if(aux->esquerda == NULL && aux->direita == NULL){ // NO FOLHA
            
            fputc(aux->byte, saida);
            aux = raiz;
        }
    }
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
    unsigned char bytes[4096]; // unsigned char vai de 0 a 255 
    size_t quant_bytes_lidos; // quantidade de bytes lida pelo fread

    unsigned char byte = 0;

    char **tabela;
    char *texto_compactado = malloc(sizeof(char));
    texto_compactado[0] = '\0';

    for(int i = 1; i < argc; i++){

        arquivo = fopen(argv[i], "rb"); // Abre o arquivo e guarda na variavel

        if (!arquivo) {
            perror("Erro ao abrir o arquivo");
            return 1;
        }

        inicializar_lista(&lista);
        inicializar_arvore(&arv_huffman);

        if(strstr(argv[i], ".huff")){ // se .huff descompactar

            int tam_arv = 0; // Guardar tamanho da arvore
            int trash = 0; // Guardar lixo

            // ==== INICIO ======================== PEGAR OS 2 PRIMEIROS BYTES (LIXO + TAM_ARVORE) ==================================

            char *byte_texto = malloc(14 * sizeof(char));
            byte_texto[0] = '\0';

            char bits_lixo[4];
            bits_lixo[0] = '\0';

            if(fread(&byte, 1, 1, arquivo) == 1){

                char *bits_8 = converte_para_binario(byte, 8);
                strcat(byte_texto, bits_8); // byte_texto recebe o byte ex: 10100000
                byte_texto[8] = '\0';

                strncpy(bits_lixo, byte_texto, 3); // copia os 3 primeiros bits para bits_lixo ex: 101
                bits_lixo[3] = '\0';

                memmove(byte_texto, byte_texto + 3, 5); // move os 5 bits restantes para o início de byte_texto
                byte_texto[5] = '\0';

                //memmove(byte_texto, byte_texto + 3, strlen(byte_texto + 3) + 1); // retira os 3 primeiros bits do lixo dos 8 bits ex: 00000

                trash = binario_para_unsigned_int(bits_lixo, 3);

                free(bits_8);
            }

            if(fread(&byte, 1, 1, arquivo) == 1){

                char *bits_8 = converte_para_binario(byte, 8);

                strcat(byte_texto, bits_8); // concatena outro byte
                byte_texto[13] = '\0';

                tam_arv = binario_para_unsigned_int(byte_texto, 13);

                free(bits_8);
            }

            //printf("%d, %d ", trash, tam_arv);
                
            // ==== FIM =====================================================================================

            // ==== INICIO ======================== RECONSTRUIR ARVORE ==============================================

            arv_huffman.raiz = reconstruir_arvore(arquivo, tam_arv);

            //imprimir_pre_ordem(arv_huffman.raiz);

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== LENDO OS BYTES COMPACTADOS ==============================================

            char *texto_comprimido = malloc(sizeof(char));
            texto_comprimido[0] = '\0';

            int tam_texto_comprimido = 0;

            while ((quant_bytes_lidos = fread(bytes, 1, 4096, arquivo)) > 0) {
                
                for (size_t i = 0; i < quant_bytes_lidos; i++) {

                    tam_texto_comprimido += 8;
                    
                    texto_comprimido = realloc(texto_comprimido, (tam_texto_comprimido + 1) * sizeof(char));

                    char *bits_8 = converte_para_binario(bytes[i], 8);

                    strcat(texto_comprimido, bits_8);

                    free(bits_8);
                }
            }

            //printf(" %s %d", texto_comprimido, strlen(texto_comprimido));

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== DESCOMPACTANDO TEXTO E GERAR ARQUIVO =============================

            FILE *saida = fopen("Saida/arquivo02.png", "wb");

            descompactar_texto(texto_comprimido, arv_huffman.raiz, trash, saida);

            // ==== FIM =====================================================================================

        } else { // compactar

            // ==== INICIO ======================== FREQUENCIA DE CADA BYTE ==================================

            while ((quant_bytes_lidos = fread(bytes, 1, 4096, arquivo)) > 0) {
                
                for (size_t i = 0; i < quant_bytes_lidos; i++) {
                    
                    frequencia[bytes[i]]++;
                }
            }

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR LISTA ENCADEADA ORDENADA ===========================

            for(int i = 0; i < 256; i++){ 

                if(frequencia[i] > 0){

                    inserir_na_lista_ordenado(&lista, i, frequencia[i], NULL);
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


            while ((quant_bytes_lidos = fread(bytes, 1, 4096, arquivo)) > 0) {
                
                for (size_t i = 0; i < quant_bytes_lidos; i++) {
                    
                    tam_texto += strlen(tabela[bytes[i]]);

                    texto_compactado = realloc(texto_compactado, (tam_texto + 1) * sizeof(char));

                    char *bits_8 = tabela[bytes[i]];

                    strcat(texto_compactado, bits_8);
                }
            }

            int trash = 8 - (tam_texto % 8); // bits lixo 

            if(trash == 8) trash = 0;

            tam_texto += trash;

            texto_compactado = realloc(texto_compactado, (tam_texto + 1) * sizeof(char));

            for(int i = trash; i > 0; i--){

                strcat(texto_compactado, "0");
            }

            // ==== FIM =====================================================================================

            // ==== INICIO ======================== GERAR O ARQUIVO COMPACTADO ================================

            char nome_saida[150];

            char base[100]; 

            strncpy(base, argv[i], (strlen(argv[i]) - strlen(strstr(argv[i], ".")))); // ex: se argv[i] = arquivo.png || base = arquivo

            base[strlen(base)] = '\0';

            sprintf(nome_saida, "Saida/%s.huff", base); // se base = arquivo || nome_saida = Saida/arquivo.huff

            FILE *saida = fopen(nome_saida, "wb");

            escrever_arquivo_compactado(saida, trash, arv_huffman.tam_arvore, arv_huffman.raiz, texto_compactado, tam_texto);

            // ==== FIM =====================================================================================

            trash = 0;
            tam_texto = 0;
            fclose(saida);
            liberar_tabela(tabela);
        }

        zerar_frequencia(frequencia);
        free(lista.inicio);
        free(arv_huffman.raiz);
        fclose(arquivo);
        
        //printf("\n");
    }

    return 0;
}