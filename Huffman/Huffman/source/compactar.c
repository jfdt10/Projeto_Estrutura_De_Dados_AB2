#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "../headers/huffman.h"

void inicializar_lista(LISTA *l){

    l->inicio = NULL;
    l->tam = 0;
}

void inicializar_arvore(ARVORE *a){

    a->raiz = NULL;
    a->tam_arvore = 0;
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

        char esquerda[colunas]; // colunas = altura_arvore
        char direita[colunas];

        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_tabela(no_arvore->esquerda, tabela, esquerda, colunas);
        gerar_tabela(no_arvore->direita, tabela, direita, colunas);
        
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

