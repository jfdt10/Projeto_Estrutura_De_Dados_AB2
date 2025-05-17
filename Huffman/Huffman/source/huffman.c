#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "../headers/huffman.h"

void zerar_frequencia(unsigned int frequencia[]){

    for(int i = 0; i < 256; i++){ 

        frequencia[i] = 0;
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

                memmove(byte_texto, byte_texto + 3, 5); // move os 5 bits restantes para o início de byte_texto ex: 00000
                byte_texto[5] = '\0';

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

            FILE *saida = fopen("../Saida/arquivo02.png", "wb");

            escrever_arquivo_descompactado(texto_comprimido, arv_huffman.raiz, trash, saida);

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

            sprintf(nome_saida, "../Saida/%s.huff", base); // se base = arquivo || nome_saida = Saida/arquivo.huff

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