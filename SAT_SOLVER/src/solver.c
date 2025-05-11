#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/sat_solver.h" // Contém FormulaCNF, ArvoreDecisao, NoArvoreDecisao e protótipos atualizados

/**
 * @brief Função principal para resolver o problema SAT utilizando uma árvore de decisão com otimizações (DPLL).
 * Cria uma árvore de decisão, constrói-a recursivamente aplicando heurísticas como
 * propagação unitária e eliminação de literais puros. Se uma solução for encontrada,
 * as atribuições finais são copiadas.
 * @param formula Ponteiro para a estrutura FormulaCNF contendo o problema.
 * @param atribuicoes_finais Array de inteiros que será preenchido com a solução encontrada
 *                           (0 para falso, 1 para verdadeiro, 2 para não atribuído se INSAT).
 *                           O índice 0 não é usado; as variáveis são de 1 a numero_variaveis.
 * @return true se a fórmula for satisfatível (SAT), false caso contrário (UNSAT).
 */
bool resolver_sat_com_arvore_otimizada(FormulaCNF *formula, int *atribuicoes_finais) {
    if (!formula || !atribuicoes_finais) {
        return false;
    }

    if (formula->numero_variaveis == 0) {
        return true; 
    }

    // Inicializa todas as atribuições com um valor que indique "não atribuído" (ex: 2)
    // Isso é importante para a lógica da árvore de decisão e para a impressão final
    for (int i = 0; i <= formula->numero_variaveis; i++) { // <= para cobrir de 1 a N
        atribuicoes_finais[i] = 2; // 2 significa não atribuído
    }
    
    ArvoreDecisao *arvore = criar_arvore_para_resolucao(formula);
    if (!arvore) {
        return false;
    }

    bool tem_solucao = construir_arvore_recursivo(arvore->no_raiz, formula, formula->numero_variaveis);

    if (tem_solucao) {
        NoArvoreDecisao *no_solucao = buscar_solucao_na_arvore(arvore);
        if (no_solucao) {
            // Copia as atribuições do nó solução para o array final
            for (int i = 0; i < formula->numero_variaveis; i++) {
                int valor_final = no_solucao->atribuicoes_do_no[i];
                atribuicoes_finais[i + 1] = valor_final;
            }
        }
    }

    liberar_arvore_decisao(arvore);
    return tem_solucao;
}

/**
 * @brief Imprime a solução SAT encontrada para o formato padrão.
 * Imprime "SAT" seguido pelas atribuições de cada variável (ex: "1 = 1 2 = 0 ...").
 * Variáveis não atribuídas (valor 2) são convencionadas para 0 na saída.
 * @param atribuicoes Array de inteiros com as atribuições das variáveis (índice 1 a numero_variaveis).
 * @param numero_variaveis Número total de variáveis.
 */
void imprimir_solucao_sat(int *atribuicoes, int numero_variaveis) {
    printf("SAT\n");
    for (int i = 1; i <= numero_variaveis; i++) {
        int valor_final = (atribuicoes[i] == 1) ? 1 : 0;
        if (atribuicoes[i] == 2) { // Se explicitamente não atribuído (valor 2)
            valor_final = 0; // Convenciona para 0
        }
        printf("%d = %d", i, valor_final);
        if (i < numero_variaveis) {
            printf(" "); // Adiciona espaço entre as atribuições, mas não no final
        }
    }
    printf("\n");
}