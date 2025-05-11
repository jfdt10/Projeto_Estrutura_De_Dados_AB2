#ifndef SOLVER_H
#define SOLVER_H

#include <stdbool.h>
#include "../include/sat_solver.h" // Contém FormulaCNF e outras declarações

/**
 * @brief Tenta resolver a fórmula SAT usando um algoritmo baseado em árvore de decisão (DPLL).
 * Esta é uma declaração de função; a implementação está em solver.c.
 * @param formula Ponteiro para a FormulaCNF a ser resolvida.
 * @param atribuicoes Ponteiro para um array de inteiros que será preenchido com a atribuição satisfatória, se encontrada.
 * @return true se a fórmula for satisfatível (SAT), false caso contrário (UNSAT).
 */
bool resolver_sat_com_arvore_otimizada(FormulaCNF *formula, int *atribuicoes);

/**
 * @brief Imprime a solução SAT (atribuições de variáveis) no formato padrão.
 * Esta é uma declaração de função; a implementação está em solver.c.
 * @param atribuicoes Array com as atribuições das variáveis.
 * @param numero_variaveis Número total de variáveis.
 */
void imprimir_solucao_sat(int *atribuicoes, int numero_variaveis);

#endif // SOLVER_H