#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100
#define ID_LEN 30

/* Keywords */
char *keywords[] = {
    "auto", "break", "case", "char", "const",
    "continue", "default", "do", "double",
    "else", "enum", "extern", "float", "for",
    "goto", "if", "int", "long", "register",
    "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile",
    "while"
};

int keywordCount = 32;

/* Check keyword */
int isKeyword(char word[])
{
    int i;

    for (i = 0; i < keywordCount; i++)
    {
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    }

    return 0;
}

/* Check operator */
int isOperator(char ch)
{
    return ch == '+' || ch == '-' ||
           ch == '*' || ch == '/' ||
           ch == '%' || ch == '=' ||
           ch == '<' || ch == '>' ||
           ch == '!' || ch == '&' ||
           ch == '|';
}

/* Check separator */
int isSeparator(char ch)
{
    return ch == '(' || ch == ')' ||
           ch == '{' || ch == '}' ||
           ch == '[' || ch == ']' ||
           ch == ',' || ch == ';' ||
           ch == ':';
}

int main()
{
    FILE *fp;
    char ch, next;
    char token[MAX];
    int i;

    fp = fopen("input.txt", "r");

    if (fp == NULL)
    {
        printf("Error: input.txt not found.\n");
        return 1;
    }

    printf("\nLEXICAL ANALYSIS\n");
    printf("============================================\n");
    printf("%-25s %s\n", "TOKEN TYPE", "LEXEME");
    printf("============================================\n");

    while ((ch = fgetc(fp)) != EOF)
    {
        /* ----------------------------------------
           1. Ignore spaces, tabs and newlines
           ---------------------------------------- */
        if (isspace(ch))
            continue;


        /* ----------------------------------------
           2. Identifier or Keyword
           ---------------------------------------- */
        if (isalpha(ch) || ch == '_')
        {
            i = 0;

            while (isalnum(ch) || ch == '_')
            {
                if (i < MAX - 1)
                    token[i++] = ch;

                ch = fgetc(fp);
            }

            token[i] = '\0';

            if (ch != EOF)
                ungetc(ch, fp);

            if (i > ID_LEN)
                printf("%-25s %s\n",
                       "INVALID IDENTIFIER", token);

            else if (isKeyword(token))
                printf("%-25s %s\n",
                       "KEYWORD", token);

            else
                printf("%-25s %s\n",
                       "IDENTIFIER", token);

            continue;
        }


        /* ----------------------------------------
           3. Number / Constant
           ---------------------------------------- */
        if (isdigit(ch))
        {
            i = 0;

            while (isdigit(ch))
            {
                token[i++] = ch;
                ch = fgetc(fp);
            }

            /* Decimal number */
            if (ch == '.')
            {
                token[i++] = ch;
                ch = fgetc(fp);

                while (isdigit(ch))
                {
                    token[i++] = ch;
                    ch = fgetc(fp);
                }
            }

            token[i] = '\0';

            if (ch != EOF)
                ungetc(ch, fp);

            printf("%-25s %s\n",
                   "CONSTANT", token);

            continue;
        }


        /* ----------------------------------------
           4. Comments / Division Operator
           ---------------------------------------- */
        if (ch == '/')
        {
            next = fgetc(fp);

            /* Single-line comment */
            if (next == '/')
            {
                while ((ch = fgetc(fp)) != EOF &&
                       ch != '\n')
                {
                    /* Ignore comment */
                }

                continue;
            }

            /* Multi-line comment */
            else if (next == '*')
            {
                char previous = '\0';

                while ((ch = fgetc(fp)) != EOF)
                {
                    if (previous == '*' && ch == '/')
                        break;

                    previous = ch;
                }

                continue;
            }

            /* Division operator */
            else
            {
                if (next != EOF)
                    ungetc(next, fp);

                printf("%-25s /\n",
                       "ARITHMETIC OPERATOR");

                continue;
            }
        }


        /* ----------------------------------------
           5. String Literal
           ---------------------------------------- */
        if (ch == '"')
        {
            i = 0;
            token[i++] = ch;

            while ((ch = fgetc(fp)) != EOF)
            {
                token[i++] = ch;

                if (ch == '"')
                    break;

                if (i >= MAX - 1)
                    break;
            }

            token[i] = '\0';

            printf("%-25s %s\n",
                   "STRING", token);

            continue;
        }


        /* ----------------------------------------
           6. Character Literal
           ---------------------------------------- */
        if (ch == '\'')
        {
            i = 0;
            token[i++] = ch;

            ch = fgetc(fp);

            if (ch != EOF)
                token[i++] = ch;

            ch = fgetc(fp);

            if (ch == '\'')
                token[i++] = ch;

            token[i] = '\0';

            printf("%-25s %s\n",
                   "CHARACTER", token);

            continue;
        }


        /* ----------------------------------------
           7. Two-character operators
           ---------------------------------------- */
        if (ch == '=' || ch == '<' ||
            ch == '>' || ch == '!')
        {
            next = fgetc(fp);

            if (next == '=')
            {
                printf("%-25s %c%c\n",
                       "RELATIONAL OPERATOR",
                       ch, next);
            }
            else
            {
                if (next != EOF)
                    ungetc(next, fp);

                if (ch == '=')
                    printf("%-25s %c\n",
                           "ASSIGNMENT OPERATOR", ch);
                else
                    printf("%-25s %c\n",
                           "RELATIONAL OPERATOR", ch);
            }

            continue;
        }


        /* ----------------------------------------
           8. +, -, *, %, &&, ||
           ---------------------------------------- */
        if (ch == '+' || ch == '-' ||
            ch == '*' || ch == '%')
        {
            next = fgetc(fp);

            if (next == ch || next == '=')
            {
                printf("%-25s %c%c\n",
                       "ARITHMETIC OPERATOR",
                       ch, next);
            }
            else
            {
                if (next != EOF)
                    ungetc(next, fp);

                printf("%-25s %c\n",
                       "ARITHMETIC OPERATOR", ch);
            }

            continue;
        }


        /* Logical operators */
        if (ch == '&' || ch == '|')
        {
            next = fgetc(fp);

            if (next == ch)
            {
                printf("%-25s %c%c\n",
                       "LOGICAL OPERATOR",
                       ch, next);
            }
            else
            {
                if (next != EOF)
                    ungetc(next, fp);

                printf("%-25s %c\n",
                       "INVALID OPERATOR", ch);
            }

            continue;
        }


        /* ----------------------------------------
           9. Separators
           ---------------------------------------- */
        if (isSeparator(ch))
        {
            printf("%-25s %c\n",
                   "SEPARATOR", ch);

            continue;
        }


        /* ----------------------------------------
           10. Special / Invalid symbol
           ---------------------------------------- */
        printf("%-25s %c\n",
               "SPECIAL / INVALID SYMBOL", ch);
    }

    printf("============================================\n");

    fclose(fp);

    return 0;
}














#include <stdio.h>

int main()
{
    int a = 10;
    float b = 20.5;

    // This is a comment

    if (a < b)
    {
        a++;
        printf("Hello");
    }

    /* Multi-line
       comment */

    return 0;
}
 
