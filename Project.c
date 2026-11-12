#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIMITE_OTIMO 30.0

// Definição das estruturas (Structs)
typedef struct {
    int id_produto;
    char nome[50];
    int quantidade_estoque;
    float consumo_diario;
    float autonomia_dias;
} Produto;

typedef struct {
    int linha;
    int corrigido;
    char descricao[100];
} ErroRegistro;

// Variáveis Globais do Sistema
Produto produtos[100];
int total_produtos = 0;
int dados_processados = 0;
ErroRegistro erros[50];
int total_erros = 0;
char caminho_arquivo[100] = "Nenhum arquivo selecionado";

// Protótipos das funções auxiliares
void imprimir_linha(char c, int n);
void limpar_entrada(void);
void ler_inteiro(const char *mensagem, int min, int max, int *destino);
int buscar_indice_por_id(int id);
float calcular_autonomia(int estoque, float consumo);
void ordenar_por_autonomia_bubble(Produto *vetor, int tamanho);
void selecionar_arquivo(void);
void processar_dados(void);
void listar_inconsistencias(void);
void corrigir_inconsistencia(void);
void ajustar_estoque(void);
void submenu_inconsistencias(void);
void exportar_saida(void);
void exibir_dashboard(void);

// Implementação das Funções Auxiliares Básicas
void imprimir_linha(char c, int n) {
    for (int i = 0; i < n; i++) {
        printf("%c", c);
    }
    printf("\n");
}

void limpar_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ler_inteiro(const char *mensagem, int min, int max, int *destino) {
    int lido;
    do {
        printf("%s", mensagem);
        lido = scanf("%d", destino);
        limpar_entrada();

        if (lido != 1 || *destino < min || *destino > max) {
            printf("Entrada invalida! Digite um valor entre %d e %d.\n", min, max);
        }
    } while (lido != 1 || *destino < min || *destino > max);
}

int buscar_indice_por_id(int id) {
    for (int i = 0; i < total_produtos; i++) {
        if (produtos[i].id_produto == id) {
            return i;
        }
    }
    return -1;
}

float calcular_autonomia(int estoque, float consumo) {
    if (consumo <= 0) return 999.0; // Evita divisão por zero
    return (float)estoque / consumo;
}

void ordenar_por_autonomia_bubble(Produto *vetor, int tamanho) {
    Produto temp;
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            // Ordenação decrescente (maior autonomia primeiro)
            if ((vetor + j)->autonomia_dias < (vetor + j + 1)->autonomia_dias) {
                temp = *(vetor + j);
                *(vetor + j) = *(vetor + j + 1);
                *(vetor + j + 1) = temp;
            }
        }
    }
}

// Funções de simulação/carregamento para testes iniciais
void selecionar_arquivo(void) {
    printf("\nDigite o nome do arquivo de entrada (ex: adubos.txt): ");
    fgets(caminho_arquivo, sizeof(caminho_arquivo), stdin);
    caminho_arquivo[strcspn(caminho_arquivo, "\n")] = 0; // Remove quebra de linha
    printf("Arquivo '%s' selecionado com sucesso!\n", caminho_arquivo);
}

void processar_dados(void) {
    // Simulando o carregamento de dados para teste rápido
    total_produtos = 3;
    
    produtos[0] = (Produto){101, "Composto Organico", 500, 10.0, 0};
    produtos[1] = (Produto){102, "Ureia Agricola", 150, 15.0, 0};
    produtos[2] = (Produto){103, "Fosfato Natural", 80, 5.0, 0};

    for (int i = 0; i < total_produtos; i++) {
        produtos[i].autonomia_dias = calcular_autonomia(produtos[i].quantidade_estoque, produtos[i].consumo_diario);
    }

    ordenar_por_autonomia_bubble(produtos, total_produtos);

    // Simulando um erro/inconsistência
    total_erros = 1;
    erros[0] = (ErroRegistro){4, 0, "Consumo diario zerado ou negativo na linha 4"};

    dados_processados = 1;
    printf("\nDados processados com sucesso! %d produtos carregados.\n", total_produtos);
}

void listar_inconsistencias(void) {
    printf("\n--- LISTA DE INCONSISTENCIAS ---\n");
    if (total_erros == 0) {
        printf("Nenhuma inconsistencia registrada.\n");
        return;
    }
    for (int i = 0; i < total_erros; i++) {
        if (erros[i].corrigido == 0) {
            printf("Erro [%d]: %s\n", erros[i].linha, erros[i].descricao);
        }
    }
}

void corrigir_inconsistencia(void) {
    printf("\n--- CORRECAO DE REGISTRO ---\n");
    if (total_erros == 0) {
        printf("Nao ha inconsistencias para corrigir.\n");
        return;
    }
    printf("Marcando primeira inconsistencia pendente como corrigida...\n");
    erros[0].corrigido = 1;
    printf("Registro corrigido com sucesso!\n");
}

// Ajusta a quantidade de estoque
void ajustar_estoque(void) {
    int id, tipo, movimento;
    int nova_quantidade;
    int posicao;

    printf("\n--- AJUSTE DE ESTOQUE ---\n");

    if (total_produtos == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }

    ler_inteiro("Digite o ID do produto: ", 1, 999999, &id);

    posicao = buscar_indice_por_id(id);

    if (posicao == -1) {
        printf("Produto nao encontrado.\n");
        return;
    }

    printf("Produto: %s\n", produtos[posicao].nome);
    printf("Estoque atual: %d kg\n", produtos[posicao].quantidade_estoque);

    ler_inteiro("Tipo [1] Entrada [2] Saida: ", 1, 2, &tipo);
    ler_inteiro("Quantidade: ", 1, 1000, &movimento);

    if (tipo == 1) {
        nova_quantidade = produtos[posicao].quantidade_estoque + movimento;
    } else {
        nova_quantidade = produtos[posicao].quantidade_estoque - movimento;
    }

    if (nova_quantidade < 0 || nova_quantidade > 1000) {
        printf("Estoque fora do limite permitido.\n");
        return;
    }

    produtos[posicao].quantidade_estoque = nova_quantidade;

    produtos[posicao].autonomia_dias = calcular_autonomia(
        nova_quantidade,
        produtos[posicao].consumo_diario
    );

    float nova_autonomia = produtos[posicao].autonomia_dias;

    ordenar_por_autonomia_bubble(produtos, total_produtos);

    printf("\nEstoque atualizado!\n");
    printf("Novo estoque: %d kg\n", nova_quantidade);
    printf("Nova autonomia: %.1f dias\n", nova_autonomia);
}

// Submenu de ajustes
void submenu_inconsistencias(void) {
    int opcao;

    if (dados_processados == 0) {
        printf("\nPrimeiro processe os dados na opcao 2.\n");
        return;
    }

    do {
        printf("\n");
        imprimir_linha('=', 55);
        printf("AJUSTE DE INCONSISTENCIAS\n");
        imprimir_linha('=', 55);

        printf("[1] Ver inconsistencias\n");
        printf("[2] Corrigir registro\n");
        printf("[3] Ajustar estoque\n");
        printf("[0] Voltar\n");

        imprimir_linha('-', 55);

        ler_inteiro("Escolha uma opcao: ", 0, 3, &opcao);

        switch (opcao) {
            case 1:
                listar_inconsistencias();
                break;

            case 2:
                corrigir_inconsistencia();
                break;

            case 3:
                ajustar_estoque();
                break;

            case 0:
                printf("Voltando ao menu...\n");
                break;
        }

    } while (opcao != 0);
}

// Exporta os dados para um arquivo
void exportar_saida(void) {
    if (dados_processados == 0) {
        printf("\nPrimeiro processe os dados na opcao 2.\n");
        return;
    }

    FILE *arq = fopen("adubos_saida.txt", "w");

    if (arq == NULL) {
        printf("Erro ao criar arquivo de saida.\n");
        return;
    }

    fprintf(arq, "Pos;ID;Produto;Estoque_kg;Consumo_kg_dia;Autonomia_dias;Situacao\n");

    for (int i = 0; i < total_produtos; i++) {
        char situacao[20];

        if (produtos[i].autonomia_dias >= LIMITE_OTIMO) {
            strcpy(situacao, "OTIMO");
        } else {
            strcpy(situacao, "DEFICITARIO");
        }

        fprintf(arq, "%d;%d;%s;%d;%.2f;%.1f;%s\n",
                i + 1,
                produtos[i].id_produto,
                produtos[i].nome,
                produtos[i].quantidade_estoque,
                produtos[i].consumo_diario,
                produtos[i].autonomia_dias,
                situacao);
    }

    fclose(arq);

    printf("\nArquivo adubos_saida.txt criado com sucesso!\n");
    printf("%d produtos exportados.\n", total_produtos);

    int pendentes = 0;

    for (int i = 0; i < total_erros; i++) {
        if (erros[i].corrigido == 0) {
            pendentes++;
        }
    }

    if (pendentes > 0) {
        printf("%d linha(s) invalida(s) ficaram de fora.\n", pendentes);
    }
}

// Mostra as estatisticas
void exibir_dashboard(void) {
    printf("\n");
    imprimir_linha('=', 65);
    printf("DASHBOARD DE ADUBOS E SUPLEMENTOS\n");
    imprimir_linha('=', 65);

    if (dados_processados == 0) {
        printf("Nenhum dado foi processado.\n");
        return;
    }

    if (total_produtos == 0) {
        printf("Nenhum produto valido encontrado.\n");
        return;
    }

    int estoque_total = 0;
    int otimos = 0;
    int deficitarios = 0;
    int pendentes = 0;
    float soma_autonomia = 0;

    for (int i = 0; i < total_produtos; i++) {
        estoque_total += produtos[i].quantidade_estoque;
        soma_autonomia += produtos[i].autonomia_dias;

        if (produtos[i].autonomia_dias >= LIMITE_OTIMO) {
            otimos++;
        } else {
            deficitarios++;
        }
    }

    for (int i = 0; i < total_erros; i++) {
        if (erros[i].corrigido == 0) {
            pendentes++;
        }
    }

    float media = soma_autonomia / total_produtos;
    float percentual_otimo = otimos * 100.0 / total_produtos;
    float percentual_deficit = deficitarios * 100.0 / total_produtos;

    printf("Arquivo ativo: %s\n", caminho_arquivo);
    printf("Total de produtos: %d\n", total_produtos);
    printf("Estoque total: %d kg\n", estoque_total);
    printf("Produtos otimos: %d (%.1f%%)\n", otimos, percentual_otimo);
    printf("Produtos deficitarios: %d (%.1f%%)\n", deficitarios, percentual_deficit);
    printf("Autonomia media: %.1f dias\n", media);
    printf("Maior autonomia: %s (%.1f dias)\n", produtos[0].nome, produtos[0].autonomia_dias);
    printf("Menor autonomia: %s (%.1f dias)\n", produtos[total_produtos - 1].nome, produtos[total_produtos - 1].autonomia_dias);
    printf("Inconsistencias pendentes: %d\n", pendentes);

    printf("\nStatus do estoque: ");

    if (percentual_otimo >= 70) {
        printf("OPERACIONAL\n");
    } else if (percentual_otimo >= 40) {
        printf("ATENCAO - REPOR ESTOQUE\n");
    } else {
        printf("CRITICO - REPOSICAO URGENTE\n");
    }

    printf("\nGRAFICO DE AUTONOMIA (# = 5 dias)\n");
    imprimir_linha('-', 65);

    for (int i = 0; i < total_produtos; i++) {
        int barras = (int)(produtos[i].autonomia_dias / 5);

        if (barras > 20) {
            barras = 20;
        }

        printf("%-23.23s | ", produtos[i].nome);

        for (int j = 0; j < barras; j++) {
            printf("#");
        }

        printf(" %.1f dias\n", produtos[i].autonomia_dias);
    }

    imprimir_linha('=', 65);
}

// Funcao principal
int main(void) {
    int opcao;
    int lido;

    do {
        printf("\n============================================\n");
        printf(" ERP SOLARPUNK - ADUBOS E SUPLEMENTOS\n");
        printf("============================================\n");
        printf("[1] Selecao do arquivo de entrada\n");
        printf("[2] Processamento do arquivo\n");
        printf("[3] Ajuste de dados\n");
        printf("[4] Geracao do arquivo de saida\n");
        printf("[5] Dashboard e estatisticas\n");
        printf("[0] Sair do sistema\n");
        printf("============================================\n");
        printf("Digite uma opcao: ");

        lido = scanf("%d", &opcao);

        if (lido != 1) {
            opcao = -1;
        }

        limpar_entrada();

        switch (opcao) {
            case 1:
                selecionar_arquivo();
                break;

            case 2:
                processar_dados();
                break;

            case 3:
                submenu_inconsistencias();
                break;

            case 4:
                exportar_saida();
                break;

            case 5:
                exibir_dashboard();
                break;

            case 0:
                printf("\nSaindo do sistema...\n");
                break;

            default:
                printf("\nOpcao invalida!\n");
        }

    } while (opcao != 0);

    return 0;
}