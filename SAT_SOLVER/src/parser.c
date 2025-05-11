#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/sat_solver.h" // Assumes FormulaCNF is defined here

/**
 * @brief Lê uma fórmula CNF de um arquivo no formato DIMACS.
 * Tenta abrir o arquivo diretamente, depois com prefixos "test/" e "test/test_cases/".
 * Realiza duas passagens: a primeira para ler a linha 'p cnf' e obter o número de variáveis e cláusulas,
 * e a segunda para ler as cláusulas em si.
 * @param nome_arquivo O caminho para o arquivo DIMACS.
 * @return Ponteiro para uma estrutura FormulaCNF preenchida, ou NULL em caso de erro.
 */
FormulaCNF* ler_formula_dimacs(const char *nome_arquivo) { 
    FILE *arquivo = fopen(nome_arquivo, "r"); 
    
    // Se o arquivo não puder ser aberto, tenta caminhos alternativos
    if (!arquivo) {
        char caminho_alternativo[256]; 
        
        // Tenta com o prefixo "test/"
        snprintf(caminho_alternativo, sizeof(caminho_alternativo), "test/%s", nome_arquivo);
        arquivo = fopen(caminho_alternativo, "r");
        
        // Se ainda falhar, tenta com o prefixo "test/test_cases/"
        if (!arquivo) {
            snprintf(caminho_alternativo, sizeof(caminho_alternativo), "test/test_cases/%s", nome_arquivo);
            arquivo = fopen(caminho_alternativo, "r");
        }
    }
    
    if (!arquivo) {
        fprintf(stderr, "Erro ao abrir arquivo: %s\n", nome_arquivo); 
        return NULL;
    }

    FormulaCNF *formula = malloc(sizeof(FormulaCNF)); 
    if (!formula) {
        fclose(arquivo);
        return NULL;
    }
    
    formula->numero_variaveis = 0;    
    formula->numero_clausulas = 0;    
    formula->clausulas_da_formula = NULL; 

    // Primeira passagem: conta variáveis e cláusulas
    char linha[1024]; 
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == 'c') {
            continue; 
        }
        if (linha[0] == 'p') {
            sscanf(linha, "p cnf %d %d", &formula->numero_variaveis, &formula->numero_clausulas);
            break;
        }
    }
    
    if (formula->numero_variaveis == 0 || formula->numero_clausulas == 0) {
        fprintf(stderr, "Erro: Linha 'p cnf' não encontrada ou formato inválido.\n");
        fclose(arquivo);
        free(formula);
        return NULL; 
    }
    
    formula->clausulas_da_formula = malloc(formula->numero_clausulas * sizeof(int*));
    if (!formula->clausulas_da_formula) {
        fprintf(stderr, "Erro ao alocar memória para ponteiros de cláusulas.\n");
        fclose(arquivo);
        free(formula);
        return NULL;
    }
    
    rewind(arquivo);
    
    int indice_clausula = 0; 
    
    int* acumulador_literais = malloc( (formula->numero_variaveis + 1) * sizeof(int) ); 
    if (!acumulador_literais) {
        fprintf(stderr, "Erro ao alocar buffer acumulador de literais.\n");
        free(formula->clausulas_da_formula);
        free(formula);
        fclose(arquivo);
        return NULL;
    }
    int num_literais_acumulados = 0;
    char linha_para_aviso[1024];

    while (fgets(linha, sizeof(linha), arquivo) && indice_clausula < formula->numero_clausulas) {
        if (linha[0] == 'c' || linha[0] == 'p') {
            continue; 
        }
        strncpy(linha_para_aviso, linha, sizeof(linha_para_aviso) - 1);
        linha_para_aviso[sizeof(linha_para_aviso) - 1] = '\0';
        char *newline_char = strchr(linha_para_aviso, '\n');
        if (newline_char) *newline_char = '\0';
        newline_char = strchr(linha_para_aviso, '\r');
        if (newline_char) *newline_char = '\0';

        char *copia_linha_para_literais = strdup(linha);
        if (!copia_linha_para_literais) {
            fprintf(stderr, "Erro ao alocar memória para copia_linha_para_literais em ler_formula_dimacs.\n");
            for (int i = 0; i < indice_clausula; ++i) {
                if(formula->clausulas_da_formula[i]) free(formula->clausulas_da_formula[i]);
            }
            free(formula->clausulas_da_formula);
            free(acumulador_literais);
            free(formula);
            fclose(arquivo);
            return NULL;
        }
        
        char *token = strtok(copia_linha_para_literais, " \t\n\r");
        while (token) {
            int literal_lido = atoi(token);
            
            if (num_literais_acumulados >= formula->numero_variaveis +1) { 
                 fprintf(stderr, "[ERROR parser] Cláusula %d (linha: '%s') excedeu o buffer de literais (%d). Abortando.\n",
                        indice_clausula + 1, linha_para_aviso, formula->numero_variaveis + 1);
                free(copia_linha_para_literais);
                for (int i = 0; i < indice_clausula; ++i) if(formula->clausulas_da_formula[i]) free(formula->clausulas_da_formula[i]);
                free(formula->clausulas_da_formula);
                free(acumulador_literais);
                free(formula);
                fclose(arquivo);
                return NULL;
            }

            acumulador_literais[num_literais_acumulados++] = literal_lido;

            if (literal_lido == 0) { 
                int literais_validos_na_clausula_atual[num_literais_acumulados];
                int contagem_validos = 0;
                for(int k=0; k < num_literais_acumulados; ++k) {
                    int l_val = acumulador_literais[k];
                    int var_abs = abs(l_val);
                    if (l_val == 0) { 
                        literais_validos_na_clausula_atual[contagem_validos++] = 0;
                        break; 
                    }
                    if (var_abs >= 1 && var_abs <= formula->numero_variaveis) {
                        literais_validos_na_clausula_atual[contagem_validos++] = l_val;
                    }
                }
                
                if (contagem_validos > 0) { 
                    formula->clausulas_da_formula[indice_clausula] = malloc(contagem_validos * sizeof(int));
                    if (!formula->clausulas_da_formula[indice_clausula]) {
                        fprintf(stderr, "Erro ao alocar memória para literais da cláusula %d\n", indice_clausula + 1);
                        free(copia_linha_para_literais);
                        for (int i = 0; i < indice_clausula; ++i) if(formula->clausulas_da_formula[i]) free(formula->clausulas_da_formula[i]);
                        free(formula->clausulas_da_formula);
                        free(acumulador_literais);
                        free(formula);
                        fclose(arquivo);
                        return NULL;
                    }
                    memcpy(formula->clausulas_da_formula[indice_clausula], literais_validos_na_clausula_atual, contagem_validos * sizeof(int));

                    indice_clausula++;
                } else if (num_literais_acumulados > 0 && acumulador_literais[0] == 0) { 
                     formula->clausulas_da_formula[indice_clausula] = malloc(sizeof(int));
                     formula->clausulas_da_formula[indice_clausula][0] = 0;
                     indice_clausula++;
                }
                
                num_literais_acumulados = 0; 
                if (indice_clausula >= formula->numero_clausulas) break; 
            }
            token = strtok(NULL, " \t\n\r");
        }
        free(copia_linha_para_literais);
        if (indice_clausula >= formula->numero_clausulas) break; 
    }
    free(acumulador_literais);
    
    if (indice_clausula < formula->numero_clausulas) {
        formula->numero_clausulas = indice_clausula; 
    }
    
    fclose(arquivo);
    return formula;
}

/**
 * @brief Libera a memória alocada para uma estrutura FormulaCNF.
 * Percorre todas as cláusulas, liberando a memória de cada uma, depois libera o array de ponteiros
 * para as cláusulas e, finalmente, a própria estrutura da fórmula.
 * @param formula Ponteiro para a FormulaCNF a ser liberada.
 */
void liberar_formula_cnf(FormulaCNF *formula) { 
    if (!formula) return;
    
    if (formula->clausulas_da_formula) {
        for (int i = 0; i < formula->numero_clausulas; i++) {
            if (formula->clausulas_da_formula[i]) {
                free(formula->clausulas_da_formula[i]);
            }
        }
        free(formula->clausulas_da_formula);
    }
    
    free(formula);
}