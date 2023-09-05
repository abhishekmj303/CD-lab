// Abhishek M J
// CS21B2018

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *str;
int i;

char get()
{
    return str[i];
}

void next()
{
    i++;
}

void accept()
{
    printf("String accepted\n");
    exit(0);
}

void reject()
{
    printf("String rejected\n");
    exit(1);
}

void S(); // S -> AB | BC
void A(); // A -> aA | ε
void B(); // B -> bB | ε
void C(); // C -> cC | ε

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <string>\n", argv[0]);
        return 1;
    }

    str = (char *)malloc(strlen(argv[1]) + 1);
    strcpy(str, argv[1]);
    // str = "aab";
    i = 0;

    S();
    if (get() != '\0')
        reject();
    accept();

    return 0;
}

void S()
{
    switch(get())
    {
        case 'a':
            next();
            A();
            B();
            break;
        case 'b':
            next();
            B();
            C();
            break;
        default:
            reject();
    }
}

void A()
{
    switch(get())
    {
        case 'a':
            next();
            A();
            break;
        case 'b':
            break;
        default:
            reject();
    }
}

void B()
{
    switch(get())
    {
        case 'b':
            next();
            B();
            break;
        case 'c': case '\0':
            break;
        default:
            reject();
    }
}

void C()
{
    switch(get())
    {
        case 'c':
            next();
            C();
            break;
        case '\0':
            break;
        default:
            reject();
    }
}