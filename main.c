#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 101 // Tamanho da tabela hash

typedef struct Livro
{
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
void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero, int salvar);
Livro *buscarLivroPorCriterio();
void emprestarLivro();
void devolverLivro();
void listarLivros();
Livro *buscaDuplicata(Livro *lista, Livro *novo);

int hash(int id){ // Função para calcular o índice da tabela hash
    return id % TABLE_SIZE;
}

void salvarLivros() // função para salvar os livros no arquivo txt
{
    FILE *arquivo = fopen("livros.txt", "wb");
    if (!arquivo)
    {
        printf("Erro ao abrir arquivo para escrita.\n");
        return;
    }
    Livro *atual = listaLivros;
    while (atual)
    {
        fprintf(arquivo, "%d;%s;%s;%d;%s;%d\n", atual->id, atual->titulo, atual->autor, atual->ano, atual->genero, atual->emprestado);
        atual = atual->prox;
    }
    fclose(arquivo);
}

void carregarLivros() // essa função é somente para o programa carregar os livros do arquivo txt
{
    FILE *arquivo = fopen("livros.txt", "r");
    if (!arquivo)
        return;

    int id, ano, emprestado;
    char titulo[100], autor[100], genero[50];

    while (fscanf(arquivo, "%d;%99[^;];%99[^;];%d;%49[^;];%d\n",
                  &id, titulo, autor, &ano, genero, &emprestado) == 6)
    {
        // Verifica se o livro já está cadastrado
        int index = hash(id);
        Livro *atual = tabela[index];
        int existe = 0;

        while (atual)
        {
            if (atual->id == id)
            {
                existe = 1;
                break;
            }
            atual = atual->prox;
        }

        if (!existe)
        {
            inserirLivro(id, titulo, autor, ano, genero, 0);
            tabela[index]->emprestado = emprestado;
        }
    }
    fclose(arquivo);
}

void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero, int salvar) // função para inserir um livro na livraria
{
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

    if (salvar == 1){
        salvarLivros();
    }
}

Livro *buscarLivroPorCriterio() {
    int opcao;
    printf("Buscar livro por:\n");
    printf("1. ID\n");
    printf("2. Título\n");
    printf("3. Autor\n");
    printf("4. Ano\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    getchar(); // Para limpar o buffer do teclado

    Livro *resultado = NULL, *ultimo = NULL;

    if (opcao == 1) {
        // 📌 Busca direta por ID na tabela hash
        int id;
        printf("Digite o ID do livro: ");
        scanf("%d", &id);

        int index = hash(id);
        Livro *atual = tabela[index];

        while (atual) {
            if (atual->id == id) {
                // Retorna o livro encontrado diretamente
                return atual;
            }
            atual = atual->prox;
        }
        printf("Nenhum livro encontrado com ID %d.\n", id);
        return NULL;

    } else {
        char termoBusca[100];
        printf("Digite o termo de busca: ");
        fgets(termoBusca, sizeof(termoBusca), stdin);
        termoBusca[strcspn(termoBusca, "\n")] = 0; // Remover '\n'

        // 📌 Busca por Título, Autor ou Ano — Percorre apenas listas não vazias
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (tabela[i]) { // Só percorre posições que têm livros
                Livro *atual = tabela[i];

                while (atual) {
                    int encontrado = 0;

                    if (opcao == 2 && strcmp(atual->titulo, termoBusca) == 0) {
                        encontrado = 1;
                    } else if (opcao == 3 && strcmp(atual->autor, termoBusca) == 0) {
                        encontrado = 1;
                    } else if (opcao == 4 && atoi(termoBusca) == atual->ano) {
                        encontrado = 1;
                    }

                    if (encontrado) {
                        if (!buscaDuplicata(resultado, atual)) {  // Se ainda não foi adicionado
                            Livro *novoLivro = (Livro *)malloc(sizeof(Livro));
                            *novoLivro = *atual;
                            novoLivro->prox = resultado;
                            resultado = novoLivro;
                        }
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    if (!resultado) {
        printf("Nenhum livro encontrado com esse critério.\n");
    }
    return resultado;
}
Livro *buscaDuplicata(Livro *lista, Livro *novo) {
    while (lista) {
        if (strcmp(lista->titulo, novo->titulo) == 0 &&
            strcmp(lista->autor, novo->autor) == 0 &&
            lista->ano == novo->ano) {
            return lista;  // Já existe
            }
        lista = lista->prox;
    }
    return NULL;  // Não encontrado
}



void emprestarLivro() {
    Livro *encontrados = buscarLivroPorCriterio();
    if (!encontrados) {
        printf("Nenhum livro encontrado!\n");
        return;
    }

    // Contar quantos livros foram encontrados
    int count = 0;
    Livro *temp = encontrados;
    while (temp) {
        count++;
        temp = temp->prox;
    }

    if (count == 1) {
        // Apenas um livro encontrado
        if (encontrados->emprestado == 0) {
            encontrados->emprestado = 1;
            printf("Livro '%s' emprestado com sucesso!\n", encontrados->titulo);
            salvarLivros();
        } else {
            printf("Livro já está emprestado!\n");
        }
    } else {
        // Vários livros encontrados, listar e pedir para escolher
        printf("\nForam encontrados %d livros. Escolha pelo ID:\n", count);
        temp = encontrados;
        while (temp) {
            printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
                   temp->id, temp->titulo, temp->autor, temp->ano,
                   temp->emprestado ? "Sim" : "Nao");
            temp = temp->prox;
        }

        // Pedir ID do livro desejado
        int idEscolhido;
        printf("Digite o ID do livro que deseja emprestar: ");
        scanf("%d", &idEscolhido);

        // Encontrar o livro correto na tabela hash e emprestar
        int index = hash(idEscolhido);
        Livro *atual = tabela[index];
        while (atual) {
            if (atual->id == idEscolhido) {
                if (atual->emprestado == 0) {
                    atual->emprestado = 1;
                    printf("Livro '%s' emprestado com sucesso!\n", atual->titulo);
                    salvarLivros();
                } else {
                    printf("Livro já está emprestado!\n");
                }
                return;
            }
            atual = atual->prox;
        }
        printf("Livro com ID %d não encontrado!\n", idEscolhido);
    }
}

void devolverLivro() {
    Livro *encontrados = buscarLivroPorCriterio();
    if (!encontrados) {
        printf("Nenhum livro encontrado!\n");
        return;
    }

    int count = 0;
    Livro *temp = encontrados;
    while (temp) {
        count++;
        temp = temp->prox;
    }

    if (count == 1) {
        // Apenas um livro encontrado
        if (encontrados->emprestado == 1) {
            encontrados->emprestado = 0;
            printf("Livro '%s' devolvido com sucesso!\n", encontrados->titulo);
            salvarLivros();
        } else {
            printf("Livro não está emprestado!\n");
        }
    } else {
        // Vários livros encontrados, listar e pedir escolha
        printf("\nForam encontrados %d livros. Escolha pelo ID:\n", count);
        temp = encontrados;
        while (temp) {
            printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
                   temp->id, temp->titulo, temp->autor, temp->ano,
                   temp->emprestado ? "Sim" : "Nao");
            temp = temp->prox;
        }

        int idEscolhido;
        printf("Digite o ID do livro que deseja devolver: ");
        scanf("%d", &idEscolhido);

        int index = hash(idEscolhido);
        Livro *atual = tabela[index];
        while (atual) {
            if (atual->id == idEscolhido) {
                if (atual->emprestado == 1) {
                    atual->emprestado = 0;
                    printf("Livro '%s' devolvido com sucesso!\n", atual->titulo);
                    salvarLivros();
                } else {
                    printf("Livro não está emprestado!\n");
                }
                return;
            }
            atual = atual->prox;
        }
        printf("Livro com ID %d não encontrado!\n", idEscolhido);
    }
}


void listarLivros(){ // Função para listar todos os livros cadastrados
    Livro *atual = listaLivros;
    if (!atual){
        printf("\nNenhum livro cadastrado!\n");
        return;
    }
    printf("\nLista de Livros:\n");
    while (atual){
        printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
               atual->id, atual->titulo, atual->autor, atual->ano,
               atual->emprestado ? "Sim" : "Nao");
        atual = atual->prox;
    }
}
int main(){
    carregarLivros();
    int opcao, id, ano;
    char titulo[100], autor[100], genero[50];
    Livro *encontrados = NULL;
    do
    {
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

        switch (opcao)
        {
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
                encontrados = buscarLivroPorCriterio();
                if (encontrados) {
                    printf("\nLivros encontrados:\n");
                    Livro *atual = encontrados;
                    while (atual) {
                        printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
                               atual->id, atual->titulo, atual->autor, atual->ano,
                               atual->emprestado ? "Sim" : "Nao");
                        atual = atual->prox;
                    }
                } else {
                    printf("Nenhum livro encontrado.\n");
                }
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
    return 0;
}