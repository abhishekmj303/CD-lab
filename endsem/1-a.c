// Abhshek M J
// CS21B2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

int n;

int isopr(char c)
{
    switch(c)
    {
        case '+':
        case '-':
        case '*':
        case '/':
            return 1;
        default:
            return 0;
    }
}

int main(int argc, char *argv[])
{
    char str[MAX];
    strcpy(str, argv[1]);
    n = strlen(str);

    char all_tokens[n][n], unq_tokens[n][n];
    int a = 0, u = 0;

    char tok_type[10];

    for (int i = 0; i < n; i++)
    {
        char token[n];
        int j = 0;

        if (isdigit(str[i]))
        {
            while(isdigit(str[i]))
            {
                token[j++] = str[i++];
            }
            strcpy(tok_type, "int");
            if (str[i] == '.')
            {
                token[j++] = str[i++];
                while(isdigit(str[i]))
                {
                    token[j++] = str[i++];
                }
                strcpy(tok_type, "float");
            }
            i--;
        }
        if (isopr(str[i]))
        {
            token[j++] = str[i];
            strcpy(tok_type, "operator");
        }
        token[j++] = '\0';
        
        printf("%s is %s\n", token, tok_type);

        strcpy(all_tokens[a++], token);
        
        int in_unq = 0;
        for (int k = 0; k < u; k++)
        {
            if (strcmp(unq_tokens[k], token) == 0)
            {
                in_unq = 1;
                break;
            }
        }
        if (!in_unq)
        {
            strcpy(unq_tokens[u++], token);
        }
    }
    printf("Total Tokens: %d\n", a);
    
    printf("\nUnique Tokens:\n");
    printf("Total: %d\n", u);
    for (int i = 0; i < u; i++)
    {
        printf("%s\n", unq_tokens[i]);
    }


    return 0;
}
