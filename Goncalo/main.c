#include <stdio.h>
#include <string.h>

// Definir o tamanho máximo para strings de forma consistente
#define MAX_TEXTO_PERGUNTA 256
#define MAX_TIPO_PERGUNTA 20
#define MAX_NICKNAME 50
#define MAX_RESPOSTA 100 // Para respostas textuais simples

// Definição da struct para Pergunta
typedef struct {
    char texto[MAX_TEXTO_PERGUNTA];
    char tipo[MAX_TIPO_PERGUNTA]; // Ex: "Numerica", "Verdadeiro ou Falso"
    int dificuldade;              // 1: Fácil, 2: Médio, 3: Difícil
    char resposta_correta[MAX_RESPOSTA]; // Resposta correta em formato de string
} Pergunta;

// Definição da struct para Jogador
typedef struct {
    char nickname[MAX_NICKNAME];
    int pontuacao;
} Jogador;

// Função principal
int main() {
    // Exemplo de criação e inicialização de uma Pergunta
    Pergunta pergunta1;
    strcpy(pergunta1.texto, "Qual é o capital de Portugal?");
    strcpy(pergunta1.tipo, "Texto"); // Simplificado para texto direto
    pergunta1.dificuldade = 1; // Fácil
    strcpy(pergunta1.resposta_correta, "Lisboa");

    // Exemplo de criação e inicialização de um Jogador
    Jogador jogador1;
    strcpy(jogador1.nickname, "AventureiroC");
    jogador1.pontuacao = 0;

    printf("--- Início do Quiz Simplificado ---\n\n");

    // Identificação do jogador
    printf("Bem-vindo, %s!\n", jogador1.nickname);

    // Gestão e Execução de Perguntas (simplificado)
    printf("\nPergunta de Dificuldade %d:\n", pergunta1.dificuldade);
    printf("%s\n", pergunta1.texto);

    char resposta_usuario[MAX_RESPOSTA];
    printf("Sua resposta: ");
    fgets(resposta_usuario, MAX_RESPOSTA, stdin);
    // Remover o caractere de nova linha que fgets pode capturar
    resposta_usuario[strcspn(resposta_usuario, "\n")] = '\0';

    // Verificação da resposta e atribuição de pontos (simplificado)
    if (strcmp(resposta_usuario, pergunta1.resposta_correta) == 0) {
        printf("Correto! Ganhou 10 pontos.\n");
        jogador1.pontuacao += 10; // Exemplo de atribuição de pontos
    } else {
        printf("Incorreto. A resposta correta era: %s\n", pergunta1.resposta_correta);
    }

    printf("\nPontuação atual de %s: %d\n", jogador1.nickname, jogador1.pontuacao);

    printf("\n--- Fim do Quiz Simplificado ---\n");

    return 0;
}