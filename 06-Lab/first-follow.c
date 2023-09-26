// Example CFG:
// V = S$
// S = aAS
// S = #
// A = ba
// A = SB
// B = cA
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
    char all[MAX];
    char non_terms[MAX];
    char terms[MAX];
    Rule rules[MAX];
    int num_rules;
} CFG;

typedef struct {
    char x;
    char first[MAX];
    char follow[MAX];
} Set;

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

void strunion(char *dest, char *src)
{
    for (int i = 0; i < strlen(src); i++)
    {
        if (strchr(dest, src[i]) == NULL)
            strncat(dest, src + i, 1);
    }
}

char *first_set(CFG *cfg, char *prod)
{
    char first[MAX];
    memset(first, 0, sizeof(first));

    for (int i = 0; i < strlen(prod); i++)
    {
        int next = strlen(first);
        if (isupper(prod[i])) // non-terminal
        {
            int n = strchr(cfg->non_terms, prod[i]) - cfg->non_terms;
            for (int j = 0; j < cfg->rules[n].num_prods; j++)
            {
                strunion(first, first_set(cfg, cfg->rules[n].prods[j]));
            }

            // If epsilon is in first set, add next terms
            if (strchr(first+next, '#') == NULL)
                break;
        }
        else // terminal
        {
            first[i] = prod[i];

            // If terminal is epsilon, add next terms
            if (prod[i] != '#')
                break;
        }
    }

    char *ret = malloc(sizeof(first));
    strcpy(ret, first);
    return ret;
}

void follow_set(CFG *cfg, Set ff[], char nt, char *prod)
{
    for (int i = 0; i < strlen(prod)-1; i++)
    {
        if (isupper(prod[i])) // non-terminal
        {
            int fl_n = strchr(cfg->non_terms, prod[i]) - cfg->non_terms;
            strunion(ff[fl_n].follow, ff[fl_n+1].first);
        }
    }
}

void first_follow(CFG cfg)
{
    Set ff[MAX];
    int nt = strlen(cfg.non_terms);
    int t = strlen(cfg.terms);

    printf("Firsts Set:\n");
    for (int i = 0; i < nt; i++)
    {
        Rule *rule = &cfg.rules[i];
        ff[i].x = rule->non_term;
        for (int j = 0; j < rule->num_prods; j++)
        {
            strunion(ff[i].first, first_set(&cfg, rule->prods[j]));
        }
        printf("%c: %s\n", ff[i].x, ff[i].first);
    }

    for (int i = 0; i < t; i++)
    {
        int ff_idx = nt + i;
        ff[ff_idx].x = cfg.terms[i];
        ff[ff_idx].first[0] = cfg.terms[i];
        printf("%c: %s\n", ff[ff_idx].x, ff[ff_idx].first);
    }

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

    first_follow(cfg);

    return 0;
}