#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 101 // Tamanho da tabela hash

// Definição da estrutura Livro
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

// Protótipos das funções
int hash(int id);
void salvarLivros();
void carregarLivros();
void inserirLivro(int id, char *titulo, char *autor, int ano, char *genero, int salvar);
Livro *buscarLivroPorCriterio();
Livro *buscaDuplicata(Livro *lista, Livro *novo);
void emprestarLivro();
void devolverLivro();
void listarLivros();
void removerLivro();  // Função para remover livro

// Função para calcular o índice da tabela hash
int hash(int id) {
    return id % TABLE_SIZE;
}

// Função para salvar os livros no arquivo txt
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

// Função para carregar os livros do arquivo txt
void carregarLivros() {
    FILE *arquivo = fopen("livros.txt", "r");
    if (!arquivo)
        return;

    int id, ano, emprestado;
    char titulo[100], autor[100], genero[50];

    while (fscanf(arquivo, "%d;%99[^;];%99[^;];%d;%49[^;];%d\n", &id, titulo, autor, &ano, genero, &emprestado) == 6) {
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

// Função para inserir um livro na livraria
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
    // Inserção na tabela hash
    novo->prox = tabela[index];
    tabela[index] = novo;

    // Inserção na lista global de livros
    novo->prox = listaLivros;
    listaLivros = novo;

    if (salvar == 1) {
        salvarLivros();
    }
}

// Função para buscar livro por critério (não retorna livros emprestados)
Livro *buscarLivroPorCriterio() {
    int opcao;
    printf("Buscar livro por:\n");
    printf("1. ID\n");
    printf("2. Titulo\n");
    printf("3. Autor\n");
    printf("4. Ano\n");
    printf("Escolha uma opcao: ");
    scanf("%d", &opcao);
    getchar(); // Para limpar o buffer do teclado

    Livro *resultado = NULL;

    if (opcao == 1) {
        // Busca direta por ID na tabela hash
        int id;
        printf("Digite o ID do livro: ");
        scanf("%d", &id);

        int index = hash(id);
        Livro *atual = tabela[index];

        while (atual) {
            // Verifica se o livro possui o ID procurado e se não está emprestado
            if (atual->id == id && atual->emprestado == 0) {
                return atual; // Retorna o livro encontrado diretamente
            }
            atual = atual->prox;
        }
        printf("Nenhum livro encontrado com ID %d ou ele esta emprestado.\n", id);
        return NULL;
    } else {
        char termoBusca[100];
        printf("Digite o termo de busca: ");
        fgets(termoBusca, sizeof(termoBusca), stdin);
        termoBusca[strcspn(termoBusca, "\n")] = 0; // Remover '\n'

        // Busca por Titulo, Autor ou Ano — percorre a tabela hash
        for (int i = 0; i < TABLE_SIZE; i++) {
            if (tabela[i]) {
                Livro *atual = tabela[i];
                while (atual) {
                    // Só consideramos livros que não estão emprestados
                    if (atual->emprestado == 0) {
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
                    }
                    atual = atual->prox;
                }
            }
        }
    }

    if (!resultado) {
        printf("Nenhum livro encontrado com esse criterio.\n");
    }
    return resultado;
}

// Função para verificar duplicatas
Livro *buscaDuplicata(Livro *lista, Livro *novo) {
    while (lista) {
        if (strcmp(lista->titulo, novo->titulo) == 0 &&
            strcmp(lista->autor, novo->autor) == 0 &&
            lista->ano == novo->ano) {
            return lista; // Já existe
        }
        lista = lista->prox;
    }
    return NULL; // Não encontrado
}

// Função para emprestar livro
void emprestarLivro() {
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
        if (encontrados->emprestado == 0) {
            encontrados->emprestado = 1;
            printf("Livro '%s' emprestado com sucesso!\n", encontrados->titulo);
            salvarLivros();
        } else {
            printf("Livro ja esta emprestado!\n");
        }
    } else {
        printf("\nForam encontrados %d livros. Escolha pelo ID:\n", count);
        temp = encontrados;
        while (temp) {
            printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
                   temp->id, temp->titulo, temp->autor, temp->ano,
                   temp->emprestado ? "Sim" : "Nao");
            temp = temp->prox;
        }

        int idEscolhido;
        printf("Digite o ID do livro que deseja emprestar: ");
        scanf("%d", &idEscolhido);

        int index = hash(idEscolhido);
        Livro *atual = tabela[index];
        while (atual) {
            if (atual->id == idEscolhido) {
                if (atual->emprestado == 0) {
                    atual->emprestado = 1;
                    printf("Livro '%s' emprestado com sucesso!\n", atual->titulo);
                    salvarLivros();
                } else {
                    printf("Livro ja esta emprestado!\n");
                }
                return;
            }
            atual = atual->prox;
        }
        printf("Livro com ID %d nao encontrado!\n", idEscolhido);
    }
}

// Função para devolver livro
void devolverLivro() {
    Livro *emprestados = NULL;
    Livro *temp = listaLivros;

    // Procurando livros emprestados
    while (temp) {
        if (temp->emprestado == 1) {
            Livro *novoLivro = (Livro *)malloc(sizeof(Livro));
            *novoLivro = *temp;
            novoLivro->prox = emprestados;
            emprestados = novoLivro;
        }
        temp = temp->prox;
    }

    if (!emprestados) {
        printf("Nao ha livros emprestados.\n");
        return;
    }

    printf("Livros atualmente emprestados:\n");
    temp = emprestados;
    while (temp) {
        printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Emprestado: %s\n",
               temp->id, temp->titulo, temp->autor, temp->ano,
               temp->emprestado ? "Sim" : "Nao");
        temp = temp->prox;
    }

    // Permitir o usuário devolver um livro
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
                printf("Livro nao esta emprestado!\n");
            }
            return;
        }
        atual = atual->prox;
    }

    printf("Livro com ID %d nao encontrado!\n", idEscolhido);
}

// Função para listar todos os livros cadastrados
void listarLivros() {
    Livro *atual = listaLivros;
    if (!atual) {
        printf("\nNenhum livro cadastrado!\n");
        return;
    }
    printf("\nLista de Livros:\n");
    while (atual) {
        printf("ID: %d \t | Titulo: %s | Autor: %s | Ano: %d | Genero: %s | Emprestado: %s\n",
               atual->id, atual->titulo, atual->autor, atual->ano, atual->genero,
               atual->emprestado ? "Sim" : "Nao");
        atual = atual->prox;
    }
}

// Função para remover um livro cadastrado
void removerLivro() {
    int id;
    printf("Digite o ID do livro que deseja remover: ");
    scanf("%d", &id);
    getchar(); // Limpa o buffer

    // Primeiro, busca o livro na lista global de livros
    Livro *ant = NULL, *cur = listaLivros;
    while (cur != NULL && cur->id != id) {
        ant = cur;
        cur = cur->prox;
    }
    if (cur == NULL) {
        printf("Livro com ID %d nao encontrado!\n", id);
        return;
    }
    // Verifica se o livro está emprestado; se estiver, não permitimos a remoção
    if (cur->emprestado == 1) {
        printf("Nao e possivel remover um livro emprestado.\n");
        return;
    }
    // Remove o livro da lista global
    if (ant == NULL) { // O livro a remover e o primeiro da lista
        listaLivros = cur->prox;
    } else {
        ant->prox = cur->prox;
    }

    // Remove o livro da tabela hash
    int index = hash(id);
    Livro *prev = NULL, *node = tabela[index];
    while (node != NULL && node->id != id) {
        prev = node;
        node = node->prox;
    }
    if (node != NULL) {
        if (prev == NULL) {
            tabela[index] = node->prox;
        } else {
            prev->prox = node->prox;
        }
    }

    free(cur);
    salvarLivros();
    printf("Livro removido com sucesso!\n");
}

int main() {
    carregarLivros();
    int opcao, id, ano;
    char titulo[100], autor[100], genero[50];
    Livro *encontrados = NULL;

    do {
        printf("\nMenu:\n");
        printf("1 - Cadastrar Livro\n");
        printf("2 - Buscar Livro\n");
        printf("3 - Emprestar Livro\n");
        printf("4 - Devolver Livro\n");
        printf("5 - Listar Livros\n");
        printf("6 - Remover Livro\n");
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
                encontrados = buscarLivroPorCriterio();
                if (encontrados) {
                    printf("\nLivros encontrados:\n");
                    Livro *atual = encontrados;
                    while (atual) {
                        printf("ID: %d | Titulo: %s | Autor: %s | Ano: %d | Genero: %s | Emprestado: %s\n",
                               atual->id, atual->titulo, atual->autor, atual->ano, atual->genero,
                               atual->emprestado ? "Sim" : "Nao");
                        atual = atual->prox;
                    }
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
            case 6:
                removerLivro();
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
