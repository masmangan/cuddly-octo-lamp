# cuddly-octo-lamp
Simple parser from HTML to CSV 


# Programa executável

Programa compilado para macOS, disponível em "Releases".

# Instruções para compilação

No terminal, digite:

```
% git clone https://github.com/masmangan/cuddly-octo-lamp.git
% cd cuddly-octo-lamp
% gcc `xml2-config --cflags --libs` -o s2a s2a.c
```


# Utilização:

Passo 1) Escolha uma turma, neste caso a turma 13 da disciplina 4645G.

Passo 2) Exporte o cronograma da turma a partir do SARC.

Passo 3) Exporte o cronograma da turma a partir do Atas.

Na pasta, os dois arquivos de cronograma devem estar visíveis. 

```
% ls
4645G-04-13-2023-1-24707-1542023175141.csv
cronograma.html
```

Passo 4) Utilize o s2a para preencher o arquivo de cronograma do Atas com os dados do arquivo de cronograma do SARC.

```
% ./s2a cronograma.html 4645G-04-13-2023-1-24707-2042023214952.csv 
```

Passo 5) Importe o cronograma da turma de volta para o Atas.
