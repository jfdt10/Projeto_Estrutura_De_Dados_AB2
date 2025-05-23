#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/sat_solver.h" // Para FormulaCNF, resolver_sat_com_arvore_otimizada, ler_formula_dimacs, liberar_formula_cnf

// main é o ponto de entrada do programa.
// argc (argument count) é o número de argumentos passados para o programa na linha de comando.
// argv (argument vector) é um array de strings, onde cada string é um argumento.
// argv[0] é o nome do programa, argv[1] é o primeiro argumento, e assim por diante.
int main(int argc, char *argv[]) {
    // Verifica se o número correto de argumentos foi fornecido (nome do programa + 1 arquivo).
    if (argc != 2) {
        // Se não, imprime uma mensagem de erro para o fluxo de erro padrão (stderr).
        // argv[0] é usado aqui para mostrar o nome do executável na mensagem de uso.
        fprintf(stderr, "Uso: %s <arquivo_entrada.cnf>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // O primeiro argumento (argv[1]) é esperado como o caminho para o arquivo CNF.
    const char *nome_arquivo = argv[1];
    
    FormulaCNF *formula = ler_formula_dimacs(nome_arquivo);
    
    if (!formula) {
        return EXIT_FAILURE;// Macro de stdlib.h que tem valor 1(indica falha).
    }

    int *atribuicoes = (int *)malloc((formula->numero_variaveis + 1) * sizeof(int));
    if (!atribuicoes) {
        fprintf(stderr, "Erro de alocação de memória para atribuições.\n");
        liberar_formula_cnf(formula);
        return EXIT_FAILURE;
    }

    bool satisfazivel = resolver_sat_com_arvore_otimizada(formula, atribuicoes);

    // Chama a função de impressão unificada em solver.c
    imprimir_solucao_sat(satisfazivel, atribuicoes, formula->numero_variaveis);

    free(atribuicoes);
    liberar_formula_cnf(formula);
    
    return EXIT_SUCCESS; // Macro de stdPPlib.h que tem valor 0(indica sucesso).
}