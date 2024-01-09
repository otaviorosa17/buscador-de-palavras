Para compilar o programa corretamente utilize a seguinte linha de comando:
gcc main.c -o ep1

No caso, o nome do arquivo executável gerado será "ep1" como no 
exemplo dado no enunciado do EP, mas este nome é arbitrário.


Para exectuar o programa, basta utilizar a seguinte linha de comando:
./ep1 test.txt arvore

Lembre-se de alterar os valores de acordo com a maneira que o programa será utilizado:
- "ep1" é o nome do programa, e deve ser alterado de acordo com o nome dado durante a compilação.

- "test.txt" é o nome do arquivo que será utilizado como base para se fazer a busca de palavras,
caso queira usar outro arquivo .txt como base, basta movê-lo para pasta do programa e alterar o nome do arquivo
na linha de comando. Exemplo:
./ep1 texto.txt arvore

- "arvore" é o tipo de índice que será utilizado durante o uso do programa, os dois tipos disponíveis são
"lista" e "arvore".
