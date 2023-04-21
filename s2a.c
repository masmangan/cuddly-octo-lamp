/* s2a.c, derived from code from libxml2 tree examples.
 * Copyright (C) 2023 Marco Mangan.
 *
 *
 * synopsis: Navigates a tree to print selected element contents
 * purpose: Parse a HTML file to a tree, use libxml2 xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          selected elements contents in document order.
 *
 *      HTML file is invalid. Replace atribute refer with refer="refer" to
 *          correct that before parsing.
 *
 *      Selected elements are the only H1, and some TD  elements from the only TABLE.
 *
 *      H1 content must be split out in in fields: course_id, course_name, and
 *          class_id. Course_id value is content until first dash.
 *          Course_name is content from first space until left parenteses from last
 *          pair of parentheses on the content. Class_id is content is between
 *          the last pair of parenthesis on the content.
 *          The table has seven columns. Table header must be ignored.
 *          Only table columns 1, 4 and 5 must be processed.
 *	    Column 1 is an integer, referred as number, a sequential row counter starting from 1.
 *          Column 4 is a string, referred as description, output as is.
 *          Column 5 is a string, referred as activity, output recoded.
 *          Output must include activity and description, in that order, for each row.
 *          Activity values must be recoded as a integer value. Recoding
 *          is as follows: "Prova" becomes 2, "Prova G2" becomes 3,
 *          "Prova de Substituição" becomes 4, "Trabalho" becomes 7,
 *          otherwise, activity value becomes 6.
 *          Some table rows must be ignored.
 *          If the row table row style includes the color Red, the row must be ignored and do not add to the row counter.
 *          If column 1 is a blank value, recode that as a integer that
 *          continues the sequential row counter from last row number.
 *          All rows without a counter are at the bottom of the table.
 *
 * compiling instructions:
 * 	% gcc `xml2-config --cflags --libs` -o s2a s2a.c
 *
 * usage:
 *	% ./s2a <sarc file>  <atas file>
 *
 * test:
 *	% s2a cronograma.html 4..csv
 *
 * specification author: Marco Mangan
 * code author: Marco Mangan, based on code from Dodji Seketeli
 */
#include <string.h>
#include <ctype.h>

#include <stdio.h>

#include <fcntl.h>

#include <dirent.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

// ATAS
#define CD_DISCIPLINA_ORIGEM 0
#define CD_DISCIPLINA 1
#define CD_TURMA 2
#define NR_CREDITO 3
#define NR_AULA 4
#define CD_TIPO_AULA 5
#define TX_PLANO_AULA 6
#define DT_AULA 7

// SARC
#define NUMERO 0
#define DIA 1
#define DATA 2
#define HORA 3
#define DESCRICAO 4
#define ATIVIDADE 5
#define RECURSOS 6


// https://stackoverflow.com/questions/26522583/c-strtok-skips-second-token-or-consecutive-delimiter
char *strtok2(char *string, char const *delimiter)
{
    static char *source = NULL;
    char *p, *riturn = 0;
    if (string != NULL)
        source = string;
    if (source == NULL)
        return NULL;

    if ((p = strpbrk(source, delimiter)) != NULL)
    {
        *p = 0;
        riturn = source;
        source = ++p;
    }
    return riturn;
}

char *ltrim(char *s)
{
    while (isspace(*s))
        s++;
    return s;
}

char *rtrim(char *s)
{
    char *back = s + strlen(s);
    while (isspace(*--back))
        ;
    *(back + 1) = '\0';
    return s;
}

char *trim(char *s)
{
    return rtrim(ltrim(s));
}

int ativ(char *s)
{
    if (!strcmp("Prova", s))
        return 2;

    if (!strcmp("Prova de G2", s))
        return 3;

    if (!strcmp("Prova de Substituição", s))
        return 4;

    if (!strcmp("Trabalho", s))
        return 7;

    return 6;
}

char *getid(char *s)
{
    char *o = s;
    while (*s != ' ')
        s++;
    *s = *(s - 1);
    s--;
    *s++ = '0';
    s++;
    *s = '\0';
    return o;
}

char *getclass(char *s)
{
    char *back = s + strlen(s);
    while (*--back != ')')
        ;
    *back = '\0';
    while (*--back != '(')
        ;
    back++;
    return back;
}

void usage()
{
    printf("Utilização:\n\t./s2a <cronograma do SARC em HTML> <cronograma do Atas em CSV>\n");
    exit(0);
}

#ifdef LIBXML_TREE_ENABLED



int i = 0;
int jumpHeader = 1;
xmlChar *lastdescription;
char *sarc_filename;
char *atas_filename;
char tmp_atas[100] = "tmp-atas-XXXXXX";
char tmp_sarc[100] = "tmp-sarc-XXXXXX";

FILE *fd_atas;
FILE *fd_sarc_tmp;

char line[400];

/* 
 * removes refer attribute from SARC file 
 * writes temp file as global tmp2
 */
void fixrefer(char *source)
{
    printf("Fixing SARC file refer boolean attribute...\n");
    FILE *f = fopen(source, "r");

    int ok = mkstemp(tmp_sarc);
    if (ok == -1)
    {
        printf("   *** Error: temporary file could not be created! Bye!\n");
        exit(2);
    }
    //printf("\nTEMP FILE: %s\n", tmp2);
    FILE *fd_fix = fopen(tmp_sarc, "w");

    /* removes lines with scripts to remove refer attribute error */
    while (fgets(line, 300, f))
        if (!strstr(line, "<script defer src=") && !strstr(line, "></script>"))
            fputs(line, fd_fix);

    fclose(fd_fix);
}


/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints values of selected xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode *a_node)
{
    xmlNode *cur_node = NULL;
    for (cur_node = a_node; cur_node; cur_node = cur_node->next)
    {
        if (cur_node->type == XML_ELEMENT_NODE)
        {

            if (!strcmp("tr", (const char *)(cur_node->name)))
            {
                xmlChar *style = xmlGetProp(cur_node, (const xmlChar *)"style");

                if (strstr((char *)style, "background-color:Red") || strstr((char *)style, "background-color:LightGrey"))
                {
                    xmlFree(style);
                    continue;
                }
                else
                {
                    xmlFree(style);
                }

                if (jumpHeader)
                {
                    jumpHeader = 0;
                    continue;
                }
            }
            if (!strcmp("H1", (const char *)(cur_node->name)))
            {
                char *h1 = (char *)xmlNodeGetContent(cur_node);
                char *class = getclass(h1);
                char *id = getid(h1);
                char t[100];
                sprintf(t, "%s-%s", id, class);
                if (strstr(atas_filename, t))
                {
                    printf("Arquivos são de turmas diferentes!");
                    exit(1);
                }
                fd_atas = fopen(atas_filename, "r");

                int ok = mkstemp(tmp_atas);
                if (ok == -1)
                {
                    exit(2);
                }
                fd_sarc_tmp = fopen(tmp_atas, "w");

                fgets(line, 300, fd_atas);
                fputs(line, fd_sarc_tmp);
            }

            if (!strcmp("td", (const char *)(cur_node->name)))
            {
                if (i % 7 == DESCRICAO)
                {
                    lastdescription = xmlNodeGetContent(cur_node);
                }

                if (i % 7 == ATIVIDADE)
                {
                    fgets(line, 300, fd_atas);
                    char *duh = trim(line);

                    char *token;
                    token = strtok2(duh, ";");
                    fprintf(fd_sarc_tmp, "%s;", token);
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%s;", token);
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%s;", token);
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%s;", token);
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%s;", token);
                    token = strtok2(NULL, ";");
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%d;%s;", ativ(trim((char *)xmlNodeGetContent(cur_node))),
                            trim((char *)lastdescription));
                    token = strtok2(NULL, ";");
                    fprintf(fd_sarc_tmp, "%s;\n", token ? token : "");
                    xmlFree(lastdescription);
                }
                i++;
            }
        }

        print_element_names(cur_node->children);
    }
}

/**
 * Simple example to parse a file,
 * walk down the DOM, and print
 * selected XML	elements  value.
 */
int main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    LIBXML_TEST_VERSION

    if (argc != 3)
    {
        usage();
    }

    sarc_filename = argv[1];
    atas_filename = argv[2];

    fixrefer(sarc_filename);

    /* starts DOM parsing from temp SARC file */

    printf("Parsing SARC file...\n");
    doc = xmlReadFile(tmp_sarc, NULL, 0);

    if (doc == NULL)
    {
        printf("   ***Error: could not parse temporary file %s\n", tmp_sarc);
        exit(10);
    }

    i = 0;
    jumpHeader = 1;
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    fclose(fd_atas);
    fclose(fd_sarc_tmp);

    remove(tmp_sarc);
    remove(atas_filename);
    rename(tmp_atas, atas_filename);
    return 0;
}
#else
int main(void)
{
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
