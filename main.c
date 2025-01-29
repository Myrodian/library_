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
    struct Livro *prox;
} Livro;

Livro *tabela[TABLE_SIZE]; // Tabela hash
Livro *listaLivros = NULL; // Lista encadeada para todos os livros

int hash(int id);
void salvarLivros();
void carregarLivros();
void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero);
Livro *buscarLivro(int id);
void emprestarLivro(int id);
void devolverLivro(int id);
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
        fprintf(arquivo, "%d;%s;%s;%d;%s;%d\n", atual->id, atual->titulo, atual->autor, atual->ano, atual->genero, atual->emprestado);
        atual = atual->prox;
    }
    fclose(arquivo);
}

void carregarLivros() {
    FILE *arquivo = fopen("livros.txt", "r");
    if (!arquivo) return;
    int id, ano, emprestado;
    char titulo[100], autor[100], genero[50];
    while (fscanf(arquivo, "%d;%99[^;];%99[^;];%d;%49[^;];%d\n", &id, titulo, autor, &ano, genero, &emprestado) != EOF) {
        inserirLivro(id, titulo, autor, ano, genero);
        Livro *livro = buscarLivro(id);
        if (livro) livro->emprestado = emprestado;
    }
    fclose(arquivo);
}

void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero) {
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

    salvarLivros();
}

Livro *buscarLivro(int id) {
    int index = hash(id);
    Livro *atual = tabela[index];
    while (atual) {
        if (atual->id == id) return atual;
        atual = atual->prox;
    }
    return NULL;
}

void emprestarLivro(int id) {
    Livro *livro = buscarLivro(id);
    if (livro && !livro->emprestado) {
        livro->emprestado = 1;
        printf("Livro '%s' emprestado com sucesso!\n", livro->titulo);
        salvarLivros();
    } else {
        printf("Livro não encontrado ou já emprestado.\n");
    }
}

void devolverLivro(int id) {
    Livro *livro = buscarLivro(id);
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
    printf("\nLista de Livros:\n");
    while (atual) {
        printf("ID: %d | Título: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
               atual->id, atual->titulo, atual->autor, atual->ano, atual->emprestado ? "Sim" : "Não");
        atual = atual->prox;
    }
}

void menu() {
    int opcao, id, ano;
    char titulo[100], autor[100], genero[50];
    carregarLivros();

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
                printf("ID: "); scanf("%d", &id);
                getchar();
                printf("Título: "); fgets(titulo, sizeof(titulo), stdin); strtok(titulo, "\n");
                printf("Autor: "); fgets(autor, sizeof(autor), stdin); strtok(autor, "\n");
                printf("Ano: "); scanf("%d", &ano);
                getchar();
                printf("Gênero: "); fgets(genero, sizeof(genero), stdin); strtok(genero, "\n");
                inserirLivro(id, titulo, autor, ano, genero);
                break;
            case 2:
                printf("ID do Livro: "); scanf("%d", &id);
                Livro *livro = buscarLivro(id);
                if (livro) printf("Livro encontrado: %s - %s\n", livro->titulo, livro->autor);
                else printf("Livro não encontrado!\n");
                break;
            case 3:
                printf("ID do Livro para empréstimo: "); scanf("%d", &id);
                emprestarLivro(id);
                break;
            case 4:
                printf("ID do Livro para devolução: "); scanf("%d", &id);
                devolverLivro(id);
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
