# cuddly-octo-lamp
Copiando colunas de dados do cronograma do SARC para o cronograma do Atas.


# Programa executável

Programa compilado para macOS, disponível em "Releases".

No Finder, utilize Ctrl+Open para habilitar a execução do programa s2a.

# Instruções para compilação

No terminal, digite:

```
% git clone https://github.com/masmangan/cuddly-octo-lamp.git
% cd cuddly-octo-lamp
% gcc `xml2-config --cflags --libs` -o s2a s2a.c
```


# Utilização:

Passo 1) Escolha uma turma, neste exemplo, a turma 13 da disciplina 4645G.

Passo 2) Exporte o cronograma da turma a partir do SARC.

No SARC, selecione a turma e aperte o botão "Exportar HTML". O arquivo cronograma.html é salvo na pasta de Downloads.

Passo 3) Exporte o cronograma da turma a partir do Atas.

No sistema de Atas, selecione o "dashboard" da turma, aperte o botão "Exportar cronograma de aula". O arquivo com código da turma e extensão .csv é salvo na pasta de Downloads.

Verifique que os dois arquivos de cronograma estão na mesma pasta. Copie os arquivos para uma pasta vazia, para evitar utilizar o arquivo incorreto. 

```
% mkdir 4645G-04-13
% cd 4645G-04-13
% ls
4645G-04-13-2023-1-24707-1542023175141.csv
cronograma.html
```

Passo 4) Utilize o s2a para preencher o arquivo de cronograma do Atas com os dados do arquivo de cronograma do SARC.

Copie o arquivo executável s2a para a pasta criada. Execute o programa informando os nomes dos dois arquivos de cronograma.
Primeiro o arquivo do SARC.

```
% ./s2a cronograma.html 4645G-04-13-2023-1-24707-2042023214952.csv 
```

O arquivo do Atas é preenchido com as colunas atividade e descrição provenientes do arquivo do SARC.

Passo 5) Importe o cronograma da turma de volta para o Atas.

No sistema de Atas, selecione o "dashboard" da turma, aperte o botão "Importar cronograma de aula". Selecione o arquivo de cronograma com extensão .csv. Aperte o botão "Upload".

A mensagem de confirmação do Atas deve indicar "Arquivo importado com sucesso!". Aperte o botão "Ok". 

Preencha manualmente a semana de G2. Preencha os campos Metodologia de Ensino e Forma de Avaliação.
Aperte o botão "Incluir atividades extraclasse" e preencha as atividades.

Repita para a próxima turma.




