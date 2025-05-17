#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "../headers/huffman.h"

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

void escrever_arquivo_descompactado(char *texto_comprimido, NO_HUFFMAN *raiz, int trash, FILE *saida){
    
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
