// war_estruturado.c
// Desafio WAR Estruturado - versão integrada (Novato + Aventureiro + Mestre)
// Código escrito no estilo de aluno: simples, direto e com comentários curtos.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TERRITORIOS 5
#define NOME_SIZE 30
#define COR_SIZE 15

// struct que guarda os dados do território
typedef struct {
    char nome[NOME_SIZE];
    char cor[COR_SIZE];
    int tropas;
} Territorio;

// função pequena para ler linha do usuário (fgets + remover '\n')
void ler_linha(char *buffer, int tamanho) {
    if (fgets(buffer, tamanho, stdin)) {
        size_t ln = strlen(buffer);
        if (ln > 0 && buffer[ln - 1] == '\n') buffer[ln - 1] = '\0';
    }
}

// impressão do mapa (array de territorios e tamanho)
void mostrar_mapa(const Territorio *mapa, int n) {
    printf("\n--- MAPA ATUAL (%d territorios) ---\n", n);
    for (int i = 0; i < n; i++) {
        printf("Territorio %d:\n", i + 1);
        printf("  Nome: %s\n", mapa[i].nome);
        printf("  Cor do exército: %s\n", mapa[i].cor);
        printf("  Tropas: %d\n", mapa[i].tropas);
        printf("-----------------------------\n");
    }
}

// Cadastro simples (Novato) em vetor estático passado por referencia
void cadastro_static(Territorio mapa[MAX_TERRITORIOS]) {
    printf("\n== Cadastro (Novato) - Preencha %d territorios ==\n", MAX_TERRITORIOS);
    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        char temp[100];
        printf("Territorio %d - digite o nome: ", i + 1);
        ler_linha(temp, sizeof(temp));
        strncpy(mapa[i].nome, temp, NOME_SIZE);
        mapa[i].nome[NOME_SIZE - 1] = '\0';

        printf("Territorio %d - digite a cor do exército: ", i + 1);
        ler_linha(temp, sizeof(temp));
        strncpy(mapa[i].cor, temp, COR_SIZE);
        mapa[i].cor[COR_SIZE - 1] = '\0';

        printf("Territorio %d - digite a quantidade de tropas: ", i + 1);
        // ler inteiro com verificação simples
        while (scanf("%d", &mapa[i].tropas) != 1) {
            printf("Entrada invalida. Digite um numero inteiro para tropas: ");
            while (getchar() != '\n'); // limpa buffer
        }
        while (getchar() != '\n'); // limpa o \n restante
        printf("\n");
    }
    printf("Cadastro concluido (Novato).\n");
}

// converte dados do static para dynamic (calloc) - retorna ponteiro alocado
Territorio* converte_para_dynamic(const Territorio mapa_static[MAX_TERRITORIOS], int n) {
    Territorio *mapa_dyn = (Territorio*) calloc(n, sizeof(Territorio));
    if (!mapa_dyn) {
        printf("Erro: memoria nao alocada.\n");
        return NULL;
    }
    for (int i = 0; i < n; i++) {
        mapa_dyn[i] = mapa_static[i]; // copia struct inteira
    }
    printf("Mapa convertido para versao Aventureiro (dynamic, calloc).\n");
    return mapa_dyn;
}

// gera um valor de dado (1 a 6)
int rolar_dado() {
    return (rand() % 6) + 1;
}

// função de ataque entre dois territorios (usa ponteiro p/ mapa dinâmico ou estático)
// atacador e defensor são indices (0-based), n é numero de territorios
void atacar(Territorio *mapa, int n, int atacante_idx, int defensor_idx) {
    if (atacante_idx < 0 || atacante_idx >= n || defensor_idx < 0 || defensor_idx >= n) {
        printf("Indice invalido.\n");
        return;
    }
    if (atacante_idx == defensor_idx) {
        printf("Nao pode atacar o mesmo territorio.\n");
        return;
    }
    if (mapa[atacante_idx].tropas <= 1) {
        printf("Territorio atacante precisa ter mais de 1 tropa para atacar.\n");
        return;
    }
    if (mapa[defensor_idx].tropas <= 0) {
        printf("Territorio defensor ja nao tem tropas.\n");
        return;
    }

    printf("\nAtaque: %s (%s, %d tropas) -> %s (%s, %d tropas)\n",
           mapa[atacante_idx].nome, mapa[atacante_idx].cor, mapa[atacante_idx].tropas,
           mapa[defensor_idx].nome, mapa[defensor_idx].cor, mapa[defensor_idx].tropas);

    // simulação simples: cada lado rola 1 dado
    int dado_atq = rolar_dado();
    int dado_def = rolar_dado();
    printf("Dado atacante: %d  |  Dado defensor: %d\n", dado_atq, dado_def);

    if (dado_atq > dado_def) {
        // defensor perde 1 tropa
        mapa[defensor_idx].tropas -= 1;
        printf("Atacante venceu: defensor perde 1 tropa.\n");
        if (mapa[defensor_idx].tropas <= 0) {
            // conquista: mudar cor e colocar 1 tropa (simples)
            printf("Defensor ficou sem tropas. Territorio %s conquistado!\n", mapa[defensor_idx].nome);
            // transferir propriedade: cor do defensor vira cor do atacante
            strncpy(mapa[defensor_idx].cor, mapa[atacante_idx].cor, COR_SIZE);
            mapa[defensor_idx].cor[COR_SIZE - 1] = '\0';
            // coloca 1 tropa e reduz 1 tropa do atacante para representar movimento
            mapa[defensor_idx].tropas = 1;
            mapa[atacante_idx].tropas -= 1;
            if (mapa[atacante_idx].tropas < 1) mapa[atacante_idx].tropas = 1; // garantir minimo
            printf("Tropas ajustadas apos conquista.\n");
        }
    } else if (dado_atq < dado_def) {
        // atacante perde 1 tropa
        mapa[atacante_idx].tropas -= 1;
        if (mapa[atacante_idx].tropas < 1) mapa[atacante_idx].tropas = 1;
        printf("Defensor venceu: atacante perde 1 tropa.\n");
    } else {
        // empate - regra do desafio: empate empata nada muda
        printf("Empate: nenhuma tropa perdida.\n");
    }
}

// conta quantos territorios pertencem a uma dada cor
int contar_por_cor(const Territorio *mapa, int n, const char *cor) {
    int cont = 0;
    for (int i = 0; i < n; i++) {
        if (strcasecmp(mapa[i].cor, cor) == 0) cont++;
    }
    return cont;
}

// verifica se todas tropas de uma cor foram destruídas (tropas == 0)
int destruiu_cor(const Territorio *mapa, int n, const char *cor) {
    for (int i = 0; i < n; i++) {
        if (strcasecmp(mapa[i].cor, cor) == 0 && mapa[i].tropas > 0) {
            return 0; // ainda existe tropa viva dessa cor
        }
    }
    return 1; // nenhuma tropa daquela cor com tropas>0
}

// verifica missões do Mestre
void verificar_missoes(const Territorio *mapa, int n, const char *meu_cor) {
    printf("\n--- Verificacao de Missoes (Mestre) ---\n");
    // Missao 1: Destruir o exército "Verde"
    int verde_destruido = destruiu_cor(mapa, n, "Verde");
    printf("Missao 1 - Destruir o exército Verde: %s\n", verde_destruido ? "CONCLUIDA" : "NAO CONCLUIDA");

    // Missao 2: Conquistar 3 territorios (ter 3 territorios com minha cor)
    int meus = contar_por_cor(mapa, n, meu_cor);
    printf("Missao 2 - Conquistar 3 territorios (sua cor: %s): %d territórios conquistados\n", meu_cor, meus);
    if (meus >= 3) printf("Missao 2: CONCLUIDA\n");
    else printf("Missao 2: NAO CONCLUIDA\n");

    if (verde_destruido && meus >= 3) {
        printf("\n*** PARABENS! Voce completou as duas missoes do Mestre! ***\n");
    } else {
        printf("\nContinue jogando para completar as missoes.\n");
    }
}

// inicializacao automatica (Mestre) - preenche 5 territorios com valores padrão
void inicializacao_automatica(Territorio mapa[MAX_TERRITORIOS]) {
    // nomes e cores simples pra teste
    const char *nomes[MAX_TERRITORIOS] = {"Aldeia", "Montanha", "Vila", "Costa", "Fortaleza"};
    const char *cores[MAX_TERRITORIOS] = {"Verde", "Azul", "Vermelho", "Amarelo", "Preto"};
    int tropas_iniciais[MAX_TERRITORIOS] = {3, 2, 4, 1, 5};

    for (int i = 0; i < MAX_TERRITORIOS; i++) {
        strncpy(mapa[i].nome, nomes[i], NOME_SIZE); mapa[i].nome[NOME_SIZE - 1] = '\0';
        strncpy(mapa[i].cor, cores[i], COR_SIZE); mapa[i].cor[COR_SIZE - 1] = '\0';
        mapa[i].tropas = tropas_iniciais[i];
    }
    printf("Inicializacao automatica concluida.\n");
}

int main() {
    // seed para aleatorio (usado no Aventureiro)
    srand((unsigned int) time(NULL));

    Territorio mapa_static[MAX_TERRITORIOS];
    Territorio *mapa_dyn = NULL; // será alocado se o jogador escolher o nivel Aventureiro
    int usando_dynamic = 0;

    char minha_cor[COR_SIZE];
    printf("Bem-vindo ao Desafio WAR Estruturado!\n");
    printf("Digite qual sera sua cor (por ex: Azul, Vermelho, Verde): ");
    ler_linha(minha_cor, sizeof(minha_cor));

    // menu principal
    int opcao = -1;
    while (opcao != 0) {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1 - Cadastro manual (Novato)\n");
        printf("2 - Inicializacao automatica (Mestre - rapido)\n");
        printf("3 - Mostrar mapa\n");
        printf("4 - Converter para Aventureiro (calloc) [ativa batalhas]\n");
        printf("5 - Atacar (Aventureiro)\n");
        printf("6 - Verificar missoes (Mestre)\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida. Tente novamente.\n");
            while (getchar() != '\n'); // limpa
            opcao = -1;
            continue;
        }
        while (getchar() != '\n'); // limpa \n

        if (opcao == 1) {
            cadastro_static(mapa_static);
            // mostrar automaticamente o mapa (Novato)
            mostrar_mapa(mapa_static, MAX_TERRITORIOS);
        } else if (opcao == 2) {
            inicializacao_automatica(mapa_static);
            mostrar_mapa(mapa_static, MAX_TERRITORIOS);
        } else if (opcao == 3) {
            if (usando_dynamic && mapa_dyn) {
                mostrar_mapa(mapa_dyn, MAX_TERRITORIOS);
            } else {
                mostrar_mapa(mapa_static, MAX_TERRITORIOS);
            }
        } else if (opcao == 4) {
            if (!usando_dynamic) {
                mapa_dyn = converte_para_dynamic(mapa_static, MAX_TERRITORIOS);
                if (mapa_dyn) usando_dynamic = 1;
            } else {
                printf("Mapa ja esta em versao dinamica.\n");
            }
        } else if (opcao == 5) {
            // atacar
            Territorio *mapa_para_usar = usando_dynamic && mapa_dyn ? mapa_dyn : mapa_static;
            printf("\nDigite o numero do territorio atacante (1 a %d): ", MAX_TERRITORIOS);
            int atq, def;
            if (scanf("%d", &atq) != 1) { printf("Entrada invalida.\n"); while (getchar() != '\n'); continue; }
            printf("Digite o numero do territorio defensor (1 a %d): ", MAX_TERRITORIOS);
            if (scanf("%d", &def) != 1) { printf("Entrada invalida.\n"); while (getchar() != '\n'); continue; }
            while (getchar() != '\n');
            // converte para indices 0-based
            atacar(mapa_para_usar, MAX_TERRITORIOS, atq - 1, def - 1);
            // mostra mapa apos batalha
            mostrar_mapa(mapa_para_usar, MAX_TERRITORIOS);
        } else if (opcao == 6) {
            // verificar missoes - sempre verifica no mapa atual (dynamic se existir)
            const Territorio *mapa_para_ver = usando_dynamic && mapa_dyn ? mapa_dyn : mapa_static;
            verificar_missoes(mapa_para_ver, MAX_TERRITORIOS, minha_cor);
        } else if (opcao == 0) {
            printf("Saindo... obrigado por jogar!\n");
        } else {
            printf("Opcao invalida. Tente novamente.\n");
        }
    }

    // liberar memoria caso tenha alocado
    if (mapa_dyn) free(mapa_dyn);
    return 0;
}
