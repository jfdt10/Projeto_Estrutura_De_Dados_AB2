# SAT Solver

Este projeto implementa um resolvedor SAT (Problema de Satisfatibilidade Booleana) em C que lê fórmulas no formato DIMACS (.cnf) e determina se são satisfatíveis (SAT) ou insatisfatíveis (UNSAT). Se a fórmula for satisfatível, o resolvedor também exibe as atribuições das variáveis.

## Estrutura do Projeto

```
sat-solver/
├── include/
│   └── sat_solver.h    # Interface pública, definições de tipo e declarações de funções centrais
├── src/
│   ├── sat.c           # Ponto de entrada principal da aplicação do resolvedor SAT
│   ├── parser.c        # Funções para analisar a entrada no formato DIMACS
│   ├── solver.c        # Implementação do algoritmo de resolução SAT (DPLL)
│   ├── solver.h        # Arquivo de cabeçalho para funções específicas do resolvedor
│   └── decision_tree.c # Implementação da lógica da árvore de decisão para o resolvedor
├── test/
│   └── test_cases/     # Diretório contendo arquivos .cnf para teste
│       ├── sat1.cnf    # Exemplo de fórmula satisfatível
│       ├── unsat1.cnf  # Exemplo de fórmula insatisfatível
│       └── ...         # Outros casos de teste
├── Makefile            # Instruções de compilação para o projeto
└── README.md           # Documentação do projeto
```

## Compilando o Projeto

### Usando Make (Recomendado)

Para compilar o resolvedor SAT, navegue até o diretório do projeto e execute um dos seguintes comandos:

```sh
make
```
Ou
```sh
make all
```

Isso compilará os arquivos-fonte e criará o executável `sat_solver.exe` (no Windows) ou `sat_solver` (no Linux/macOS).

### Compilação Manual (sem Make)

Se você não tem o `make` instalado ou prefere compilar manualmente, pode usar o `gcc`. Certifique-se de que o `gcc` está instalado e no PATH do seu sistema. Execute os seguintes comandos a partir do diretório raiz do projeto:

**1. Compile os arquivos-fonte para arquivos objeto:**
```sh
gcc -c src/parser.c -o parser.o -Iinclude -Wall -Wextra -g
gcc -c src/decision_tree.c -o decision_tree.o -Iinclude -Wall -Wextra -g
gcc -c src/solver.c -o solver.o -Iinclude -Wall -Wextra -g
gcc -c src/sat.c -o sat.o -Iinclude -Wall -Wextra -g
```

**2. Linke os arquivos objeto para criar o executável:**

Para Windows:
```sh
gcc parser.o decision_tree.o solver.o sat.o -o sat_solver.exe
```

Para Linux/macOS:
```sh
gcc parser.o decision_tree.o solver.o sat.o -o sat_solver
```

## Executando o Resolvedor

Para executar o resolvedor SAT, use o seguinte comando a partir do diretório raiz do projeto:

No Windows:
```sh
.\\sat_solver.exe test/test_cases/<nome_do_arquivo.cnf>
```

No Linux/macOS:
```sh
./sat_solver test/test_cases/<nome_do_arquivo.cnf>
```

Substitua `<nome_do_arquivo.cnf>` pelo caminho para o seu arquivo DIMACS (.cnf) dentro do diretório `test/test_cases/`, ou forneça o caminho completo para qualquer outro arquivo `.cnf`.

## Exemplo

Dado um arquivo DIMACS `test/test_cases/sat1.cnf`, você pode executar o resolvedor da seguinte forma (no Windows):

```sh
.\\sat_solver.exe test/test_cases/sat1.cnf
```

### Saída

A saída indicará se a fórmula é SAT ou UNSAT. Se for SAT, também exibirá as atribuições das variáveis.
Por exemplo:
```
SAT!
1 = 1 2 = 0 3 = 1 ...
```
Ou:
```
UNSAT!
```