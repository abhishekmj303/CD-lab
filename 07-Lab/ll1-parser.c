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

typedef struct
{
    char data[MAX];
    int top;
    int cap;
} Stack;


CFG cfg;
Set ff[MAX];
char table[MAX][MAX][MAX];
Stack stack;


void print_cfg()
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

void print_first_follow()
{
    int n = strlen(cfg.all);

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

void print_ll1_table()
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

char *strrmchr(char *str, char c)
{
    char *c_ptr = strchr(str, c);
    if (c_ptr == NULL)
        return str;

    char *res = (char *)calloc(strlen(str)+1, strlen(str)+1);

    *c_ptr = '\0';
    strcat(res, str);
    strcat(res, c_ptr+1);
    *c_ptr = c;

    return res;
}

char *rm_epsilon(char *str)
{
    return strrmchr(str, '#');
}

int is_epsilon_in(char *str)
{
    if (strchr(str, '#'))
        return 1;
    else
        return 0;
}

int indexof(char *str, char c)
{
    return strchr(str, c) - str;
}

char *first(char x)
{
    int idx = indexof(cfg.all, x);
    return ff[idx].first;
}

char *follow(char x)
{
    int idx = indexof(cfg.all, x);
    return ff[idx].follow;
}

void compute_first()
{
    int n_all = strlen(cfg.all);

    for (int i = 0; i < n_all; i++)
    {
        if (!isupper(ff[i].x)) // for each (a ∈ T) FIRST(a) = {a}
            ff[i].first[0] = ff[i].x;
        else                   // for each (A ∈ N) FIRST(A) = ∅
            strcpy(ff[i].first, "");
    }

    int changed = 1;
    while (changed)
    {
        changed = 0;
        for(int i = 0; i < cfg.num_rules; i++)
        {
            for(int j = 0; j < cfg.rules[i].num_prods; j++)
            {
                char A = cfg.rules[i].non_term;  // A
                char *X = cfg.rules[i].prods[j]; // X0,X1,X2,...,Xn-1
                char *first_A = first(A);

                int k = 0, end = strlen(X)-1;

                changed |= strunion(first_A, rm_epsilon(first(X[k])));

                while (is_epsilon_in(first(X[k])) && k < end)
                {
                    changed |= strunion(first_A, rm_epsilon(first(X[k+1])));
                    k++;
                }
                if (k == end && is_epsilon_in(first(X[end])))
                {
                    changed |= strunion(first_A, "#");
                }
            }
        }
    }
}

void compute_follow()
{
    int n_all = strlen(cfg.all);

    for (int i = 0; i < n_all; i++) // for each (A ∈ N) FOLLOW(A) = ∅
    {
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
                char A = cfg.rules[i].non_term;  // A
                char *X = cfg.rules[i].prods[j]; // X0,X1,X2,...,Xn-1

                int k = strlen(X) - 1;

                changed |= strunion(follow(X[k]), follow(A));
                char *rest = follow(A);

                for (; k > 0; k--)
                {
                    char *follow_Xk1 = follow(X[k-1]);
                    char *first_Xk = first(X[k]);

                    if (is_epsilon_in(first_Xk))
                    {
                        changed |= strunion(follow_Xk1, rm_epsilon(first_Xk));
                        changed |= strunion(follow_Xk1, rest);
                    }
                    else
                    {
                        changed |= strunion(follow_Xk1, first_Xk);
                    }
                    rest = follow_Xk1;
                }
            }
        }
    }
}

void compute_first_follow()
{
    for (int i = 0; i < strlen(cfg.all); i++)
    {
        ff[i].x = cfg.all[i];
    }

    compute_first();
    compute_follow();
    print_first_follow();
}

char *ll1_table(char non_term, char term)
{
    int nt_idx = indexof(cfg.non_terms, non_term);
    int t_idx = indexof(cfg.terms, term);
    return table[nt_idx][t_idx];
}

int compute_ll1_table()
{
    char dirsym[MAX] = {0};

    int result = 1;

    for (int i = 0; i < cfg.num_rules; i++)
    {
        for (int j = 0; j < cfg.rules[i].num_prods; j++)
        {
            char A = cfg.rules[i].non_term;
            char *X = cfg.rules[i].prods[j];
            memset(dirsym, 0, sizeof(dirsym));
            int follow_req = 0;

            // union to dirsym, while epsilon in first set
            for (int k = 0; k < strlen(X); k++)
            {
                char *first_Xk = first(X[k]);
                if (is_epsilon_in(first_Xk))
                {
                    strunion(dirsym, rm_epsilon(first_Xk));
                    follow_req = 1;
                }
                else
                {
                    strunion(dirsym, first_Xk);
                    follow_req = 0;
                    break;
                }
            }

            if (follow_req == 1)
            {
                strunion(dirsym, follow(A));
            }

            // add to table, for each terminal in dirsym
            for (int k = 0; k < strlen(dirsym); k++)
            {
                char *table_prod = ll1_table(A, dirsym[k]);
                if (table_prod[0] != '\0')
                {
                    strcat(table_prod, " | ");
                    result = 0;
                }
                strcat(table_prod, X);
            }
        }
    }

    print_ll1_table();

    return result;
}

void push_stack(char c)
{
    if (stack.top == stack.cap-1)
        return;
    
    stack.data[++stack.top] = c;
}

char pop_stack()
{
    if (stack.top < 0)
        return '\0';

    char res = stack.data[stack.top];
    stack.data[stack.top--] = '\0';
    return res;
}

char top_stack()
{
    if (stack.top < 0)
        return '\0';
    
    return stack.data[stack.top];
}

int is_stack_empty()
{
    return (stack.top < 0);
}

void ll1_parser(char *str)
{
    // First and Follow sets
    compute_first_follow();

    // LL(1) Table
    int result = compute_ll1_table();
    if (result)
        printf("\nGiven grammar is LL(1)\n");
    else
    {
        printf("\nGiven grammar is not LL(1)\n");
        exit(1);
    }

    stack.cap = MAX;
    stack.top = -1;

    int i = 0;
    push_stack(cfg.start);
    while (!is_stack_empty())
    {
        char top = top_stack();
        if (!isupper(top)) // top is terminal
        {
            if (top == '#') // top is epsilon
            {
                pop_stack();
            }
            else if (top == str[i])
            {
                pop_stack();
                i++;
            }
            else
            {
                printf("String %s is rejected\n", str);
                exit(1);
            }
        }
        else // top is non-terminal
        {
            char *prod = ll1_table(top, str[i]);
            pop_stack();
            for (int k = strlen(prod)-1; k >= 0; k--)
            {
                push_stack(prod[k]);
            }
        }
    }

    if (i != strlen(str))
    {
        printf("String %s is rejected\n", str);
        exit(1);
    }
    printf("String %s is accepted\n", str);
}


int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: %s <CFG-input-file> <string>\n", argv[0]);
        return 1;
    }
    // argv[1] = "CFG";

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("Error: Unable to open file %s\n", argv[1]);
        return 1;
    }

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
        n = indexof(cfg.non_terms, token[0]);

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

    print_cfg();

    fclose(fp);

    ll1_parser(argv[2]);

    return 0;
}