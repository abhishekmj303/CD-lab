#include <stdio.h>
#include <string.h>

void preprocess(FILE *fin, FILE *fout)
{
    int is_multi_line_comment = 0;
    int state = 0;
    char line[200];
    while (fgets(line, 200, fin) != NULL)
    {
        for (int i = 0; i < strlen(line); i++)
        {
            switch (state)
            {
                case 0: // default state
                    if (line[i] == '/') // initial '/'
                        state = 1;
                    else if (line[i] == '"') // start string
                        state = 2;
                    else if (line[i] == '#') // start preprocessor directive
                    {
                        state = 3;
                        line[i] = '\0'; // truncate the line from start of the preprocessor directive
                    }
                    else
                        state = 0;
                    break;
                case 1: // after initial '/'
                    if (line[i] == '/') // start single line comment
                    {
                        state = 3;
                        line[i-1] = '\0'; // truncate the line from start of the comment
                    }
                    else if (line[i] == '*') // start multi-line comment
                    {
                        state = 4;
                        line[i-1] = line[i] = ' '; // replace multi-line comment with spaces
                    }
                    else
                        state = 0;
                    break;
                case 2: // inside string
                    if (line[i] == '"') // end string
                        state = 0;
                    else
                        state = 2;
                    break;
                case 3: // final state to truncate the line
                    break;
                case 4: // inside multi-line comment
                    if (line[i] == '*') // possible end of multi-line comment
                        state = 5;
                    else
                    {
                        state = 4;
                    }
                    line[i] = ' '; // replace multi-line comment with spaces
                    break;
                case 5: // possible end of multi-line comment
                    if (line[i] == '/') // end of multi-line comment
                    {
                        state = 0;
                    }
                    else if (line[i] == '*') // possible end of multi-line comment
                        state = 5;
                    else
                        state = 4;
                    line[i] = ' '; // replace multi-line comment with spaces
                    break;
            }
            if (state == 3) // go to next line if single line comment
            {
                state = 0;
                break;
            }
        }
        fprintf(fout, "%s", line);
    }
}

int isWhiteSpace(char c)
{
    if (c == ' ' || c == '\t' || c == '\n')
        return 1;
    return 0;
}

int isIdentifier(char *token)
{}

int isKeyword(char *token)
{
    char *keywords[] = {"auto", "break", "case", "char", "const", "continue", "default", "do",
                        "double", "else", "enum", "extern", "float", "for", "goto", "if",
                        "int", "long", "register", "return", "short", "signed", "sizeof", "static",
                        "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"};
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (int i = 0; i < num_keywords; i++)
    {
        if (strcmp(token, keywords[i]) == 0)
            return 1;
    }
}

int isConstant(char *token)
{}

int isOperator(char *token)
{}

int isDelimiter(char c)
{
    if (c == ';' || c == ',' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}')
        return 1;
    return 0;
}

int isStringLiteral(char *token)
{}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <input_file>\n", argv[0]);
    }

    FILE *fin = fopen(argv[1], "r");
    if (fin == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }

    FILE *fout = fopen("inter.c", "w");

    preprocess(fin, fout);

    fclose(fin);
    fclose(fout);

    return 0;
}