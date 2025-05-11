# Define o compilador C a ser usado (gcc)
CC = gcc
# Define as flags do compilador:
# -Iinclude: Adiciona o diretório 'include' para busca de arquivos de cabeçalho (.h)
# -Wall: Ativa a maioria dos avisos do compilador
# -Wextra: Ativa avisos extras do compilador
# -g: Inclui informações de depuração no executável
CFLAGS = -Iinclude -Wall -Wextra -g

# Lista todos os arquivos fonte .c no diretório src
SRC = src/sat.c src/parser.c src/solver.c src/decision_tree.c
# Lista todos os arquivos objeto .o que serão gerados a partir dos arquivos .c
# A convenção é que cada arquivo .c é compilado em um arquivo .o com o mesmo nome base.
OBJ = sat.o parser.o solver.o decision_tree.o
# Define o nome do arquivo executável final
TARGET = sat_solver

# A regra 'all' é a regra padrão. Executar 'make' ou 'make all' construirá o $(TARGET).
# Ela depende do $(TARGET) estar construído.
all: $(TARGET)

# Regra para construir o executável final ($(TARGET)).
# Depende de todos os arquivos objeto ($(OBJ)) listados.
$(TARGET): $(OBJ)
	$(CC) -o $@ $^

# Regra de padrão para compilar arquivos .c em arquivos .o.
# %.o: Qualquer arquivo que termine com .o (ex: sat.o).
# src/%.c: O arquivo .c correspondente no diretório src (ex: src/sat.c).
%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra 'clean' para remover arquivos gerados pela compilação.
clean:
	# Comando para deletar os arquivos objeto e o executável.
	# O '-' no início faz com que o make ignore erros se os arquivos não existirem.
	# /Q é para o modo silencioso do comando 'del' no Windows.
	-del /Q $(OBJ) $(TARGET).exe

# Declara 'all' e 'clean' como alvos "phony".
# Isso significa que o make sempre executará os comandos para esses alvos,
# mesmo que existam arquivos com esses nomes, pois eles não representam arquivos reais a serem construídos.
.PHONY: all clean