#include <stdio.h>   // Para printf, scanf, fgets, fopen, fclose, fwrite, fread
#include <string.h>  // Para strcspn, strcmp, strcpy, strlen, strtok, sscanf

// --- Constantes Globais ---
#define MAX_PERGUNTAS 50
#define MAX_JOGADORES_RANKING 10
#define MAX_TEXTO_PERGUNTA 256
#define MAX_OPCOES 4
#define MAX_TEXTO_OPCAO 100
#define MAX_TEXTO_RESPOSTA 100 // Para respostas numéricas ou V/F como texto, ou escolha múltipla
#define MAX_NOME_JOGADOR 50

#define NOME_FICHEIRO_PERGUNTAS_BINARIO "perguntas.bin"
#define NOME_FICHEIRO_RANKING_BINARIO "ranking.bin"
#define NOME_FICHEIRO_PERGUNTAS_TXT "perguntas.txt" // Para carregar do txt

// --- Enumeradores para facilitar a leitura e gestão ---
typedef enum tipo_pergunta {
    ESCOLHA_MULTIPLA,
    RESPOSTA_NUMERICA,
    VERDADEIRO_FALSO
} TipoPergunta;

typedef enum dificuldade {
    FACIL,
    MEDIO,
    DIFICIL
} Dificuldade;

// --- Estruturas ---
// Estrutura para representar uma pergunta
typedef struct pergunta {
    char texto[MAX_TEXTO_PERGUNTA];
    TipoPergunta tipo;
    Dificuldade dificuldade;
    char opcoes[MAX_OPCOES][MAX_TEXTO_OPCAO]; // Usado apenas para escolha múltipla
    char resposta_correta_texto[MAX_TEXTO_RESPOSTA]; // Resposta para todos os tipos (será convertida conforme o tipo)
} Pergunta;

// Estrutura para representar um jogador no ranking
typedef struct jogador {
    char nome[MAX_NOME_JOGADOR];
    int pontuacao;
} Jogador;

// --- Variáveis Globais (para simplificar a passagem entre funções em código académico) ---
Pergunta perguntas[MAX_PERGUNTAS];
int contador_perguntas = 0; // Número de perguntas atualmente carregadas/adicionadas

Jogador ranking[MAX_JOGADORES_RANKING];
int num_jogadores_ranking = 0; // Número de jogadores no ranking

// --- Protótipos de Funções ---
void limparBufferEntrada();
int menu_principal();
void identificar_jogador(Jogador *jogador_atual);
void gerir_perguntas();
void adicionar_pergunta();
void carregar_perguntas_txt(const char *nome_ficheiro);
void salvar_perguntas_binario(const char *nome_ficheiro);
int carregar_perguntas_binario(const char *nome_ficheiro); // Retorna o número de perguntas carregadas
void executar_quiz(Jogador *jogador_atual);
void mostrar_ranking();
void atualizar_ranking(Jogador novo_jogador);
void salvar_ranking_binario(const char *nome_ficheiro);
int carregar_ranking_binario(const char *nome_ficheiro); // Retorna o número de jogadores carregados

// Função auxiliar para calcular o valor absoluto de um float (evitando math.h)
float abs_float(float f) {
    return (f < 0) ? -f : f;
}

// Implementação da função para limpar o buffer de entrada
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Implementação do menu principal
int menu_principal() {
    int escolha;
    printf("\n--- Menu Principal do Quiz ---\n");
    printf("1. Iniciar Novo Jogo\n");
    printf("2. Gerir Perguntas\n");
    printf("3. Ver Ranking\n");
    printf("4. Sair\n");
    printf("Escolha uma opcao: ");
    // Validação de input básica
    if (scanf("%d", &escolha) != 1) {
        printf("Input invalido. Por favor, digite um numero.\n");
        limparBufferEntrada(); // Limpa o buffer após input inválido
        return 0; // Retorna 0 ou outro valor para indicar erro e repetir o menu
    }
    limparBufferEntrada();
    return escolha;
}

// Implementação da identificação do jogador
void identificar_jogador(Jogador *jogador_atual) {
    printf("\n--- Identificacao do Jogador ---\n");
    printf("Digite o seu nome/nickname: ");
    fgets(jogador_atual->nome, MAX_NOME_JOGADOR, stdin);
    jogador_atual->nome[strcspn(jogador_atual->nome, "\n")] = '\0'; // Remove o \n no final
    jogador_atual->pontuacao = 0; // Inicia com pontuação zero
    printf("Bem-vindo(a), %s! A sua pontuacao atual e: %d.\n", jogador_atual->nome, jogador_atual->pontuacao);
}

// Implementação do menu de gestão de perguntas
void gerir_perguntas() {
    int escolha;
    do {
        printf("\n--- Gestao de Perguntas ---\n");
        printf("1. Adicionar Nova Pergunta\n");
        printf("2. Carregar Perguntas de Ficheiro TXT\n");
        printf("3. Salvar Perguntas em Ficheiro Binario\n");
        printf("4. Voltar ao Menu Principal\n");
        printf("Escolha uma opcao: ");
        // Validação de input
        if (scanf("%d", &escolha) != 1) {
            printf("Input invalido. Por favor, digite um numero.\n");
            limparBufferEntrada();
            continue; // Continua o loop para pedir novamente
        }
        limparBufferEntrada();

        switch (escolha) {
            case 1:
                adicionar_pergunta();
                break;
            case 2:
                carregar_perguntas_txt(NOME_FICHEIRO_PERGUNTAS_TXT);
                break;
            case 3:
                salvar_perguntas_binario(NOME_FICHEIRO_PERGUNTAS_BINARIO);
                break;
            case 4:
                printf("A voltar ao Menu Principal...\n");
                break;
            default:
                printf("Opcao invalida. Por favor, tente novamente.\n");
        }
    } while (escolha != 4);
}

// Implementação da adição de uma nova pergunta
void adicionar_pergunta() {
    if (contador_perguntas >= MAX_PERGUNTAS) {
        printf("Limite de perguntas atingido (%d). Nao e possivel adicionar mais.\n", MAX_PERGUNTAS);
        return;
    }

    Pergunta *nova_pergunta = &perguntas[contador_perguntas];
    int tipo_escolha, dificuldade_escolha;

    printf("\n--- Adicionar Nova Pergunta ---\n");

    printf("Texto da pergunta: ");
    fgets(nova_pergunta->texto, MAX_TEXTO_PERGUNTA, stdin);
    nova_pergunta->texto[strcspn(nova_pergunta->texto, "\n")] = '\0';

    printf("Tipo de Pergunta (0-Escolha Multipla, 1-Numerica, 2-Verdadeiro/Falso): ");
    if (scanf("%d", &tipo_escolha) != 1 || tipo_escolha < 0 || tipo_escolha > 2) {
        printf("Tipo de pergunta invalido. A definir como Escolha Multipla.\n");
        tipo_escolha = 0;
    }
    limparBufferEntrada();
    nova_pergunta->tipo = (TipoPergunta)tipo_escolha;

    // Input das opções/resposta baseado no tipo de pergunta
    if (nova_pergunta->tipo == ESCOLHA_MULTIPLA) {
        printf("Opcoes para a escolha multipla (uma por linha, %d opcoes):\n", MAX_OPCOES);
        for (int i = 0; i < MAX_OPCOES; i++) {
            printf("Opcao %d: ", i + 1);
            fgets(nova_pergunta->opcoes[i], MAX_TEXTO_OPCAO, stdin);
            nova_pergunta->opcoes[i][strcspn(nova_pergunta->opcoes[i], "\n")] = '\0';
        }
        printf("Indique o TEXTO da opcao correta (exatamente como introduzida): ");
        fgets(nova_pergunta->resposta_correta_texto, MAX_TEXTO_RESPOSTA, stdin);
        nova_pergunta->resposta_correta_texto[strcspn(nova_pergunta->resposta_correta_texto, "\n")] = '\0';

    } else if (nova_pergunta->tipo == RESPOSTA_NUMERICA) {
        printf("Resposta numerica correta (ex: 12.34): ");
        fgets(nova_pergunta->resposta_correta_texto, MAX_TEXTO_RESPOSTA, stdin);
        nova_pergunta->resposta_correta_texto[strcspn(nova_pergunta->resposta_correta_texto, "\n")] = '\0';
    } else if (nova_pergunta->tipo == VERDADEIRO_FALSO) {
        printf("Resposta correta (Verdadeiro/Falso - escreva 'V' ou 'F'): ");
        fgets(nova_pergunta->resposta_correta_texto, MAX_TEXTO_RESPOSTA, stdin);
        nova_pergunta->resposta_correta_texto[strcspn(nova_pergunta->resposta_correta_texto, "\n")] = '\0';
    }

    printf("Dificuldade (0-Facil, 1-Medio, 2-Dificil): ");
    if (scanf("%d", &dificuldade_escolha) != 1 || dificuldade_escolha < 0 || dificuldade_escolha > 2) {
        printf("Dificuldade invalida. A definir como Facil.\n");
        dificuldade_escolha = 0;
    }
    limparBufferEntrada();
    nova_pergunta->dificuldade = (Dificuldade)dificuldade_escolha;

    contador_perguntas++;
    printf("Pergunta adicionada com sucesso! (%d perguntas no total).\n", contador_perguntas);
}

// Implementação do carregamento de perguntas a partir de um ficheiro TXT
// Formato esperado por linha no TXT:
// Para Escolha Multipla: TextoPergunta|Tipo(int)|Dificuldade(int)|RespostaCorretaTexto|Opcao1|Opcao2|Opcao3|Opcao4
// Para Numerica/Verdadeiro/Falso: TextoPergunta|Tipo(int)|Dificuldade(int)|RespostaCorretaTexto
void carregar_perguntas_txt(const char *nome_ficheiro) {
    FILE *ficheiro = fopen(nome_ficheiro, "r");
    if (ficheiro == NULL) {
        printf("Erro: Nao foi possivel abrir o ficheiro TXT '%s'.\n", nome_ficheiro);
        return;
    }

    contador_perguntas = 0; // Reset das perguntas existentes para simplicidade académica
    char linha[MAX_TEXTO_PERGUNTA + (MAX_OPCOES * MAX_TEXTO_OPCAO) + MAX_TEXTO_RESPOSTA + 50]; // Buffer para a linha

    printf("\n--- Carregar Perguntas de Ficheiro TXT ---\n");

    while (fgets(linha, sizeof(linha), ficheiro) != NULL && contador_perguntas < MAX_PERGUNTAS) {
        Pergunta *p = &perguntas[contador_perguntas];
        char *token;
        char *rest = linha;

        // Texto da Pergunta
        token = strtok(rest, "|");
        if (token) strcpy(p->texto, token); else continue;

        // Tipo da Pergunta
        token = strtok(NULL, "|");
        if (token) p->tipo = (TipoPergunta)atoi(token); else continue; // atoi é de stdlib.h. Se não permitido, sscanf
                                                                       // sscanf é uma alternativa: sscanf(token, "%d", &p->tipo);

        // Dificuldade da Pergunta
        token = strtok(NULL, "|");
        if (token) p->dificuldade = (Dificuldade)atoi(token); else continue; // sscanf(token, "%d", &p->dificuldade);

        // Resposta Correta (texto)
        token = strtok(NULL, "|");
        if (token) {
            token[strcspn(token, "\n")] = '\0'; // Remove '\n' do final se presente
            strcpy(p->resposta_correta_texto, token);
        } else continue;

        // Se for Escolha Múltipla, ler as opções
        if (p->tipo == ESCOLHA_MULTIPLA) {
            for (int i = 0; i < MAX_OPCOES; i++) {
                token = strtok(NULL, "|");
                if (token) {
                    token[strcspn(token, "\n")] = '\0'; // Remove '\n' do final se presente
                    strcpy(p->opcoes[i], token);
                } else {
                    strcpy(p->opcoes[i], ""); // Preenche com vazio se não houver opção
                }
            }
        }
        
        contador_perguntas++;
    }
    fclose(ficheiro);
    printf("Perguntas carregadas do ficheiro TXT '%s'. Total: %d.\n", nome_ficheiro, contador_perguntas);
}

// Implementação do salvamento de perguntas num ficheiro binário
void salvar_perguntas_binario(const char *nome_ficheiro) {
    FILE *ficheiro = fopen(nome_ficheiro, "wb");
    // Em código académico básico, vamos omitir a verificação de NULL para fopen na escrita,
    // conforme o requisito de "evitar verificações desnecessárias".
    // Na prática, esta verificação seria essencial.

    fwrite(perguntas, sizeof(Pergunta), contador_perguntas, ficheiro);
    fclose(ficheiro);
    printf("Perguntas salvas em ficheiro binario '%s'. Total: %d.\n", nome_ficheiro, contador_perguntas);
}

// Implementação do carregamento de perguntas a partir de um ficheiro binário
int carregar_perguntas_binario(const char *nome_ficheiro) {
    FILE *ficheiro = fopen(nome_ficheiro, "rb");
    if (ficheiro == NULL) {
        printf("Ficheiro binario de perguntas '%s' nao encontrado ou inacessivel. Iniciando com 0 perguntas.\n", nome_ficheiro);
        return 0; // Nenhuma pergunta carregada
    }

    contador_perguntas = fread(perguntas, sizeof(Pergunta), MAX_PERGUNTAS, ficheiro);
    fclose(ficheiro);
    printf("Perguntas carregadas do ficheiro binario '%s'. Total: %d.\n", nome_ficheiro, contador_perguntas);
    return contador_perguntas;
}

// Implementação da execução do Quiz
void executar_quiz(Jogador *jogador_atual) {
    printf("\n--- A Iniciar o Quiz ---\n");
    if (contador_perguntas == 0) {
        printf("Nao existem perguntas para jogar. Por favor, adicione ou carregue perguntas.\n");
        return;
    }

    // Para um quiz simples, vamos percorrer as perguntas disponíveis
    for (int i = 0; i < contador_perguntas; i++) {
        Pergunta p = perguntas[i];
        char resposta_jogador[MAX_TEXTO_RESPOSTA];
        int pontuacao_pergunta = 0; // Pontos base para a pergunta

        // Pontuação baseada na dificuldade
        if (p.dificuldade == FACIL) pontuacao_pergunta = 10;
        else if (p.dificuldade == MEDIO) pontuacao_pergunta = 20;
        else if (p.dificuldade == DIFICIL) pontuacao_pergunta = 30;

        printf("\nPergunta %d (Dificuldade: ", i + 1);
        if (p.dificuldade == FACIL) printf("Facil");
        else if (p.dificuldade == MEDIO) printf("Medio");
        else printf("Dificil");
        printf("):\n");
        printf("%s\n", p.texto);

        if (p.tipo == ESCOLHA_MULTIPLA) {
            for (int j = 0; j < MAX_OPCOES; j++) {
                if (strlen(p.opcoes[j]) > 0) {
                    printf("%d. %s\n", j + 1, p.opcoes[j]);
                }
            }
            printf("A sua resposta (digite o TEXTO da opcao): ");
            fgets(resposta_jogador, MAX_TEXTO_RESPOSTA, stdin);
            resposta_jogador[strcspn(resposta_jogador, "\n")] = '\0';

            // Comparação case-sensitive
            if (strcmp(resposta_jogador, p.resposta_correta_texto) == 0) {
                printf("Correto! Ganhou %d pontos.\n", pontuacao_pergunta);
                jogador_atual->pontuacao += pontuacao_pergunta;
            } else {
                printf("Incorreto. A resposta correta era: '%s'\n", p.resposta_correta_texto);
            }
        } else if (p.tipo == RESPOSTA_NUMERICA) {
            float resposta_num_jogador;
            float resposta_num_correta;

            printf("A sua resposta (numero): ");
            fgets(resposta_jogador, MAX_TEXTO_RESPOSTA, stdin);
            resposta_jogador[strcspn(resposta_jogador, "\n")] = '\0';
            
            // Conversão de string para float usando sscanf
            sscanf(resposta_jogador, "%f", &resposta_num_jogador);
            sscanf(p.resposta_correta_texto, "%f", &resposta_num_correta);

            // Comparar floats com uma pequena tolerância devido a imprecisões de ponto flutuante
            if (abs_float(resposta_num_jogador - resposta_num_correta) < 0.001) {
                printf("Correto! Ganhou %d pontos.\n", pontuacao_pergunta);
                jogador_atual->pontuacao += pontuacao_pergunta;
            } else {
                printf("Incorreto. A resposta correta era: %.2f\n", resposta_num_correta);
            }
        } else if (p.tipo == VERDADEIRO_FALSO) {
            printf("A sua resposta (V/F): ");
            fgets(resposta_jogador, MAX_TEXTO_RESPOSTA, stdin);
            resposta_jogador[strcspn(resposta_jogador, "\n")] = '\0';

            // Comparação V/F (case-sensitive para simplicidade)
            if (strcmp(resposta_jogador, p.resposta_correta_texto) == 0) {
                 printf("Correto! Ganhou %d pontos.\n", pontuacao_pergunta);
                jogador_atual->pontuacao += pontuacao_pergunta;
            } else {
                printf("Incorreto. A resposta correta era: '%s'\n", p.resposta_correta_texto);
            }
        }
    }
    printf("\n--- Quiz Terminado ---\n");
    printf("%s, a sua pontuacao final e: %d.\n", jogador_atual->nome, jogador_atual->pontuacao);

    atualizar_ranking(*jogador_atual); // Adicionar ou atualizar jogador no ranking
}

// Implementação da exibição do ranking
void mostrar_ranking() {
    printf("\n--- Ranking dos Melhores Jogadores ---\n");

    if (num_jogadores_ranking == 0) {
        printf("Nao ha jogadores no ranking ainda.\n");
        return;
    }

    // Ordenar o ranking por pontuação decrescente (usando Bubble Sort para manter a simplicidade académica)
    //
    for (int i = 0; i < num_jogadores_ranking - 1; i++) {
        for (int j = 0; j < num_jogadores_ranking - i - 1; j++) {
            if (ranking[j].pontuacao < ranking[j+1].pontuacao) {
                // Trocar jogadores
                Jogador temp = ranking[j];
                ranking[j] = ranking[j+1];
                ranking[j+1] = temp;
            }
        }
    }

    // Imprimir o ranking
    for (int i = 0; i < num_jogadores_ranking; i++) {
        printf("%d. %s - %d pontos\n", i + 1, ranking[i].nome, ranking[i].pontuacao);
    }
}

// Implementação da atualização do ranking
void atualizar_ranking(Jogador novo_jogador) {
    int i;
    // Tentar encontrar o jogador no ranking
    for (i = 0; i < num_jogadores_ranking; i++) {
        //
        if (strcmp(ranking[i].nome, novo_jogador.nome) == 0) {
            // Se o jogador já existe, atualizar a pontuação se for maior
            if (novo_jogador.pontuacao > ranking[i].pontuacao) {
                ranking[i].pontuacao = novo_jogador.pontuacao;
                printf("Pontuacao de %s atualizada no ranking.\n", novo_jogador.nome);
            } else {
                printf("%s ja esta no ranking com uma pontuacao igual ou superior.\n", novo_jogador.nome);
            }
            // Chama mostrar_ranking para reordenar e exibir após atualização
            mostrar_ranking();
            return;
        }
    }

    // Se o jogador não foi encontrado
    if (num_jogadores_ranking < MAX_JOGADORES_RANKING) {
        ranking[num_jogadores_ranking] = novo_jogador;
        num_jogadores_ranking++;
        printf("%s adicionado ao ranking.\n", novo_jogador.nome);
    } else {
        // Ranking cheio, verificar se a nova pontuação é suficiente para entrar no top 10
        // Para isso, garantimos que o ranking está ordenado antes de verificar a menor pontuação.
        // A função mostrar_ranking() já faz a ordenação.
        mostrar_ranking(); // Para garantir que está ordenado e ver a menor pontuação (último elemento)

        if (novo_jogador.pontuacao > ranking[MAX_JOGADORES_RANKING - 1].pontuacao) {
            ranking[MAX_JOGADORES_RANKING - 1] = novo_jogador; // Substituir o último (o de menor pontuação)
            printf("%s entrou para o ranking!\n", novo_jogador.nome);
        } else {
            printf("A pontuacao de %s (%d) nao foi suficiente para entrar no top %d do ranking.\n", novo_jogador.nome, novo_jogador.pontuacao, MAX_JOGADORES_RANKING);
        }
    }
    // Após qualquer alteração (adição ou substituição), re-exibir o ranking ordenado
    mostrar_ranking();
}

// Implementação do salvamento do ranking num ficheiro binário
void salvar_ranking_binario(const char *nome_ficheiro) {
    FILE *ficheiro = fopen(nome_ficheiro, "wb");
    // Em código académico básico, vamos omitir a verificação de NULL para fopen na escrita.
    fwrite(ranking, sizeof(Jogador), num_jogadores_ranking, ficheiro);
    fclose(ficheiro);
    printf("Ranking salvo em ficheiro binario '%s'.\n", nome_ficheiro);
}

// Implementação do carregamento do ranking a partir de um ficheiro binário
int carregar_ranking_binario(const char *nome_ficheiro) {
    FILE *ficheiro = fopen(nome_ficheiro, "rb");
    if (ficheiro == NULL) {
        printf("Ficheiro binario de ranking '%s' nao encontrado ou inacessivel. Ranking vazio.\n", nome_ficheiro);
        return 0; // Nenhum jogador carregado
    }

    num_jogadores_ranking = fread(ranking, sizeof(Jogador), MAX_JOGADORES_RANKING, ficheiro);
    fclose(ficheiro);
    printf("Ranking carregado do ficheiro binario '%s'. Total: %d jogadores.\n", nome_ficheiro, num_jogadores_ranking);
    return num_jogadores_ranking;
}

// --- Função Principal ---
int main(void) {
    // Carregar dados existentes ao iniciar a aplicação
    carregar_perguntas_binario(NOME_FICHEIRO_PERGUNTAS_BINARIO);
    carregar_ranking_binario(NOME_FICHEIRO_RANKING_BINARIO);

    int escolha;
    Jogador jogador_atual; // Variável para o jogador que está a jogar atualmente

    do {
        escolha = menu_principal(); // Exibe o menu e obtém a escolha do utilizador

        switch (escolha) {
            case 1:
                identificar_jogador(&jogador_atual);
                executar_quiz(&jogador_atual); // Passa o jogador atual para o quiz
                break;
            case 2:
                gerir_perguntas();
                break;
            case 3:
                mostrar_ranking();
                break;
            case 4:
                printf("A sair da aplicacao. Adeus!\n");
                break;
            default:
                printf("Opcao invalida. Por favor, selecione uma opcao entre 1 e 4.\n");
                break;
        }
        // Para manter a tela visível antes de limpar (se usasse system("cls") ou similar)
        //printf("\nPressione Enter para continuar...");
        //limparBufferEntrada(); // Garante que o Enter é consumido

    } while (escolha != 4);

    // Salvar dados ao sair da aplicação
    salvar_perguntas_binario(NOME_FICHEIRO_PERGUNTAS_BINARIO);
    salvar_ranking_binario(NOME_FICHEIRO_RANKING_BINARIO);

    return 0; // Indica que o programa terminou com sucesso
}