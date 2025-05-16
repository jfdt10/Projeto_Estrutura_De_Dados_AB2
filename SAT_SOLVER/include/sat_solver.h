#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <stdbool.h>

// Estrutura para uma fórmula CNF
/**
 * @struct FormulaCNF
 * @brief Representa uma fórmula lógica na Forma Normal Conjuntiva (CNF).
 * @var FormulaCNF::numero_variaveis Número total de variáveis na fórmula.
 * @var FormulaCNF::numero_clausulas Número total de cláusulas na fórmula.
 * @var FormulaCNF::clausulas_da_formula Um array de ponteiros para inteiros. Cada ponteiro aponta
 *                                     para um array de inteiros representando uma cláusula.
 *                                     Cada cláusula é uma lista de literais terminada por 0.
 *                                     Um literal positivo `x` é representado por `x`.
 *                                     Um literal negativo `¬x` é representado por `-x`.
 */
typedef struct {
    int numero_variaveis;
    int numero_clausulas;
    int **clausulas_da_formula; //Array de Cláusulas, onde cada clausula é um array de inteiros e termina com 0
} FormulaCNF;

// Estrutura para um nó da árvore binária de decisão
/**
 * @struct NoArvoreDecisao
 * @brief Representa um nó em uma árvore de decisão para o algoritmo DPLL.
 * @var NoArvoreDecisao::variavel_do_no A variável que foi decidida (ramificada) neste nó.
 * @var NoArvoreDecisao::valor_atribuido_no O valor (0 ou 1) atribuído à variavel_do_no para criar este nó.
 * @var NoArvoreDecisao::atribuicoes_do_no Um array representando o estado completo das atribuições de todas
 *                                        as variáveis neste ponto da árvore. (0=falso, 1=verdadeiro, 2=não atribuído).
 * @var NoArvoreDecisao::eh_no_solucao Flag booleana que indica se este nó representa uma atribuição completa e satisfatória.
 * @var NoArvoreDecisao::profundidade_do_no A profundidade do nó na árvore (raiz = 0).
 * @var NoArvoreDecisao::no_esquerdo Ponteiro para o filho esquerdo (geralmente representa a atribuição da próxima variável como verdadeira).
 * @var NoArvoreDecisao::no_direito Ponteiro para o filho direito (geralmente representa a atribuição da próxima variável como falsa).
 */
typedef struct NoArvoreDecisao {
    int variavel_do_no;              // Variável sendo atribuída neste nó
    int valor_atribuido_no;            // Valor atribuído (0 ou 1)
    int *atribuicoes_do_no;      // Array com a atribuição atual
    bool eh_no_solucao;     // Indica se este nó é uma solução
    int profundidade_do_no;          // Profundidade do nó na árvore

    struct NoArvoreDecisao *no_esquerdo;  // Filho esquerdo (próxima variável = 1)
    struct NoArvoreDecisao *no_direito;   // Filho direito (próxima variável = 0)
} NoArvoreDecisao;

// Estrutura para a árvore binária de decisão
/**
 * @struct ArvoreDecisao
 * @brief Representa a árvore de decisão completa usada pelo solver SAT.
 * @var ArvoreDecisao::no_raiz Ponteiro para o nó raiz da árvore.
 * @var ArvoreDecisao::numero_variaveis_formula O número total de variáveis na fórmula CNF que esta árvore está tentando resolver.
 */
typedef struct {
    NoArvoreDecisao *no_raiz;       // Raiz da árvore
    int numero_variaveis_formula;         // Número de variáveis na fórmula
} ArvoreDecisao;

// Funções do parser
/**
 * @brief Lê uma fórmula CNF de um arquivo no formato DIMACS.
 * @param nome_arquivo O nome do arquivo a ser lido.
 * @return Ponteiro para uma estrutura FormulaCNF preenchida, ou NULL em caso de erro.
 */
FormulaCNF* ler_formula_dimacs(const char *nome_arquivo);
/**
 * @brief Libera a memória alocada para uma estrutura FormulaCNF.
 * @param formula Ponteiro para a FormulaCNF a ser liberada.
 */
void liberar_formula_cnf(FormulaCNF *formula);

// Funções do solver
/**
 * @brief Tenta resolver a fórmula SAT usando um algoritmo baseado em árvore de decisão (DPLL).
 * @param formula Ponteiro para a FormulaCNF a ser resolvida.
 * @param atribuicoes Ponteiro para um array de inteiros que será preenchido com a atribuição satisfatória, se encontrada.
 *                  As atribuições são para as variáveis de 1 a `formula->numero_variaveis`.
 * @return true se a fórmula for satisfatível (SAT), false caso contrário (UNSAT).
 */
bool resolver_sat_com_arvore_otimizada(FormulaCNF *formula, int *atribuicoes);
/**
 * @brief Imprime o resultado da verificação SAT.
 * Se satisfatível, imprime "SAT!" seguido pelas atribuições de cada variável.
 * Se insatisfatível, imprime "UNSAT!".
 * @param satisfazivel Booleano indicando se a fórmula é satisfatível.
 * @param atribuicoes Array com as atribuições das variáveis (usado apenas se SAT).
 * @param numero_variaveis Número total de variáveis (usado apenas se SAT).
 */
void imprimir_solucao_sat(bool satisfazivel, int *atribuicoes, int numero_variaveis);

// Funções da árvore binária
/**
 * @brief Cria a estrutura inicial da árvore de decisão para uma dada fórmula.
 * @param formula Ponteiro para a FormulaCNF.
 * @return Ponteiro para a ArvoreDecisao criada, ou NULL em caso de erro.
 */
ArvoreDecisao* criar_arvore_para_resolucao(FormulaCNF *formula);
/**
 * @brief Libera toda a memória alocada para uma árvore de decisão.
 * @param arvore Ponteiro para a ArvoreDecisao a ser liberada.
 */
void liberar_arvore_decisao(ArvoreDecisao *arvore);
/**
 * @brief Busca na árvore um nó que representa uma solução completa e satisfatória.
 * @param arvore Ponteiro para a ArvoreDecisao.
 * @return Ponteiro para um NoArvoreDecisao que é uma solução, ou NULL se não houver.
 */
NoArvoreDecisao* buscar_solucao_na_arvore(ArvoreDecisao *arvore);
/**
 * @brief Função recursiva principal que constrói a árvore de decisão e busca por uma solução (parte do DPLL).
 * @param no_atual O nó atual na árvore a partir do qual a exploração continua.
 * @param formula A FormulaCNF sendo resolvida.
 * @param total_variaveis O número total de variáveis na fórmula.
 * @return true se uma solução for encontrada a partir deste ramo, false caso contrário.
 */
bool construir_arvore_recursivo(NoArvoreDecisao *no_atual, FormulaCNF *formula, int total_variaveis);

#endif // SAT_SOLVER_H