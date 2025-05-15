#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STACK_SIZE 100
#define MAX_TEXT_LENGTH 1000

typedef struct {
    char text[MAX_TEXT_LENGTH];
} TextState;

typedef struct {
    TextState states[MAX_STACK_SIZE];
    int top;
} Stack;

void initStack(Stack *s) {
    s->top = -1;
}

bool isStackEmpty(Stack *s) {
    return s->top == -1;
}

bool isStackFull(Stack *s) {
    return s->top == MAX_STACK_SIZE - 1;
}

void push(Stack *s, TextState state) {
    if (isStackFull(s)) {
        // Remove o estado mais antigo (fundo da pilha)
        for (int i = 0; i < s->top; i++) {
            s->states[i] = s->states[i+1];
        }
        s->top--;
    }
    s->top++;
    s->states[s->top] = state;
}

TextState pop(Stack *s) {
    if (isStackEmpty(s)) {
        TextState empty = {""};
        return empty;
    }
    return s->states[s->top--];
}

void saveState(Stack *undoStack, Stack *redoStack, const char *text) {
    // Quando salvamos um novo estado, limpamos a pilha redo
    initStack(redoStack);
    
    TextState newState;
    strncpy(newState.text, text, MAX_TEXT_LENGTH);
    push(undoStack, newState);
}

void undo(Stack *undoStack, Stack *redoStack, char *text) {
    if (isStackEmpty(undoStack)) return;
    
    // Salva o estado atual no redo antes de desfazer
    TextState currentState;
    strncpy(currentState.text, text, MAX_TEXT_LENGTH);
    push(redoStack, currentState);
    
    // Restaura o estado anterior
    TextState prevState = pop(undoStack);
    strncpy(text, prevState.text, MAX_TEXT_LENGTH);
}

void redo(Stack *undoStack, Stack *redoStack, char *text) {
    if (isStackEmpty(redoStack)) return;
    
    // Salva o estado atual no undo antes de refazer
    TextState currentState;
    strncpy(currentState.text, text, MAX_TEXT_LENGTH);
    push(undoStack, currentState);
    
    // Restaura o estado que foi desfeito
    TextState nextState = pop(redoStack);
    strncpy(text, nextState.text, MAX_TEXT_LENGTH);
}

int main() {
    Stack undoStack, redoStack;
    initStack(&undoStack);
    initStack(&redoStack);
    
    char text[MAX_TEXT_LENGTH] = "";
    
    int choice;
    char input[MAX_TEXT_LENGTH];
    
    while (1) {
        printf("\nTexto atual: %s\n", text);
        printf("1. Adicionar texto\n");
        printf("2. Undo\n");
        printf("3. Redo\n");
        printf("4. Sair\n");
        printf("Escolha: ");
        scanf("%d", &choice);
        getchar(); // Limpa o buffer
        
        switch (choice) {
            case 1:
                printf("Digite o texto a ser adicionado: ");
                fgets(input, MAX_TEXT_LENGTH, stdin);
                input[strcspn(input, "\n")] = '\0'; // Remove a quebra de linha
                
                // Salva o estado atual antes de modificar
                saveState(&undoStack, &redoStack, text);
                
                // Concatena o novo texto
                strncat(text, input, MAX_TEXT_LENGTH - strlen(text) - 1);
                break;
                
            case 2:
                undo(&undoStack, &redoStack, text);
                break;
                
            case 3:
                redo(&undoStack, &redoStack, text);
                break;
                
            case 4:
                exit(0);
                
            default:
                printf("Opção inválida!\n");
        }
    }
    
    return 0;
}
