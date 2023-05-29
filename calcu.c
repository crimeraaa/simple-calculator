#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* const int NUM_ARGS
argv[0] is the executable
argv[1] is some number
argv[2] is your operand
argv[3] is the other number 
    
This will just set how many arguments the program needs to function. 
*/ 

#define NUM_ARGS 4

/* 
const char *DECMIALS_FMT = some-format-string

'%%' to escape the '%' char
'.' is the decimal point, but is unaffected right now. 
    It will be formatted later on into '.(integer)'...
    ...to represent how many decimals we'll print in printf.
'%i' is the format specifer
'f' has no '%' so we don't need to escape it 

_FMT constants will be used in sprintf to act as the format string.

================================================================

const size_t DECIMALS_BUFF = sizeof-whole-array / sizeof-first-element

This is just a way to compute the amount of elements without using strlen.
Why? Because you can't call such a function on globals at compile time!

They'll be used to specify a buffer length for sprintf later on.
*/ 

#define DECIMALS_FMT "%%.%if"
#define DECIMALS_BUFF sizeof(DECIMALS_FMT) / sizeof(DECIMALS_FMT[0])

/* See above explanation. */
#define EQUATION_FMT "%s %%c %s = %s\n"
#define EQUATION_BUFF sizeof(EQUATION_FMT) / sizeof(EQUATION_FMT[0])

/* 
If only got 2 command-line args, then check against these.
It will print out specific instructions on how to use the program. 
*/

const char *HELP_COMMANDS[] = {"--help", "--h", "help", "h"};
#define NUM_OF_HELP_COMMANDS sizeof(HELP_COMMANDS) / sizeof(HELP_COMMANDS[0])

struct RealNumber
{
    /* 
    Members starting with 'is_' are always bools.
    Init to false on construction, but set to true if right conditions are met. 
    */ 
    bool is_negative;
    bool is_decimal;
    /* Set this to NULL on error, then check against it later on. */ 
    bool *is_valid_string;

    /* Init this to 0. */ 
    int integer;

    /* Init these to 0 or 0.0 if is_decimal is true. */
    int num_decimal_places;
    double decimal;
    double decimal_places;
};

/* 
The way I use forward declarations from here on, 
they are for functions called within the nearest 'major' function.

e.g. forward declare `EvaluateNums` and `StringToReal` to be used in `main` 
*/

void EvaluateNums(struct RealNumber *real1, char operand, struct RealNumber *real2);
struct RealNumber StringToReal(char *src);
int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        for (int i = 0; i < NUM_OF_HELP_COMMANDS; i++)
        {
            int got_help = strcmp(argv[1], HELP_COMMANDS[i]);
            /* 
            strcmp returns 0 on success
            (that is, both input strings have equal chars) 
            */ 
            if (got_help == 0)
            {
                printf("USAGE\n\t./nums num1 operand num2\n");
                printf("OPERATORS\n\t+ - x / %% ^\n");
                printf("NUMBERS\n\tAny real integer or double-precision float.\n");
                return 0;
            }
        }
        printf("Unknown command '%s'\n", argv[1]);
        return 1;
    }
    else if (argc != NUM_ARGS)
    {
        printf("Perform basic math on 2 real numbers!\n");
        printf("Try ./calcu --help (or --h) for more information.\n");

        /* Use '%zu' as size_t may not always be unsigned long on all platforms. */
        // printf("DECIMALS\n\t_FMT: %s\n\t_BUFF: %zu\n", DECIMALS_FMT, DECIMALS_BUFF);
        // printf("EQUATION\n\t_FMT: %s\n\t_BUFF: %zu\n", EQUATION_FMT, EQUATION_BUFF);
        return 0;
    }

    struct RealNumber num1 = StringToReal(argv[1]);
    char operand = argv[2][0];
    struct RealNumber num2 = StringToReal(argv[3]);

    if (num1.is_valid_string == NULL || num2.is_valid_string == NULL)
    {
        // printf("Invalid command-line inputs!\n");
        return 1;
    }

    EvaluateNums(&num1, operand, &num2);
    return 0;
}

bool IsSymbol(char indiv, struct RealNumber *struct_ptr);
void WriteValues(int val, struct RealNumber *struct_ptr);
struct RealNumber StringToReal(char *src)
{
    /* Init the return value that we'll mess around with. */ 
    bool valid = true;
    struct RealNumber ret = {
        /* Init to false, set to true if we encounter to right conditions */ 
        .is_negative = false,
        .is_decimal = false,
        /* Need to init this pointer to something, else we segfault */ 
        .is_valid_string = &valid,

        .integer = 0,
    };

    for (int i = 0; src[i] != '\0'; i++)
    {
        if (IsSymbol(src[i], &ret))
        {
            /* 
            Jump back to start of loop so we don't perform math...
            ...on the ascii value of this character. 
            */
            continue;
        }
        else if (src[i] < '0' || src[i] > '9')
        /* Set the is_valid_string ptr to NULL so we can error check. */ 
        {
            ret.is_valid_string = NULL;
        }

        if (ret.is_valid_string == NULL)
        {
            printf("Invalid input string '%s'\n", src);
            return ret;
        }

        WriteValues(src[i] - '0', &ret);
    }

    if (ret.is_negative)
    {
        ret.integer *= -1;
        if (ret.is_decimal)
        {
            ret.decimal *= -1.0;
        }
    }

    return ret;
}
 
bool IsSymbol(char indiv, struct RealNumber *struct_ptr)
{
    if (indiv == '+')
    {
        struct_ptr->is_negative = false;
        return true;
    }
    else if (indiv == '-')
    {
        /* Flip in case we encounter multiple unary negations */ 
        struct_ptr->is_negative = !struct_ptr->is_negative;
        return true;
    }
    else if (indiv == '.')
    {
        /* 
        If we got multiple decimal points, we got a syntax error
        That is, we set .is_decimal to true beforehand...
        ...and encountered another decimal point! 
        */ 
        if (struct_ptr->is_decimal)
        {
            struct_ptr->is_valid_string = NULL;
            return false;
        }

        struct_ptr->is_decimal = true;
        struct_ptr->decimal = 0.0;
        struct_ptr->decimal_places = 0.1;
        struct_ptr->num_decimal_places = 0;
        return true;
    }
    return false;
}

void WriteValues(int val, struct RealNumber *struct_ptr)
{
    /* For the whole number section. */ 
    if (!struct_ptr->is_decimal)
    {
        struct_ptr->integer = (struct_ptr->integer * 10) + val;
    }
    else // if (struct_ptr->is_decimal)
    {
        struct_ptr->decimal += struct_ptr->decimal_places * val;
        struct_ptr->decimal_places /= 10;
        struct_ptr->num_decimal_places++;
    }
}

void ComputeInts(int num1, char operand, int num2);
void ComputeDoubles(double num1, int decimals1, char operand, double num2, int decimals2);
void EvaluateNums(struct RealNumber *real1, char operand, struct RealNumber *real2)
{
    if (!real1->is_decimal && !real2->is_decimal)
    {
        int num1 = real1->integer;
        int num2 = real2->integer;
        ComputeInts(num1, operand, num2);
        return;
    }
    /* If at least 1 is a decimal, convert them to doubles even if the other is an int */ 
    double num1 = real1->integer + real1->decimal;
    int decimals1 = real1->num_decimal_places;

    double num2 = real2->integer + real2->decimal;
    int decimals2 = real2->num_decimal_places;
    ComputeDoubles(num1, decimals1, operand, num2, decimals2);
}

void ComputeInts(int num1, char operand, int num2)
{
    int ans = 0;
    switch (operand)
    {
    case '+':
        ans = num1 + num2;
        break;
    case '-':
        ans = num1 - num2;
        break;
    /* Use 'x' as commandline mistakes '*' for a pointer */ 
    case 'x':
        ans = num1 * num2;
        break;
    case '/':
        /* Due to the nature of division, it's usually better to treat them as doubles */ 
        printf("%i %c %i = %f\n", num1, operand, num2, (double) num1 / num2);
        return;
    case '%':
        ans = num1 % num2;
        break;
    case '^':
        /* pow returns doubles, so cast to int if both inputs were ints to begin with */ 
        ans = (int) pow(num1, num2);
        break;
    default:
        printf("Invalid operator '%c'\n", operand);
        return;
    }
    // printf("%i %c %i = %i\n", num1, operand, num2, ans);

    char main_fmt[EQUATION_BUFF];
    sprintf(main_fmt, EQUATION_FMT, "%i", "%i", "%i");
    printf(main_fmt, num1, operand, num2, ans);
}

char *PrecisionString(int decimal_places);
void ComputeDoubles(double num1, int decimals1, char operand, double num2, int decimals2)
{
    double ans = 0.0;
    switch (operand)
    {
    case '+':
        ans = num1 + num2;
        break;
    case '-':
        ans = num1 - num2;
        break;
    /* Use 'x' as command-line thinks the char '*' is a pointer or something */ 
    case 'x':
        ans = num1 * num2;
        break;
    case '/':
        ans = num1 / num2;
        break;
    case '%':
        /* Modulo with floats/doubles is kinda weird due to the nature of remainders being whole */
        ans = (int) num1 % (int) num2;
        break;
    case '^':
        ans = pow(num1, num2);
        break;
    default:
        printf("Invalid operator '%c'\n", operand);
        return;
    }
    // printf("%f %c %f = %f\n", num1, operand, num2, ans);

    char main_fmt[EQUATION_BUFF];
    char *fmt1 = PrecisionString(decimals1);
    char *fmt2 = PrecisionString(decimals2);

    sprintf(main_fmt, EQUATION_FMT, fmt1, fmt2, "%f");
    printf(main_fmt, num1, operand, num2, ans);

    free(fmt1);
    free(fmt2);
}

/* This returns a malloc'd pointer. Remember to free it! */
char *PrecisionString(int decimal_places)
{
    /* 
    Can't use `static` as we need variable amounts of decimal places. 
    So malloc DECIMALS_BUFF and use a pointer instead.
    */ 
    char *fmt_length = malloc(DECIMALS_BUFF * sizeof(char));

    /*
    To format a string, use sprintf:
    static inline int sprintf(char *__stream, const char *__format, ...)
        
    char *__stream = output string, must be buffered already.
    const char *__format = format string, see DECIMALS_FMT.
    ... = everything after __format will be formatted into __format.
    */ 
    sprintf(fmt_length, DECIMALS_FMT, decimal_places);
    return fmt_length;
}