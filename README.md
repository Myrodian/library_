# Gerenciador de Biblioteca em C

Este projeto é um sistema simples de gerenciamento de biblioteca desenvolvido em C, permitindo o cadastro, busca, empréstimo, devolução e listagem de livros.

## Funcionalidades

- **Cadastro de Livros:** Permite adicionar livros com ID, título, autor, ano e gênero.
- **Busca de Livros:** Pesquisa por título, autor ou ano.
- **Empréstimo de Livros:** Marca um livro como emprestado para um usuário específico.
- **Devolução de Livros:** Atualiza o status de um livro emprestado para disponível.
- **Listagem de Livros:** Exibe todos os livros cadastrados e seus status.
- **Persistência de Dados:** Os livros são armazenados em um arquivo (`livros.txt`), garantindo que os dados sejam mantidos entre execuções.

## Como foi desenvolvido

- Utiliza **listas encadeadas** para armazenar os livros dinamicamente.
- Usa uma **tabela hash** para indexação eficiente dos livros pelo ID.
- Manipulação de arquivos para salvar e carregar os dados automaticamente.
- Interação via **menu interativo** no terminal.

## Como rodar o código

1. Compile o código com um compilador C, como `gcc`:
   ```sh
   gcc biblioteca.c -o biblioteca
   ```
2. Execute o programa:
   ```sh
   ./biblioteca
   ```

Ao rodar o programa, um menu será exibido para interação. Os dados serão automaticamente carregados e salvos no arquivo `livros.txt`. que está dentro da pasta cmake-build-debug.

