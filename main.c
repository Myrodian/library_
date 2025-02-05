#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 101 // Tamanho da tabela hash

typedef struct Livro {
    int id;
    char titulo[100];
    char autor[100];
    int ano;
    char genero[50];
    int emprestado;
    char user_borrowed[100];
    struct Livro *prox;
} Livro;

Livro *tabela[TABLE_SIZE]; // Tabela hash
Livro *listaLivros = NULL; // Lista encadeada para todos os livros

int hash(int id);
void salvarLivros();
void carregarLivros();
void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero, int salvar);
Livro *buscarLivroPorCriterio();
void emprestarLivro();
void devolverLivro();
void listarLivros();
void menu();


int hash(int id) {
    return id % TABLE_SIZE;
}

void salvarLivros() {
    FILE *arquivo = fopen("livros.txt", "wb");
    if (!arquivo) {
        printf("Erro ao abrir arquivo para escrita.\n");
        return;
    }
    Livro *atual = listaLivros;
    while (atual) {
        fprintf(arquivo, "%d;%s;%s;%d;%s;%d;%s\n", atual->id, atual->titulo, atual->autor, atual->ano, atual->genero, atual->emprestado, atual->user_borrowed);
        atual = atual->prox;
    }
    fclose(arquivo);
}

void carregarLivros() {
    FILE *arquivo = fopen("livros.txt", "r");
    if (!arquivo) return;

    int id, ano, emprestado;
    char titulo[100], autor[100], genero[50];

    while (fscanf(arquivo, "%d;%99[^;];%99[^;];%d;%49[^;];%d\n",
                  &id, titulo, autor, &ano, genero, &emprestado) == 6) {
        // Verifica se o livro já está cadastrado
        int index = hash(id);
        Livro *atual = tabela[index];
        int existe = 0;

        while (atual) {
            if (atual->id == id) {
                existe = 1;
                break;
            }
            atual = atual->prox;
        }

        if (!existe) {
            inserirLivro(id, titulo, autor, ano, genero, 0);
            tabela[index]->emprestado = emprestado;
        }
    }
    fclose(arquivo);
}

void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero, int salvar) {
    Livro *novo = (Livro *)malloc(sizeof(Livro));
    novo->id = id;
    strcpy(novo->titulo, titulo);
    strcpy(novo->autor, autor);
    novo->ano = ano;
    strcpy(novo->genero, genero);
    novo->emprestado = 0;
    novo->prox = NULL;

    int index = hash(id);
    novo->prox = tabela[index];
    tabela[index] = novo;

    novo->prox = listaLivros;
    listaLivros = novo;

    if (salvar) {
        salvarLivros();
    }
}


Livro *buscarLivroPorCriterio() {
    int tipo = 0;
    char criterio[100];
    do {
        printf("Buscar por: 1-Titulo, 2-Autor, 3-Ano: ");
        scanf("%d", &tipo);
        if (tipo == 1 || tipo == 2) {
            printf("Digite o termo: ");
            scanf(" %99[^\n]", criterio);
        } else if (tipo == 3) {
            printf("Digite o ano: ");
            scanf("%s", criterio);
        } else {
            printf("Opcao invalida!\n");
        }
    }while(tipo <= 0 || tipo > 3);
    for (int i = 0; i < TABLE_SIZE; i++) {
        Livro *atual = tabela[i];
        while (atual) {
            if ((tipo == 1 && strcmp(atual->titulo, criterio) == 0) ||
                (tipo == 2 && strcmp(atual->autor, criterio) == 0) ||
                (tipo == 3 && atoi(criterio) == atual->ano)) {
                printf("Livro encontrado: %s - %s (%d)\n", atual->titulo, atual->autor, atual->ano);
                return atual; // Retorna o livro encontrado
                }
            atual = atual->prox;
        }
    }
    return NULL; // Retorna NULL se nenhum livro for encontrado
}

void emprestarLivro() {
    Livro *livro = buscarLivroPorCriterio();
    char borrow[100];
    if (livro) {
        if (!livro->emprestado) {
            livro->emprestado = 1;
            printf("para quem será emprestado o livro?\n");
            getchar();
            fgets(borrow, sizeof(borrow), stdin);
            strtok(borrow, "\n");
            strcpy(livro->user_borrowed, borrow);
            printf("Livro '%s' emprestado com sucesso!\n", livro->titulo);
            salvarLivros();
        } else {
            printf("Livro já está emprestado.\n");
        }
    } else {
        printf("Livro não encontrado.\n");
    }
}

void devolverLivro() {
    Livro *livro = buscarLivroPorCriterio();
    if (livro && livro->emprestado) {
        livro->emprestado = 0;
        printf("Livro '%s' devolvido com sucesso!\n", livro->titulo);
        salvarLivros();
    } else {
        printf("Livro não encontrado ou não estava emprestado.\n");
    }
}

void listarLivros() {
    Livro *atual = listaLivros;

    // Debug: Verificar se a lista encadeada tem nós
    if (!atual) {
        printf("\nNenhum livro cadastrado!\n");
        return;
    }

    printf("\nLista de Livros:\n");
    while (atual) {
        printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s | Para quem?: %s\n",
               atual->id, atual->titulo, atual->autor, atual->ano,
               atual->emprestado ? "Sim" : "Nao",
               strlen(atual->user_borrowed) == 0 ? "Ninguem" : atual->user_borrowed);

        atual = atual->prox; // Move para o próximo livro
    }
}





// void listarLivros() {
//     Livro *atual = listaLivros;
//     printf("\nLista de Livros:\n");
//     while (atual) {
//         printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s | Para quem?: %s\n",
//         atual->id, atual->titulo, atual->autor, atual->ano, atual->emprestado ? "Sim" : "Nao", strlen(atual->user_borrowed) == 0 ? "ninguem" : atual->user_borrowed);
//         atual = atual->prox;
//     }
// }

void menu() {
    carregarLivros();
    int opcao, id, ano;
    char titulo[100], autor[100], genero[50];
    do {
        printf("\nMenu:\n");
        printf("1 - Cadastrar Livro\n");
        printf("2 - Buscar Livro\n");
        printf("3 - Emprestar Livro\n");
        printf("4 - Devolver Livro\n");
        printf("5 - Listar Livros\n");
        printf("0 - Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1:
                printf("ID: ");
                scanf("%d", &id);
                getchar();
                printf("Titulo: ");
                fgets(titulo, sizeof(titulo), stdin);
                strtok(titulo, "\n");
                printf("Autor: ");
                fgets(autor, sizeof(autor), stdin);
                strtok(autor, "\n");
                printf("Ano: ");
                scanf("%d", &ano);
                getchar();
                printf("Genero: ");
                fgets(genero, sizeof(genero), stdin);
                strtok(genero, "\n");
                inserirLivro(id, titulo, autor, ano, genero, 1);
                break;
            case 2:
                buscarLivroPorCriterio();
                break;
            case 3:
                emprestarLivro();
                break;
            case 4:
                devolverLivro();
                break;
            case 5:
                listarLivros();
                break;
        }
    } while (opcao != 0);
}

int main() {
    menu();
    return 0;
}
