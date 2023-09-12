// Example CFG:
// V = S$
// S = aAS
// S = c
// A = ba
// A = SB
// B = bA
// B = S

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

typedef struct {
    char non_term;
    char prods[MAX][MAX];
    int num_prods;
} Rule;

typedef struct {
    char start;
    char non_terms[MAX];
    char terms[MAX];
    Rule rules[MAX];
    int num_rules;
} CFG;

void printCFG(CFG cfg)
{
    printf("CFG:\n");
    printf("Start: %c\n", cfg.start);
    printf("Non-terminals: %s\n", cfg.non_terms);
    printf("Terminals: %s\n", cfg.terms);
    printf("Rules:\n");
    for (int i = 0; i < cfg.num_rules; i++)
    {
        printf("%c", cfg.rules[i].non_term);
        for (int j = 0; j < cfg.rules[i].num_prods; j++)
        {
            printf(" -> %s \n ", cfg.rules[i].prods[j]);
        }
        printf("\r");
    }
}


char *next_terms(CFG *cfg, char *prod)
{
    char next[MAX];
    memset(next, 0, sizeof(next));
    
    if (isupper(prod[0]))
    {
        int n = strchr(cfg->non_terms, prod[0]) - cfg->non_terms;
        for (int i = 0; i < cfg->rules[n].num_prods; i++)
        {
            strcat(next, next_terms(cfg, cfg->rules[n].prods[i]));
        }
    }
    else
    {
        next[0] = prod[0];
    }

    char *ret = malloc(sizeof(next));
    strcpy(ret, next);
    return ret;
}

void ll1_table(CFG cfg)
{
    // Table
    int n_nt = strlen(cfg.non_terms);
    int n_t = strlen(cfg.terms);
    int n_r = cfg.num_rules;
    Rule table[n_nt][n_t];
    
    int total = 0;
    // For all rules with a non-terminal
    for (int i = 0; i < n_r; i++)
    {
        // For all productions of the rule
        for (int j = 0; j < cfg.rules[i].num_prods; j++)
        {
            char *next = next_terms(&cfg, cfg.rules[i].prods[j]);
            for (int k = 0; k < strlen(next); k++)
            {
                int n = strchr(cfg.non_terms, cfg.rules[i].non_term) - cfg.non_terms;
                int m = strchr(cfg.terms, next[k]) - cfg.terms;
                strcpy(table[n][m].prods[0], cfg.rules[i].prods[j]);
                printf("(%c, %c): %c -> %s\n", cfg.rules[i].non_term, next[k], cfg.rules[i].non_term, table[n][m].prods[0]);
                total++;
            }
        }
    }

    printf("Total: %d\n", total);

    // printTable(&cfg, table);
}

int main(int argc, char *argv[])
{
    // if (argc < 2) {
    //     printf("Usage: %s <CFG-input-file>\n", argv[0]);
    //     return 1;
    // }
    argv[1] = "CFG";

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: Unable to open file %s\n", argv[1]);
        return 1;
    }

    CFG cfg;
    int n = 0, end = 0;
    char line[MAX];
    while (fgets(line, MAX, fp) != NULL)
    {
        // Non terminal
        char *token = strtok(line, " =\n");
        if (strchr(cfg.non_terms, token[0]) == NULL) {
            strncat(cfg.non_terms, token, 1);
            cfg.rules[end].non_term = token[0];
            cfg.rules[end].num_prods = 0;
            end++;
        }
        n = strchr(cfg.non_terms, token[0]) - cfg.non_terms;

        // Production
        token = strtok(NULL, " =\n");
        for (int i = 0; i < strlen(token); i++)
        {
            if (strchr(cfg.terms, token[i]) == NULL && !isupper(token[i]))
                strncat(cfg.terms, token + i, 1);
        }
        strcpy(cfg.rules[n].prods[cfg.rules[n].num_prods], token);
        cfg.rules[n].num_prods++;
    }
    cfg.start = cfg.rules[0].non_term;
    cfg.num_rules = end;

    printCFG(cfg);

    fclose(fp);

    ll1_table(cfg);

    return 0;
}