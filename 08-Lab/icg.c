#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

typedef struct
{
    char data[MAX];
    int top;
    int cap;
} Stack;

Stack init_stack()
{
    Stack stack;
    stack.top = -1;
    stack.cap = MAX;

    return stack;
}

void push_stack(Stack *stack, char c)
{
    if (stack->top == stack->cap-1)
        return;
    
    stack->data[++stack->top] = c;
}

char pop_stack(Stack *stack)
{
    if (stack->top < 0)
        return '\0';

    char res = stack->data[stack->top];
    stack->data[stack->top--] = '\0';
    return res;
}

char top_stack(Stack stack)
{
    if (stack.top < 0)
        return '\0';
    
    return stack.data[stack.top];
}

int is_stack_empty(Stack stack)
{
    return (stack.top < 0);
}

int precedence(char c)
{
    switch (c)
    {
        case '+': case '-':
            return 1;
        case '*': case '/':
            return 2;
        default:
            return 0;
    }
}

char *infix_to_postfix(char *infix)
{
    int n = strlen(infix);
    Stack stack = init_stack();
    char *postfix = (char *) calloc(n+1, n+1);
    int j = 0;

    for (int i = 0; i < n; i++)
    {
        if (isalpha(infix[i]))
        {
            postfix[j++] = infix[i];
        }
        else if (infix[i] == '(')
        {
            push_stack(&stack, '(');
        }
        else if (infix[i] == ')')
        {
            while (top_stack(stack) != '(')
            {
                postfix[j++] = pop_stack(&stack);
            }
            pop_stack(&stack); // pop '(' but don't push
        }
        else // operators
        {
            int cur_precedence = precedence(infix[i]);
            while (precedence(top_stack(stack)) >= cur_precedence)
            {
                postfix[j++] = pop_stack(&stack);
            }
            push_stack(&stack, infix[i]);
        }
    }

    while (!is_stack_empty(stack))
    {
        postfix[j++] = pop_stack(&stack);
    }

    return postfix;
}

void print_arg(char arg)
{
    if (isdigit(arg))
        printf("t%c ", arg);
    else
        printf("%c ", arg);
}

void three_address_code(char *postfix)
{
    Stack stack = init_stack();
    int t = 1;

    for (int i = 0, n = strlen(postfix); i < n; i++)
    {
        if (isalpha(postfix[i]))
        {
            push_stack(&stack, postfix[i]);
        }
        else
        {
            char arg2 = pop_stack(&stack);
            char arg1 = pop_stack(&stack);

            printf("t%d = ", t); // result
            print_arg(arg1);
            printf("%c ", postfix[i]); // operator
            print_arg(arg2);
            printf("\n");

            push_stack(&stack, t+'0');
            t++;
        }
    }
}

void icg(char *infix)
{
    char *postfix = infix_to_postfix(infix);
    printf("Postfix: %s\n", postfix);

    printf("Intermediate 3-Address Code:\n");
    three_address_code(postfix);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <infix-expression>\n", argv[0]);
        exit(1);
    }
    // argv[1] = "a+b*c-d/(b*c)";

    icg(argv[1]);

    return 0;
}