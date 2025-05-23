#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "../include/sat_solver.h"

// Limite máximo de nós para evitar estouro de memória
#define MAX_NOS 1000000UL

// Contador global de nós criados
unsigned long total_nos_criados = 0;

// Protótipos de funções estáticas (internas ao módulo)
static NoArvoreDecisao* buscar_solucao_recursivo(NoArvoreDecisao *no_atual);
static void liberar_no_recursivo(NoArvoreDecisao *no_atual);
static NoArvoreDecisao* alocar_no(int variavel, int valor, const int *atribuicoes_pai, int total_variaveis);
static bool eh_atribuicao_consistente(const int *atribuicoes, const FormulaCNF *formula);
static int escolher_proxima_variavel(const int *atribuicoes, int total_variaveis);

/**
 * @brief Verifica se uma atribuição parcial de variáveis é consistente com a fórmula CNF.
 * Uma atribuição é inconsistente se alguma cláusula se torna falsa (todos os seus literais
 * são falsos sob a atribuição atual) e todas as variáveis dessa cláusula já foram atribuídas.
 * @param atribuicoes Array de inteiros representando as atribuições atuais das variáveis.
 *                    (0 para falso, 1 para verdadeiro, 2 para não atribuído).
 * @param formula Ponteiro para a estrutura FormulaCNF contendo a fórmula.
 * @return true se a atribuição é consistente, false caso contrário.
 */
static bool eh_atribuicao_consistente(const int *atribuicoes, const FormulaCNF *formula) {
    if (!formula) {
        return false; 
    }

    for (int indice_clausula = 0; indice_clausula < formula->numero_clausulas; indice_clausula++) {
        bool clausula_satisfeita = false;
        bool todas_variaveis_atribuidas_na_clausula = true;
        
        if (!formula->clausulas_da_formula) {
            return false; 
        }
        if (!formula->clausulas_da_formula[indice_clausula]) {
            return false; 
        }

        for (int indice_literal = 0; ; indice_literal++) {
            int literal_atual = formula->clausulas_da_formula[indice_clausula][indice_literal];

            if (literal_atual == 0) { 
                break;
            }

            int variavel_atual = abs(literal_atual); 
            
            if (variavel_atual < 1 || variavel_atual > formula->numero_variaveis) {
                return false; 
            }

           
            if (atribuicoes[variavel_atual] == 2) { 
                todas_variaveis_atribuidas_na_clausula = false;
            } else {
                if ((literal_atual > 0 && atribuicoes[variavel_atual] == 1) ||
                    (literal_atual < 0 && atribuicoes[variavel_atual] == 0)) {
                    clausula_satisfeita = true;
                    break; 
                }
            }
        } 

        if (!clausula_satisfeita && todas_variaveis_atribuidas_na_clausula) {
            return false;
        }
    } 
    return true;
}

/**
 * @brief Escolhe a próxima variável a ser atribuída.
 * Esta é uma heurística simples: escolhe a primeira variável ainda não atribuída.
 * @param atribuicoes Array de inteiros representando as atribuições atuais.
 * @param total_variaveis Número total de variáveis na fórmula.
 * @return O índice (baseado em 1) da próxima variável a ser atribuída, ou 0 se todas estiverem atribuídas.
 */
static int escolher_proxima_variavel(const int *atribuicoes, int total_variaveis) {
    // Itera de 1 a total_variaveis
    for (int i = 1; i <= total_variaveis; i++) {
        if (atribuicoes[i] == 2) { 
            return i; 
        }
    }
    return 0; 
}

/**
 * @brief Realiza a propagação unitária.
 * Se uma cláusula tem apenas um literal não atribuído e todos os outros são falsos,
 * esse literal deve ser verdadeiro para satisfazer a cláusula. A função atribui
 * esse valor e repete o processo até que não haja mais cláusulas unitárias ou
 * ocorra um conflito.
 * @param formula Ponteiro para a estrutura FormulaCNF.
 * @param variaveis_atribuidas Array de atribuições a ser modificado.
 * @return true se a propagação unitária não levar a conflitos, false caso contrário.
 */
bool resolver_unitarias(FormulaCNF* formula, int* variaveis_atribuidas) {
    bool mudanca_feita_na_passada_geral;
    int iteracoes_passada_geral = 0; 
    do {
        mudanca_feita_na_passada_geral = false;
        iteracoes_passada_geral++;

        for (int i = 0; i < formula->numero_clausulas; i++) {
            int literal_unitario = 0;
            int var_unitaria = 0;
            int valor_unitario = -1; 
            int literais_nao_atribuidos = 0;
            bool clausula_satisfeita = false;
            int literais_falsos = 0;
            int total_literais_clausula = 0;

            for (int j = 0; formula->clausulas_da_formula[i][j] != 0; j++) {
                total_literais_clausula++;
                int literal = formula->clausulas_da_formula[i][j];
                int variavel = abs(literal);

                if (variavel <= 0 || variavel > formula->numero_variaveis) {
                    continue; 
                }

                if (variaveis_atribuidas[variavel] == 2) {
                    literais_nao_atribuidos++;
                    literal_unitario = literal; 
                } else {
                    bool atribuicao_correta = (literal > 0 && variaveis_atribuidas[variavel] == 1) ||
                                              (literal < 0 && variaveis_atribuidas[variavel] == 0);
                    if (atribuicao_correta) {
                        clausula_satisfeita = true;
                        break; 
                    } else {
                        literais_falsos++;
                    }
                }
            }

            if (!clausula_satisfeita && literais_nao_atribuidos == 1) { 
                var_unitaria = abs(literal_unitario);
                valor_unitario = (literal_unitario > 0) ? 1 : 0;

                if (variaveis_atribuidas[var_unitaria] == 2) {
                    variaveis_atribuidas[var_unitaria] = valor_unitario;
                    
                    mudanca_feita_na_passada_geral = true;

                   
                    if (!eh_atribuicao_consistente(variaveis_atribuidas, formula)) {
                        return false; 
                    }
                } else if (variaveis_atribuidas[var_unitaria] != valor_unitario) {
                    return false; 
                }
            } else if (!clausula_satisfeita && literais_nao_atribuidos == 0 && total_literais_clausula > 0 && literais_falsos == total_literais_clausula) {
                return false; 
            }
        } 
        if (iteracoes_passada_geral > formula->numero_variaveis + formula->numero_clausulas && mudanca_feita_na_passada_geral) { 
            break; 
        }

    } while (mudanca_feita_na_passada_geral);

    return true; 
}

/**
 * @brief Detecta e atribui literais puros na fórmula.
 * Um literal é puro se ele aparece apenas em sua forma positiva ou apenas em sua forma negativa
 * em todas as cláusulas não satisfeitas onde suas variáveis ainda não foram atribuídas.
 * Atribuir um valor a um literal puro de forma a satisfazer as cláusulas onde ele aparece
 * nunca impede uma solução.
 * @param formula Ponteiro para a estrutura FormulaCNF.
 * @param atribuicoes Array de atribuições a ser modificado.
 * @return true se algum literal puro foi encontrado e atribuído, false caso contrário.
 */
bool resolver_literais_puros(FormulaCNF *formula, int *atribuicoes) {
    bool atribuiu_algo = false;
    bool *aparece_positivo = (bool*)calloc(formula->numero_variaveis + 1, sizeof(bool));
    bool *aparece_negativo = (bool*)calloc(formula->numero_variaveis + 1, sizeof(bool));
    
    if (!aparece_positivo || !aparece_negativo) {
        if (aparece_positivo) free(aparece_positivo);
        if (aparece_negativo) free(aparece_negativo);
        return false;
    }
    
    for (int indice_clausula = 0; indice_clausula < formula->numero_clausulas; indice_clausula++) {
        bool clausula_satisfeita_pela_atribuicao_atual = false;
        for (int indice_literal = 0; formula->clausulas_da_formula[indice_clausula][indice_literal] != 0; indice_literal++) {
            int literal_atual = formula->clausulas_da_formula[indice_clausula][indice_literal];
            int variavel_atual = abs(literal_atual); 

            if (variavel_atual < 1 || variavel_atual > formula->numero_variaveis) {
                free(aparece_positivo);
                free(aparece_negativo);
                return false;
            }

            if (atribuicoes[variavel_atual] != 2) { 
                if ((literal_atual > 0 && atribuicoes[variavel_atual] == 1) || 
                    (literal_atual < 0 && atribuicoes[variavel_atual] == 0)) { 
                    clausula_satisfeita_pela_atribuicao_atual = true;
                    break;
                }
            }
        }
        
        if (!clausula_satisfeita_pela_atribuicao_atual) {
            for (int indice_literal = 0; formula->clausulas_da_formula[indice_clausula][indice_literal] != 0; indice_literal++) {
                int literal_atual = formula->clausulas_da_formula[indice_clausula][indice_literal];
                int variavel_atual = abs(literal_atual); 

                if (atribuicoes[variavel_atual] == 2) { 
                    if (literal_atual > 0) {
                        aparece_positivo[variavel_atual] = true;
                    } else {
                        aparece_negativo[variavel_atual] = true;
                    }
                }
            }
        }
    }
    
    for (int variavel_id = 1; variavel_id <= formula->numero_variaveis; variavel_id++) { 
        if (atribuicoes[variavel_id] == 2) { 
            if (aparece_positivo[variavel_id] && !aparece_negativo[variavel_id]) {
                atribuicoes[variavel_id] = 1;
                atribuiu_algo = true;
            } else if (!aparece_positivo[variavel_id] && aparece_negativo[variavel_id]) {
                atribuicoes[variavel_id] = 0;
                atribuiu_algo = true;
            }
        }
    }
    
    free(aparece_positivo);
    free(aparece_negativo);
    return atribuiu_algo;
}

/**
 * @brief Aloca e inicializa um novo nó da árvore de decisão.
 * @param variavel A variável que este nó representa uma decisão.
 * @param valor O valor atribuído à variável (0 ou 1).
 * @param atribuicoes_pai Array de atribuições do nó pai.
 * @param total_variaveis Número total de variáveis na fórmula.
 * @return Ponteiro para o novo nó alocado, ou NULL se a alocação falhar ou o limite de nós for atingido.
 */
NoArvoreDecisao* alocar_no(int variavel, int valor, const int *atribuicoes_pai, int total_variaveis) {
    if (total_nos_criados >= MAX_NOS) {
        return NULL;
    }

    NoArvoreDecisao *novo_no = (NoArvoreDecisao*)malloc(sizeof(NoArvoreDecisao));
    if (!novo_no) {
        perror("Erro ao alocar memória para NoArvoreDecisao");
        return NULL;
    }
    total_nos_criados++;

    // Alocar para total_variaveis + 1 para usar índices de 1 a total_variaveis
    novo_no->atribuicoes_do_no = (int*)malloc((total_variaveis + 1) * sizeof(int));
    if (!novo_no->atribuicoes_do_no) {
        perror("Erro ao alocar memória para atribuições do nó");
        free(novo_no);
        return NULL;
    }

    novo_no->variavel_do_no = variavel; 
    novo_no->valor_atribuido_no = valor;
    novo_no->eh_no_solucao = false;
    novo_no->no_esquerdo = NULL;
    novo_no->no_direito = NULL;




    if (atribuicoes_pai) {
        memcpy(novo_no->atribuicoes_do_no, atribuicoes_pai, (total_variaveis + 1) * sizeof(int));
    } else {
        // Para o nó raiz ou se não houver pai, inicializa todas as vars (1 a N) como não atribuídas (2)
        for (int i = 1; i <= total_variaveis; i++) {
            novo_no->atribuicoes_do_no[i] = 2; // 2 = não atribuído
        }
    }

    if (variavel > 0 && variavel <= total_variaveis) {
        novo_no->atribuicoes_do_no[variavel] = valor;
    }

    return novo_no;
}

/**
 * @brief Cria e inicializa a árvore de decisão para a resolução do SAT.
 * A raiz da árvore representa o estado inicial sem nenhuma variável atribuída.
 * @param formula Ponteiro para a estrutura FormulaCNF.
 * @return Ponteiro para a árvore de decisão criada, ou NULL se a alocação falhar.
 */
ArvoreDecisao* criar_arvore_para_resolucao(FormulaCNF *formula) {
    if (!formula) {
        return NULL;
    }

    ArvoreDecisao *arvore = (ArvoreDecisao*)malloc(sizeof(ArvoreDecisao));
    if (!arvore) {
        return NULL;
    }
    arvore->numero_variaveis_formula = formula->numero_variaveis;

    int *atribuicoes_iniciais = (int*)malloc((formula->numero_variaveis + 1) * sizeof(int));
    if (!atribuicoes_iniciais) {
        free(arvore);
        return NULL;
    }
   
    for (int i = 1; i <= formula->numero_variaveis; i++) {
        atribuicoes_iniciais[i] = 2; 
    }

    
    arvore->no_raiz = alocar_no(0, 0, atribuicoes_iniciais, formula->numero_variaveis);
    free(atribuicoes_iniciais);

    if (!arvore->no_raiz) {
        free(arvore);
        return NULL;
    }
    return arvore;
}

/**
 * @brief Função recursiva principal para construir a árvore de decisão e encontrar uma solução (DPLL).
 * Explora o espaço de busca de atribuições de variáveis. Para cada variável não atribuída,
 * tenta atribuir verdadeiro (1) e depois falso (0), recursivamente.
 * Aplica propagação unitária e eliminação de literais puros em cada nó.
 * @param no_atual O nó atual na árvore de decisão sendo explorado.
 * @param formula Ponteiro para a estrutura FormulaCNF.
 * @param total_variaveis Número total de variáveis na fórmula.
 * @return true se uma solução for encontrada a partir deste nó, false caso contrário.
 */
// Implementa o algoritmo DPLL recursivamente para encontrar uma solução SAT.
bool construir_arvore_recursivo(NoArvoreDecisao *no_atual, FormulaCNF *formula, int total_variaveis) {
    // Verifica se a atribuição atual é consistente com a fórmula.
    if (!eh_atribuicao_consistente(no_atual->atribuicoes_do_no, formula)) {
        return false; // Conflito: caminho insatisfatível.
    }

    // Aplica propagação unitária para forçar atribuições.
    if (!resolver_unitarias(formula, no_atual->atribuicoes_do_no)) {
        return false; // Conflito durante a propagação unitária.
    }

    // Aplica eliminação de literais puros.
    resolver_literais_puros(formula, no_atual->atribuicoes_do_no);
    
    // Re-verifica consistência após as simplificações.
    if (!eh_atribuicao_consistente(no_atual->atribuicoes_do_no, formula)) {
        return false;
    }

    // Escolhe a próxima variável para ramificação.
    int proxima_variavel = escolher_proxima_variavel(no_atual->atribuicoes_do_no, total_variaveis);

    // Se não há mais variáveis para atribuir, é uma solução.
    if (proxima_variavel == 0) { 
        no_atual->eh_no_solucao = true; 
        return true; 
    }

    // Valida a variável escolhida.
    if (proxima_variavel < 1 || proxima_variavel > total_variaveis) {
        return false; // Erro na escolha da variável.
    }
    
    // Tenta atribuir VERDADEIRO (1) para a próxima variável e explora recursivamente.
    no_atual->no_esquerdo = alocar_no(proxima_variavel, 1, no_atual->atribuicoes_do_no, total_variaveis);
    if (no_atual->no_esquerdo) {
        if (construir_arvore_recursivo(no_atual->no_esquerdo, formula, total_variaveis)) {
            return true; 
        } else {
            // Backtrack: libera o nó do ramo esquerdo se não houver solução.
            liberar_no_recursivo(no_atual->no_esquerdo);
            no_atual->no_esquerdo = NULL;
        }
    }

    // Se o ramo VERDADEIRO falhou, tenta atribuir FALSO (0) e explora recursivamente.
    no_atual->no_direito = alocar_no(proxima_variavel, 0, no_atual->atribuicoes_do_no, total_variaveis);
    if (no_atual->no_direito) {
        if (construir_arvore_recursivo(no_atual->no_direito, formula, total_variaveis)) {
            return true; 
        } else {
            // Backtrack: libera o nó do ramo direito se não houver solução.
            liberar_no_recursivo(no_atual->no_direito);
            no_atual->no_direito = NULL;
        }
    }
    
    // Nenhum dos ramos levou a uma solução.
    return false; // Backtrack: Nenhuma solução a partir deste nó.
}

/**
 * @brief Função auxiliar recursiva para buscar um nó de solução na árvore.
 * Percorre a árvore em profundidade.
 * @param no_atual O nó atual sendo verificado.
 * @return Ponteiro para o primeiro nó de solução encontrado, ou NULL se nenhum for encontrado.
 */
static NoArvoreDecisao* buscar_solucao_recursivo(NoArvoreDecisao *no_atual) {
    if (!no_atual) {
        return NULL;
    }
    if (no_atual->eh_no_solucao) {
        return no_atual;
    }
    NoArvoreDecisao *solucao_encontrada = buscar_solucao_recursivo(no_atual->no_esquerdo);
    if (solucao_encontrada) {
        return solucao_encontrada;
    }
    return buscar_solucao_recursivo(no_atual->no_direito);
}

/**
 * @brief Busca um nó de solução na árvore de decisão.
 * @param arvore Ponteiro para a árvore de decisão.
 * @return Ponteiro para um nó de solução, ou NULL se a árvore for inválida ou não houver solução.
 */
NoArvoreDecisao* buscar_solucao_na_arvore(ArvoreDecisao *arvore) {
    if (!arvore || !arvore->no_raiz) {
        return NULL;
    }
    return buscar_solucao_recursivo(arvore->no_raiz);
}

/**
 * @brief Libera recursivamente a memória de um nó e todos os seus descendentes.
 * Também decrementa o contador global de nós criados.
 * @param no_atual O nó a ser liberado.
 */
static void liberar_no_recursivo(NoArvoreDecisao *no_atual) {
    if (!no_atual) {
        return;
    }
    liberar_no_recursivo(no_atual->no_esquerdo);
    liberar_no_recursivo(no_atual->no_direito);
    
    if (no_atual->atribuicoes_do_no) { 
        free(no_atual->atribuicoes_do_no);
        no_atual->atribuicoes_do_no = NULL; 
    }
    free(no_atual);
    
    if (total_nos_criados > 0) { 
        total_nos_criados--;
    }
}

/**
 * @brief Libera toda a memória alocada para a árvore de decisão, incluindo todos os nós.
 * @param arvore Ponteiro para a árvore de decisão a ser liberada.
 */
void liberar_arvore_decisao(ArvoreDecisao *arvore) {
    if (!arvore) {
        return;
    }
    liberar_no_recursivo(arvore->no_raiz);
    free(arvore);
}
