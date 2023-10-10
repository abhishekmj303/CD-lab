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
    // printf("All: %s\n", cfg.all);
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

void printFirstFollow(Set ff[], int n)
{
    printf("\nFirsts Set:\n");
    for (int i = 0; i < n; i++)
    {
        if (ff[i].x == '#' || ff[i].x == '$')
            continue;
        printf("%c: %s\n", ff[i].x, ff[i].first);
    }
    printf("\nFollows Set:\n");
    for (int i = 0; i < n; i++)
    {
        if (ff[i].x == '#' || ff[i].x == '$')
            continue;
        printf("%c: %s\n", ff[i].x, ff[i].follow);
    }
}

void printLL1Table(char table[MAX][MAX][MAX], CFG cfg)
{
    int max_width(char table[MAX][MAX][MAX], int r, int c)
    {
        int max = 0;
        for (int i = 0; i < r; i++)
        {
            for (int j = 0; j < c; ++j)
            {
                int len = strlen(table[i][j]);
                if (len > max)
                    max = len;
            }
        }
        return max;
    }

    void printchar(char c, int n)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%c", c);
        }
    }

    int r = strlen(cfg.non_terms);
    int c = strlen(cfg.terms);
    int width = max_width(table, r, c);

    printf("\nLL(1) Table:\n ");

    for (int i = 0; i < c; i++)
    {
        printf("  ║  ");
        int left = (width - 1)/2;
        int right = width - left - 1;
        printchar(' ', left);
        printf("%c", cfg.terms[i]);
        printchar(' ', right);
    }
    printf("\n");

    printf("═");
    for (int i = 0; i < c; i++)
    {
        printf("══╬══");
        printchar('=', width);
    }
    printf("\n");

    for (int i = 0; i < r; i++)
    {
        printf("%c", cfg.non_terms[i]);
        for (int j = 0; j < c; j++)
        {
            int len = strlen(table[i][j]);
            int spaces = width - len;
            printf("  ║  ");
            printchar(' ', spaces/2);
            printf("%s", table[i][j]);
            printchar(' ', spaces - spaces/2);
        }
        printf("\n");
    }
}

int strunion(char *dest, char *src)
{
    int changed = 0;
    for (int i = 0; i < strlen(src); i++)
    {
        if (strchr(dest, src[i]) == NULL)
        {
            strncat(dest, src + i, 1);
            changed = 1;
        }
    }
    return changed;
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

void follow(CFG cfg, Set ff[MAX])
{
    int n_all = strlen(cfg.all);

    for (int i = 0; i < n_all; i++) // for each (A ∈ N) FOLLOW(A) = ∅
    {
        ff[i].x = cfg.all[i];
        if(ff[i].x == cfg.start)
            ff[i].follow[0] = '$';
        else
            strcpy(ff[i].follow, "");
    }

    int changed = 1;
    while(changed)
    {
        changed = 0;
        for(int i = 0; i < cfg.num_rules; i++)
        {
            for(int j = 0; j < cfg.rules[i].num_prods; j++)
            {
                char non_term = cfg.rules[i].non_term;
                char *prod = cfg.rules[i].prods[j];

                int index = strchr(cfg.all, non_term) - cfg.all;
                int k = strlen(prod) - 1;

                int x_index = strchr(cfg.all, prod[k]) - cfg.all;
                changed |= strunion(ff[x_index].follow, ff[index].follow);

                char *rest = ff[index].follow;
                for(k = strlen(prod)-1; k >= 1; k--)
                {
                    int x_i_index = strchr(cfg.all, prod[k]) - cfg.all;
                    char *x_i_first_set = ff[x_i_index].first;
                    char x_i_1 = prod[k - 1];
                    int x_i_1_index = strchr(cfg.all, x_i_1) - cfg.all;

                    char* eps_i = strchr(x_i_first_set, '#');
                    if(eps_i != NULL)
                    {
                        *eps_i = '\0';   // remove '#'
                        strcat(x_i_first_set, eps_i+1);
                        changed |= strunion(ff[x_i_1_index].follow, x_i_first_set);
                        changed |= strunion(ff[x_i_1_index].follow, rest);
                        strcat(x_i_first_set, "#");
                    }
                    else
                    {
                        changed |= strunion(ff[x_i_1_index].follow, x_i_first_set);
                    }
                    rest = ff[x_i_index].follow;
                }
            }
        }
    }
}

void first_follow(CFG cfg, Set ff[MAX])
{
    int nt = strlen(cfg.non_terms);
    int t = strlen(cfg.terms);

    for (int i = 0; i < nt; i++)
    {
        Rule *rule = &cfg.rules[i];
        ff[i].x = rule->non_term;
        for (int j = 0; j < rule->num_prods; j++)
        {
            strunion(ff[i].first, first_set(&cfg, rule->prods[j]));
        }
    }

    for (int i = 0; i < t; i++)
    {
        int ff_idx = nt + i;
        ff[ff_idx].x = cfg.terms[i];
        ff[ff_idx].first[0] = cfg.terms[i];
    }

    follow(cfg, ff);

    printFirstFollow(ff, nt + t);
}

int ll1_table(CFG cfg, Set ff[MAX], char table[MAX][MAX][MAX])
{
//    int r = strlen(cfg.non_terms);
//    int c = strlen(cfg.terms);
    char non_term;
    char *prod;
    char dirsym[MAX] = {0};

    int result = 1;

    for (int i = 0; i < cfg.num_rules; i++)
    {
        for (int j = 0; j < cfg.rules[i].num_prods; j++)
        {
            non_term = cfg.rules[i].non_term;
            prod = cfg.rules[i].prods[j];
            memset(dirsym, 0, sizeof(dirsym));
            int follow_req = 0;
            // union to dirsym, while epsilon in first set
            for (int k = 0; k < strlen(prod); k++)
            {
                int x_index = strchr(cfg.all, prod[k]) - cfg.all;
                char *x_first_set = ff[x_index].first;
                char *eps = strchr(x_first_set, '#');
                if (eps != NULL)
                {
                    *eps = '\0';
                    strcat(x_first_set, eps + 1);
                    strunion(dirsym, x_first_set);
                    follow_req = 1;
                    strcat(x_first_set, "#");
                }
                else
                {
                    strunion(dirsym, x_first_set);
                    follow_req = 0;
                    break;
                }
            }

            if (follow_req == 1)
            {
                int x_index = strchr(cfg.all, non_term) - cfg.all;
                strunion(dirsym, ff[x_index].follow);
            }

            // add to table, for each terminal in dirsym
            for (int k = 0; k < strlen(dirsym); k++)
            {
                int x_index = strchr(cfg.terms, dirsym[k]) - cfg.terms;
                int y_index = strchr(cfg.non_terms, non_term) - cfg.non_terms;
                if (table[y_index][x_index][0] != '\0')
                {
                    strcat(table[y_index][x_index], " | ");
                    result = 0;
                }
                strcat(table[y_index][x_index], prod);
            }
        }
    }

    printLL1Table(table, cfg);

    return result;
}


void ll1_parser(CFG cfg)
{
    // First and Follow sets
    Set ff[MAX];
    first_follow(cfg, ff);

    // LL(1) Table
    char table[MAX][MAX][MAX];
    int result = ll1_table(cfg, ff, table);
    if (result)
        printf("\nGiven grammar is LL(1)\n");
    else
    {
        printf("\nGiven grammar is not LL(1)\n");
        return;
    }
}


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <CFG-input-file>\n", argv[0]);
        return 1;
    }
//    argv[1] = "CFG";

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
    strcpy(cfg.all, cfg.non_terms);
    strcat(cfg.all, cfg.terms);

    printCFG(cfg);

    fclose(fp);

    ll1_parser(cfg);

    return 0;
}