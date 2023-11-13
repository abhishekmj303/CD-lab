#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

typedef struct
{
    int key;
    char value[1000];
    int true_l;
    int false_l;
} Pair;

int e, t;
Pair *expressions = NULL;
Pair *inter_code = NULL;

Pair *init_pair_arr(int n)
{
    return (Pair *) malloc(sizeof(Pair) * n);
}

int index_of(Pair pair_arr[], int n, int key)
{
    for (int i = 0; i < n; i++)
    {
        if (pair_arr[i].key == key)
        {
            return i;
        }
    }
    return -1;
}

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
        case '|':
            return 1;
        case '&':
            return 2;
        default:
            return 0;
    }
}

int is_bool_op(char c)
{
    if (c == '|' || c == '&')
        return 1;
    else
        return 0;
}

int is_comp_op(char c)
{
    if (c == '<' || c == '>')
        return 1;
    else
        return 0;
}

char *preprocess(char *infix)
{
    int n = strlen(infix);
    char *new_infix = (char *) calloc(2*n, 2*n);
    int j = 0;

    int prev_comp = 0;
    for (int i = 0; i < n; i++)
    {
        if (is_comp_op(infix[i]))
        {
            if (prev_comp == 1)
            {
                strcat(new_infix, "&&");
                new_infix[j+2] = new_infix[j-1];
                j += 3;
            }
            prev_comp = 1;
        }
        else if (!isalpha(infix[i]))
        {
            prev_comp = 0;
        }
        new_infix[j++] = infix[i];
    }

    return new_infix;
}

char *infix_to_postfix(char *infix)
{
    int n = strlen(infix);
    Stack stack = init_stack();
    char *postfix = (char *) calloc(n+1, n+1);
    int j = 0;

    expressions = init_pair_arr(n);
    e = 0;
    int expr_num = 'A'; // start with A

    for (int i = 0; i < n; i++)
    {
        if (isalpha(infix[i]))
        {
            // Get the expression string
            char *expr_str = (char *) calloc(n+1, n+1);
            int k = 0;
            do
            {
                expr_str[k++] = infix[i++];
            } while (i < n && !is_bool_op(infix[i]));
            if (i < n && isalpha(infix[i]))
                expr_str[k] = infix[i];
            else
                i--;
            
            // Store it as a key-value pair
            printf("%c -> %s\n", expr_num, expr_str);
            expressions[e].key = expr_num;
            strcpy(expressions[e].value, expr_str);
            e++;
            free(expr_str);

            postfix[j++] = expr_num++;
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
            i++;
        }
    }

    while (!is_stack_empty(stack))
    {
        postfix[j++] = pop_stack(&stack);
    }

    return postfix;
}

Pair *condition_code(int expr_key, int *l)
{
    int i = index_of(inter_code, t, expr_key);
    if (i != -1)
    {
        return &inter_code[i];
    }

    int expr_i = index_of(expressions, e, expr_key);
    if (expr_i == -1)
    {
        printf("Invalid expr_key %d\n", expr_key);
        return NULL;
    }
    inter_code[t].key = expr_key;
    sprintf(inter_code[t].value, "if %s goto L%d\ngoto L%d\n", expressions[expr_i].value, *l, *l+1);
    inter_code[t].true_l = *l;
    inter_code[t].false_l = *l+1;
    *l += 2;
    t++;

    return &inter_code[t-1];
}

void three_address_code(char *postfix)
{
    Stack stack = init_stack();
    int n = strlen(postfix);

    inter_code = init_pair_arr(n);
    t = 0; // index for inter_code

    int l = 1; // label number

    for (int i = 0; i < n; i++)
    {
        if (isalpha(postfix[i]))
        {
            push_stack(&stack, postfix[i]);
        }
        else
        {
            int exp2 = pop_stack(&stack);
            int exp1 = pop_stack(&stack);
            char opr = postfix[i];

            Pair *exp1_code = condition_code(exp1, &l);
            Pair *exp2_code = condition_code(exp2, &l);

            inter_code[t].key = t;
            char *comb_code = inter_code[t].value;
            if (opr == '&')
            {
                sprintf(comb_code, "%sL%d: %s", exp1_code->value, exp1_code->true_l, exp2_code->value);
                sprintf(comb_code+strlen(comb_code), "L%d: T=1\ngoto L%d\n", exp2_code->true_l, l);
                sprintf(comb_code+strlen(comb_code), "L%d: L%d: T=0\ngoto L%d\n", exp1_code->false_l, exp2_code->false_l, l+1);
            }
            else if (opr == '|')
            {
                sprintf(comb_code, "%sL%d: %s", exp1_code->value, exp1_code->false_l, exp2_code->value);
                sprintf(comb_code+strlen(comb_code), "L%d: L%d: T=1\ngoto L%d\n", exp1_code->true_l, exp2_code->true_l, l);
                sprintf(comb_code+strlen(comb_code), "L%d: T=0\ngoto L%d\n", exp2_code->false_l, l+1);
            }
            inter_code[t].true_l = l;
            inter_code[t].false_l = l+1;
            l += 2;

            push_stack(&stack, t);
            t++;
        }
    }
    char *final_code = inter_code[t-1].value;
    sprintf(final_code+strlen(final_code), "L%d: L%d: next code\n", l-2, l-1);

    // for (int i = 0; i < t; i++)
    // {
    //     printf("%s\n", inter_code[i].value);
    // }
    printf("%s", final_code);
}

void icg(char *infix)
{
    char *new_infix = preprocess(infix);
    printf("Infix: %s\n", new_infix);

    char *postfix = infix_to_postfix(new_infix);
    printf("Postfix: %s\n", postfix);

    printf("\nIntermediate code with short circuit evaluation:\n");
    three_address_code(postfix);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        // printf("Usage: %s <boolean-expression>\n", argv[0]);
        // exit(1);
        argv[1] = "a>b||c<d";
    }
    // argv[1] = "a>b||c<d";

    icg(argv[1]);

    return 0;
}