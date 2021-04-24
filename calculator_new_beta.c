#include "stdio.h"
#include "string.h"
#include "stdlib.h"

char* format_formula(char* formula);
char* get_postfix_formula(char *formula);
double calculate(char *formula);
char* remove_element_by_index(char* formula, int index);
int is_number(char c);
int is_operator(char c);

int main()
{
    char* formula = "-18*4.5-(10+12/(-3))";
//    char* formula = "8*4-(10+12/3)";
//    char* formula = "-18*4.5+5";
//    char* formula = "5+5";
    // printf("%s", remove_element_by_index(formula, strlen(formula) - 1));

    printf("%s\n", formula);

    formula = format_formula(formula);

    printf("%s\n", formula);

    formula = get_postfix_formula(formula);

    printf("%s\n", formula);

    printf("%f\n", calculate(formula));

    return 0;
}

char* remove_element_by_index(char* formula, int index)
{
    int i;
    int idex = 0;
    int len = strlen(formula);
    char* buf = (char*) malloc(sizeof(char) * len);
    memset(buf, 0, len);
    for(i = 0; i < len; i++)
    {
        if (i != index)
        {
            buf[idex++] = formula[i];
        }
    }
    return buf;
}

int is_number(char c)
{
    return c >= 48 && c <= 57;
}

int is_operator(char c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

char* format_formula(char* formula)
{
    int i;
    int formula_len = strlen(formula);
    char* buf = (char*) malloc(sizeof(char) * formula_len * 2);
    memset(buf, 0, formula_len * 2);
    for (i = 0; i < formula_len; i++)
    {
        if(formula[i] == ' ')
        {
            continue;
        }
        if (i == 0 && formula[i] == '-')
        {
            strcat(buf, "0");
        }
        if (formula[i] == '-' && formula[i - 1] == '(')
        {
            strcat(buf, "0-");
        }
        else
        {
            buf[strlen(buf)] = formula[i];
        }
    }
    // Add char "_" in the end of number, mark the end of number.
    // 5_+ 5_- 5_/ 5_* 5_) 5_
    int buf_len = strlen(buf);
    char* new_buf = (char*) malloc(sizeof(char) * buf_len * 2);
    memset(new_buf, 0, buf_len * 2);
    int new_buf_index = 0;
    for (i = 0; i < buf_len; i++)
    {
        new_buf[new_buf_index++] = buf[i];
        if (is_number(buf[i]) && (i + 1 < buf_len) && is_operator(buf[i + 1]))
        {
            new_buf[new_buf_index++] = '_';
        }
        else if (is_number(buf[i]) && (i + 1 < buf_len) && buf[i + 1] == ')')
        {
            new_buf[new_buf_index++] = '_';
        }
        else if (is_number(buf[i]) && (i + 1 == buf_len))
        {
            new_buf[new_buf_index++] = '_';
        }
    }
    return new_buf;
}

char* get_postfix_formula(char* formula)
{
    int i;
    int formula_len = strlen(formula);
    char* stack = (char*) malloc(sizeof(char) * formula_len);
    memset(stack, 0, formula_len);
    char* buf = (char*) malloc(sizeof(char) * formula_len);
    memset(buf, 0, formula_len);
    double number = 0;
    int decimals = 0;
    for (i = 0; i < formula_len; i++)
    {
        // handle number
        if (is_number(formula[i]))
        {
            if (decimals == 0) {
                number = number * 10 + formula[i] - '0';
            }
            else if (decimals > 0)
            {
                number = number + (formula[i] - '0') / (decimals * 10 * 1.0);
                decimals++;
            }
            continue;
        }
        // handle decimal point
        if (formula[i] == '.')
        {
            decimals = 1;
            continue;
        }
        // Assign number to buf if meet the char of number end.
        if (formula[i] == '_')
        {
            sprintf(buf + strlen(buf), "%g", number);
            sprintf(buf + strlen(buf), "%c", formula[i]);
            number = 0;
            decimals = 0;
            continue;
        }
        // handle operator '+' and '-'
        if (formula[i] == '+' || formula[i] == '-')
        {
            while (strlen(stack) > 0 && is_operator(stack[strlen(stack) - 1]))
            {
                char stack_top = stack[strlen(stack) - 1];
                sprintf(buf + strlen(buf), "%c", stack_top);
                stack = remove_element_by_index(stack, strlen(stack) - 1);
            }
            sprintf(stack + strlen(stack), "%c", formula[i]);
            continue;
        }
        // handle operator '*' and '/'
        if (formula[i] == '*' || formula[i] == '/')
        {
            while (strlen(stack) > 0 &&
                   (stack[strlen(stack) - 1] == '*' || stack[strlen(stack) - 1] == '/'))
            {
                char stack_top = stack[strlen(stack) - 1];
                sprintf(buf + strlen(buf), "%c", stack_top);
                stack = remove_element_by_index(stack, strlen(stack) - 1);
            }
            sprintf(stack + strlen(stack), "%c", formula[i]);
            continue;
        }
        // handle '('
        if (formula[i] == '(')
        {
            sprintf(stack + strlen(stack), "%c", formula[i]);
            continue;
        }
        // handle ')'
        if (formula[i] == ')')
        {
            while (strlen(stack) > 0 && stack[strlen(stack) - 1] != '(')
            {
                char stack_top = stack[strlen(stack) - 1];
                sprintf(buf + strlen(buf), "%c", stack_top);
                stack = remove_element_by_index(stack, strlen(stack) - 1);
            }
            stack = remove_element_by_index(stack, strlen(stack) - 1);
            continue;
        }
    }
    // Clear stack
    for (i = strlen(stack) - 1; i >= 0; i--) {
        sprintf(buf + strlen(buf), "%c", stack[i]);
    }
    return buf;
}

double calculate(char* formula)
{
    int formula_len = strlen(formula);
    double* stack = (double*) malloc(sizeof(double) * formula_len * 2);
    int stack_count = 0;
    double number = 0;
    int decimals = 0;
    int i;
    for (i = 0; i < formula_len; ++i) {
        if (is_number(formula[i]))
        {
            if (decimals == 0) {
                number = number * 10 + formula[i] - '0';
            }
            else if (decimals > 0)
            {
                number = number + (formula[i] - '0') / (decimals * 10 * 1.0);
                decimals++;
            }
            continue;
        }
        if (formula[i] == '.')
        {
            decimals = 1;
            continue;
        }
        if (formula[i] == '_')
        {
            stack[stack_count++] = number;
            number = 0;
            decimals = 0;
            continue;
        }
        if (is_operator(formula[i]))
        {
            double n1 = stack[stack_count - 1];
            double n2 = stack[stack_count - 2];
            double result = 0;
            switch (formula[i]) {
                case '+': result += n2 + n1; break;
                case '-': result += n2 - n1; break;
                case '*': result += n2 * n1; break;
                case '/': result += n2 / n1; break;
            }
            stack[stack_count - 1] = 0;
            stack[stack_count - 2] = 0;
            stack_count -= 2;
            stack[stack_count++] = result;
        }
    }
    return stack[0];
}
