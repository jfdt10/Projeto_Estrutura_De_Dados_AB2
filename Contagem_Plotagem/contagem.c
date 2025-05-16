// Inclui bibliotecas padrão para entrada/saída, alocação de memória e manipulação de tempo.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define a estrutura para um nó da lista encadeada.
// Cada nó contém uma chave (valor inteiro) e um ponteiro para o próximo nó.
typedef struct NoLista {
    int chave;
    struct NoLista* proximo;
} NoLista;

// Insere um novo nó no início da lista encadeada.
// Retorna o ponteiro para a nova cabeça da lista.
NoLista* inserir_lista(NoLista* cabeca, int chave) {
    // Aloca memória para o novo nó.
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista));
    // Define a chave do novo nó.
    novo->chave = chave;
    // O próximo do novo nó aponta para a antiga cabeça da lista.
    novo->proximo = cabeca;
    // Retorna o novo nó como a nova cabeça.
    return novo;
}

// Busca uma chave na lista encadeada.
// Retorna o número de comparações realizadas até encontrar a chave ou percorrer toda a lista.
int buscar_lista(NoLista* cabeca, int chave) {
    int comparacoes = 0; // Inicializa o contador de comparações.
    NoLista* atual = cabeca; // Começa a busca a partir da cabeça.
    // Percorre a lista enquanto o nó atual não for nulo.
    while (atual) {
        comparacoes++; // Incrementa o contador a cada nó visitado.
        // Se a chave do nó atual for igual à chave procurada, retorna o número de comparações.
        if (atual->chave == chave)
            return comparacoes;
        // Avança para o próximo nó.
        atual = atual->proximo;
    }
    // Se a chave não for encontrada, retorna o total de comparações feitas.
    return comparacoes;
}

// Libera a memória alocada para todos os nós da lista encadeada.
void liberar_lista(NoLista* cabeca) {
    NoLista* tmp; // Ponteiro temporário para auxiliar na liberação.
    // Percorre a lista enquanto a cabeça não for nula.
    while (cabeca) {
        tmp = cabeca; // Guarda o nó atual.
        cabeca = cabeca->proximo; // Avança a cabeça para o próximo nó.
        free(tmp); // Libera a memória do nó guardado.
    }
}

// Define a estrutura para um nó da árvore binária de busca (BST).
// Cada nó contém uma chave, um ponteiro para o filho esquerdo e um ponteiro para o filho direito.
typedef struct NoArvore {
    int chave;
    struct NoArvore* esquerda;
    struct NoArvore* direita;
} NoArvore;

// Insere uma nova chave na árvore BST.
// Retorna o ponteiro para a raiz da árvore (modificada ou não).
NoArvore* inserir_arvore(NoArvore* no, int chave) {
    // Se o nó atual é nulo (ponto de inserção encontrado), cria um novo nó.
    if (no == NULL) {
        // Aloca memória para o novo nó.
        NoArvore* novo = (NoArvore*)malloc(sizeof(NoArvore));
        // Define a chave do novo nó.
        novo->chave = chave;
        // Os filhos esquerdo e direito do novo nó são nulos.
        novo->esquerda = novo->direita = NULL;
        return novo; // Retorna o novo nó.
    }
    // Se a chave a ser inserida é menor que a chave do nó atual, insere na subárvore esquerda.
    if (chave < no->chave)
        no->esquerda = inserir_arvore(no->esquerda, chave);
    // Se a chave a ser inserida é maior que a chave do nó atual, insere na subárvore direita.
    else if (chave > no->chave)
        no->direita = inserir_arvore(no->direita, chave);
    // Retorna o nó (raiz da subárvore), possivelmente com a nova inserção.
    return no;
}

// Busca uma chave na árvore BST.
// Retorna o número de comparações realizadas até encontrar a chave ou determinar que ela não existe.
int buscar_arvore(NoArvore* no, int chave) {
    int comparacoes = 0; // Inicializa o contador de comparações.
    // Percorre a árvore enquanto o nó atual não for nulo.
    while (no != NULL) {
        comparacoes++; // Incrementa o contador a cada nó visitado.
        // Se a chave procurada é igual à chave do nó atual, retorna o número de comparações.
        if (chave == no->chave)
            return comparacoes;
        // Se a chave procurada é menor, vai para a subárvore esquerda.
        else if (chave < no->chave)
            no = no->esquerda;
        // Se a chave procurada é maior, vai para a subárvore direita.
        else
            no = no->direita;
    }
    // Se a chave não for encontrada, retorna o total de comparações feitas.
    return comparacoes;
}

// Libera a memória alocada para todos os nós da árvore BST (percurso em pós-ordem).
void liberar_arvore(NoArvore* no) {
    if (no == NULL) return; // Condição de parada da recursão.
    liberar_arvore(no->esquerda); // Libera a subárvore esquerda.
    liberar_arvore(no->direita);  // Libera a subárvore direita.
    free(no); // Libera o nó atual.
}

// Função principal do programa.
int main() {
    // Abre (ou cria) o arquivo CSV para escrita dos resultados.
    FILE *fp = fopen("dados_busca.csv", "w"); 
    // Verifica se o arquivo foi aberto com sucesso.
    if (fp == NULL) {
        perror("Erro ao abrir arquivo dados_busca.csv"); // Mensagem de erro corrigida para o arquivo correto
        return 1; // Retorna 1 indicando erro.
    }
    // Escreve o cabeçalho no arquivo CSV.
    fprintf(fp, "NumeroProcurado,ComparacoesLista,ComparacoesBST\n");

    // Define o número máximo de elementos a serem gerados e inseridos.
    const int TAMANHO_MAXIMO = 10000;
    // Define o número de buscas a serem realizadas.
    const int NUMERO_DE_BUSCAS = 10000; 

    // Inicializa o gerador de números aleatórios com uma semente baseada no tempo atual.
    // Isso garante que os números gerados sejam diferentes a cada execução.
    srand((unsigned int)time(NULL)); 

    printf("Iniciando execucao unica...\n");

    // Aloca memória para um array que armazenará os valores únicos gerados.
    int *valores = (int *)malloc(TAMANHO_MAXIMO * sizeof(int));
    // Verifica se a alocação de memória foi bem-sucedida.
    if (valores == NULL) {
        perror("Falha ao alocar memoria para valores");
        fclose(fp); // Fecha o arquivo antes de sair.
        return 1; // Retorna 1 indicando erro.
    }

    printf("Gerando %d valores aleatorios unicos para insercao (metodo O(N^2))...\n", TAMANHO_MAXIMO);
    // Loop para gerar TAMANHO_MAXIMO números aleatórios únicos.
    for (int i = 0; i < TAMANHO_MAXIMO; i++) {
        int novo_valor;
        int duplicado;
        // Garante que o novo valor gerado seja único.
        do {
            novo_valor = rand(); // Gera um número aleatório.
            duplicado = 0; // Assume que não é duplicado inicialmente.
            // Verifica se o novo valor já existe no array de valores.
            for (int j = 0; j < i; j++) {
                if (valores[j] == novo_valor) {
                    duplicado = 1; // Marca como duplicado.
                    break; // Sai do loop interno.
                }
            }
        } while (duplicado); // Repete se o valor for duplicado.
        valores[i] = novo_valor; // Armazena o valor único gerado.
    }
    printf("Valores unicos para insercao gerados.\n");

    // Inicializa as cabeças da lista encadeada e da árvore BST como nulas.
    NoLista* lista = NULL;
    NoArvore* arvore = NULL;

    printf("Inserindo %d elementos em ambas as estruturas...\n", TAMANHO_MAXIMO);
    // Loop para inserir os valores gerados na lista encadeada e na árvore BST.
    for (int i = 0; i < TAMANHO_MAXIMO; i++) {
        lista = inserir_lista(lista, valores[i]);
        arvore = inserir_arvore(arvore, valores[i]);
    }
    printf("Insercao concluida.\n");

    printf("Realizando %d buscas por elementos existentes (com possivel repeticao na escolha)...\n", NUMERO_DE_BUSCAS);
    // Loop para realizar as buscas.
    for (int i = 0; i < NUMERO_DE_BUSCAS; i++) {
        // Escolhe aleatoriamente um índice de um valor já inserido.
        int idx_busca = rand() % TAMANHO_MAXIMO;
        // Obtém o valor a ser procurado.
        int chave_procurada = valores[idx_busca];

        // Realiza a busca na lista e na árvore.
        int compLista = buscar_lista(lista, chave_procurada);
        int compArvore = buscar_arvore(arvore, chave_procurada);

        // Escreve os resultados da busca no arquivo CSV.
        fprintf(fp, "%d,%d,%d\n", chave_procurada, compLista, compArvore);
    }
    printf("Buscas concluidas.\n");
 
    // Libera a memória alocada para a lista, a árvore e o array de valores.
    liberar_lista(lista);
    liberar_arvore(arvore);
    free(valores);

    // Fecha o arquivo CSV.
    fclose(fp);
    // Mensagem de confirmação atualizada para o nome correto do arquivo.
    printf("Dados salvos em dados_busca.csv\n");
    return 0;
}