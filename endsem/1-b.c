// Abhshek M J
// CS21B2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

typedef struct
{
	int key;
	char value[MAX];
} Pair;

int e;
Pair *expr;

char *value_of(int key)
{
	for (int i = 0; i < e; i++)
	{
		if (expr[i].key == key)
		{
			return expr[i].value;
		}
	}
	return NULL;
}

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

int precedence(char c)
{
	switch(c)
	{
		case '/': return 4;
		case '*': return 3;
		case '+': return 2;
		case '-': return 1;
		default: return 0;
	}
}

char *infix_to_postfix(char *infix)
{
	int n = strlen(infix);
	char stack[n]; int top = -1;

	char *postfix = (char *) calloc(n+1, n+1);
	int j = 0;
	
	expr = (Pair *) malloc(sizeof(Pair)*n);
	e = 0;
	
	int key = 'A';
	
	for (int i = 0; i < n; i++)
	{
		// printf("%d\n", i);
		if (isdigit(infix[i]))
		{
			expr[e].key = key;
			int k = 0;
			while(!isopr(infix[i]))
			{
				expr[e].value[k++] = infix[i++];
			}
			i--;
			
			printf("%c -> %s\n", key, expr[e].value);
			e++;
			
			postfix[j++] = key++;
		}
		else
		{
			int curr = precedence(infix[i]);
			while (precedence(stack[top]) >= curr)
			{
				postfix[j++] = stack[top--];
			}
			stack[++top] = infix[i];
		}
	}

	while (top != -1)
	{
		postfix[j++] = stack[top--];
	}
	
	return postfix;
}

void print_arg(char arg)
{
	if (isdigit(arg))
	{
		printf("t%c ", arg);
	}
	else
	{
		printf("%s ", value_of(arg));
	}
}

void three_address_code(char *postfix)
{
	int n = strlen(postfix);
	char stack[n], top = -1;

	int t = 1;

	for (int i = 0; i < n; i++)
	{
		if (isopr(postfix[i]))
		{
			char arg2 = stack[top--];
			char arg1 = stack[top--];

			printf("t%d = ", t);
			print_arg(arg1);
			printf("%c ", postfix[i]);
			print_arg(arg2);
			printf("\n");

			stack[++top] = t+'0';
			t++;
		}
		else
		{
			stack[++top] = postfix[i];
		}
	}
}

int main(int argc, char *argv[])
{
    char str[MAX];
    strcpy(str, argv[1]);
    int n = strlen(str);
    
    char *postfix = infix_to_postfix(str);
	printf("Postfix: %s\n", postfix);

	printf("Three address code:\n");
	three_address_code(postfix);
}
