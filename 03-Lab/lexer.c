#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STRING_SIZE 1000

void preprocess(FILE *fin, FILE *fout)
{
    int state = 0;
    char line[MAX_STRING_SIZE];
    while (fgets(line, MAX_STRING_SIZE, fin) != NULL)
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
                        line[i] = '\n'; // add newline character at the end of the preprocessor directive
                        line[i+1] = '\0'; // truncate the line from start of the preprocessor directive
                    }
                    else
                        state = 0;
                    break;
                case 1: // after initial '/'
                    if (line[i] == '/') // start single line comment
                    {
                        state = 3;
                        line[i-1] = '\n'; // add newline character at the end of the comment
                        line[i] = '\0'; // truncate the line from start of the comment
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
                        state = 4;
                    if (line[i] != '\n')
                        line[i] = ' '; // replace multi-line comment with spaces
                    break;
                case 5: // possible end of multi-line comment
                    if (line[i] == '/') // end of multi-line comment
                        state = 0;
                    else if (line[i] == '*') // possible end of multi-line comment
                        state = 5;
                    else
                        state = 4;
                    if (line[i] != '\n')
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

int isWhiteSpace(const char c)
{
    if (c == ' ' || c == '\t' || c == '\n')
        return 1;
    return 0;
}

int isKeyword(const char *token)
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
    return 0;
}

int endIdentifier(const char *token)
{
    int state = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        switch (state)
        {
            case 0: // initial state
                if (isalpha(token[i]) || token[i] == '_')
                    state = 1;
                else
                    state = -1;
                break;
            case 1: // after first character
                if (isalpha(token[i]) || token[i] == '_' || isdigit(token[i]))
                    state = 1;
                else
                    state = 2;
                break;
            case 2: // final state
                break;
        }

        if (state == 2)
            return i;
        else if (state == -1)
            return 0;
    }
}

int isDigit(const char c)
{
    if (c >= '0' && c <= '9')
        return 1;
    return 0;
}

int isHexdigit(const char c)
{
    if (isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        return 1;
    return 0;
}

int isOctdigit(const char c)
{
    if (c >= '0' && c <= '7')
        return 1;
    return 0;
}

int isIntEnd(const char c)
{
    if (c == 'u' || c == 'U' || c == 'l' || c == 'L')
        return 1;
    return 0;
}

int isHexOctConstant(const char *token)
{
    int state = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        switch (state)
        {
            case 0: // initial state
                if (token[i] == '0')
                    state = 1;
                else
                    state = 2;
                break;
            case 1: // after first character
                if (token[i] == 'x' || token[i] == 'X')
                    state = 3;
                else if (isOctdigit(token[i]))
                    state = 4;
                else
                    state = 2;
                break;
            case 2: // not a hex/octal constant
                break;
            case 3: // after '0x' or '0X'
                if (isHexdigit(token[i]))
                    state = 5;
                else
                    state = 2;
                break;
            case 4: // after '0'
                if (isOctdigit(token[i]))
                    state = 6;
                else
                    state = 2;
                break;
            case 5: // inside hex digits
                if (isHexdigit(token[i]))
                    state = 5;
                else if (isIntEnd(token[i]))
                {
                    state = 7;
                    i++;
                }
                else
                    state = 7;
                break;
            case 6: // inside octal digits
                if (isOctdigit(token[i]))
                    state = 6;
                else if (isIntEnd(token[i]))
                {
                    state = 7;
                    i++;
                }
                else
                    state = 7;
                break;
            case 7: // after hex/octal digits
                break;
        }
        if (state == 2)
            return 0;
        else if (state == 7)
            return ++i;
    }
}

int isIntegerConstant(const char *token)
{
    int state = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        switch (state)
        {
            case 0: // initial state
                if (isDigit(token[i]))
                    state = 1;
                else
                    state = 2;
                break;
            case 1: // after first character
                if (isDigit(token[i]))
                    state = 1;
                else if (isIntEnd(token[i]))
                {
                    state = 3;
                    i++;
                }
                else
                    state = 3;
                break;
            case 2: // not an integer constant
                break;
            case 3: // after digits
                break;
        }
        if (state == 2)
            return 0;
        else if (state == 3)
            return i;
    }
}

int isFloatEnd(const char c)
{
    if (c == 'f' || c == 'F' || c == 'l' || c == 'L')
        return 1;
    return 0;
}

int isRealConstant(const char *token)
{
    int state = 0;
    for (int i = 0; i < strlen(token); i++)
    {
        switch (state)
        {
            case 0: // initial state
                if (isDigit(token[i]))
                    state = 1;
                else if (token[i] == '.')
                    state = 2;
                else
                    state = -1;
                break;
            case 1: // before point
                if (isDigit(token[i]))
                    state = 1;
                else if (token[i] == '.')
                    state = 2;
                else if (token[i] == 'e' || token[i] == 'E')
                    state = 3;
                else
                    state = -1;
                break;
            case 2: // after point
                if (isDigit(token[i]))
                    state = 2;
                else if (token[i] == 'e' || token[i] == 'E')
                    state = 3;
                else
                    state = 4;
                break;
            case 3: // after 'e' or 'E'
                if (isDigit(token[i]))
                    state = 6;
                else if (token[i] == '+' || token[i] == '-')
                    state = 5;
                else
                    state = -1;
                break;
            case 4: // final state
                break;
            case 5: // after '+' or '-'
                if (isDigit(token[i]))
                    state = 6;
                else
                    state = -1;
                break;
            case 6: // after digits
                if (isDigit(token[i]))
                    state = 6;
                else if (isFloatEnd(token[i]))
                {
                    state = 4;
                    i++;
                }
                else
                    state = 4;
                break;
        }
        if (state == -1)
            return 0;
        else if (state == 4)
            return i;
    }
}

int endConstant(const char *token)
{
    int end = isHexOctConstant(token);
    if (end)
        return end;
    end = isRealConstant(token);
    if (end)
        return end;
    end = isIntegerConstant(token);
    if (end)
        return end;
    return 0;
}

int isOperator(const char *token)
{
    char *operators[] = {"+", "-", "*", "/", "%", "++", "--", "==", "!=", ">", "<", ">=", "<=", "&&", "||", "!", "&", "|", "^", "~", "<<", ">>", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|=", "->", ".", ":", "?"};
    int num_operators = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < num_operators; i++)
    {
        if (strcmp(token, operators[i]) == 0)
            return 1;
    }
    return 0;
}

int isDelimiter(const char c)
{
    char *delimiters = "[]{}(),;:";
    int num_delimiters = strlen(delimiters);
    for (int i = 0; i < num_delimiters; i++)
    {
        if (c == delimiters[i])
            return 1;
    }
    return 0;
}

void lexicalAnalysis(FILE *fin, FILE *fout)
{
    int indentifier_count = 0, keyword_count = 0, constant_count = 0, operator_count = 0, spchar_count = 0, string_count = 0;
    int line_no = 1;
    char line[MAX_STRING_SIZE];
    while (fgets(line, MAX_STRING_SIZE, fin) != NULL)
    {
        int state = 0;
        int i = 0;
        char token[MAX_STRING_SIZE];
        int token_index = 0;
        while (i < strlen(line))
        {
            switch(state)
            {
                case 0:
                    if (line[i] == '"') // start string
                    {
                        state = 1;
                    }
                    else if (isDelimiter(line[i])) // special character
                    {
                        state = 2;
                    }
                    else if (isDigit(line[i])) // constant
                    {
                        state = 3;
                    }
                    else if (isalpha(line[i]) || line[i] == '_') // identifier or keyword
                    {
                        state = 4;
                    }
                    else if (isWhiteSpace(line[i])) // whitespace
                    {
                        i++;
                        state = 0;
                    }
                    else // operator
                    {
                        state = 5;
                    }
                    break;
                case 1: // inside string
                    token[token_index++] = line[i++];
                    while (line[i] != '"')
                        token[token_index++] = line[i++];
                    token[token_index++] = line[i++];
                    token[token_index] = '\0';
                    fprintf(fout, "Line %d: %s <-> string\n", line_no, token);
                    string_count++;
                    token_index = 0;
                    state = 0;
                    break;
                case 2: // special character
                    token[token_index++] = line[i++];
                    token[token_index] = '\0';
                    fprintf(fout, "Line %d: %s <-> special character\n", line_no, token);
                    spchar_count++;
                    token_index = 0;
                    state = 0;
                    break;
                case 3: // constant
                    int end_const = endConstant(line+i);
                    if (end_const)
                    {
                        strncpy(token, line+i, end_const);
                        token[end_const] = '\0';
                        fprintf(fout, "Line %d: %s <-> constant\n", line_no, token);
                        constant_count++;
                        i += end_const;
                        token_index = 0;
                        state = 0;
                    }
                    else
                    {
                        token[token_index++] = line[i++];
                        state = 3;
                    }
                    break;
                case 4: // identifier or keyword
                    int end_id = endIdentifier(line+i);
                    if (end_id)
                    {
                        strncpy(token, line+i, end_id);
                        token[end_id] = '\0';
                        if (isKeyword(token))
                        {
                            fprintf(fout, "Line %d: %s <-> keyword\n", line_no, token);
                            keyword_count++;
                        }
                        else
                        {
                            fprintf(fout, "Line %d: %s <-> identifier\n", line_no, token);
                            indentifier_count++;
                        }
                        i += end_id;
                        token_index = 0;
                        state = 0;
                    }
                    else
                    {
                        token[token_index++] = line[i++];
                        state = 4;
                    }
                    break;
                case 5: // operator
                    while (!isWhiteSpace(line[i]) && !isDelimiter(line[i]) && !isalpha(line[i]) && !isDigit(line[i]))
                        token[token_index++] = line[i++];
                    token[token_index] = '\0';
                    if (isOperator(token))
                    {
                        fprintf(fout, "Line %d: %s <-> operator\n", line_no, token);
                        operator_count++;
                    }
                    else
                    {
                        for (int j = 0; j < strlen(token); j++) // for ** operator
                        {
                            char temp[2];
                            temp[0] = token[j];
                            temp[1] = '\0';
                            if (isOperator(temp))
                            {
                                fprintf(fout, "Line %d: %s <-> operator\n", line_no, temp);
                                operator_count++;
                            }
                            else
                                fprintf(fout, "Line %d: %s <-> Invalid\n", line_no, temp);
                        }
                    }
                    token_index = 0;
                    state = 0;
                    break;
            }
        }
        line_no++;
    }
    printf("Number of identifiers: %d\n", indentifier_count);
    printf("Number of keywords: %d\n", keyword_count);
    printf("Number of constants: %d\n", constant_count);
    printf("Number of operators: %d\n", operator_count);
    printf("Number of special characters: %d\n", spchar_count);
    printf("Number of strings: %d\n", string_count);
    printf("Total: %d\n", indentifier_count + keyword_count + constant_count + operator_count + spchar_count + string_count);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        // printf("Usage: %s <input_file>\n", argv[0]);
        // return 1;
        argv[1] = "/home/abhishek/Programs/CD/02-Lab/input.c";
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

    fin = fopen("inter.c", "r");
    fout = fopen("output.txt", "w");

    lexicalAnalysis(fin, fout);

    fclose(fin);
    fclose(fout);

    return 0;
}