#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1
#define false 0

// argv[0] is the executable, argv[1] is some number, argv[2] is your operand, argv[3] is the other number
#define NUM_ARGS 4

// %% = escape the '%' char, %i is the format specifer, 'f' has no '%' so we don't need to escape it
#define DECIMALS_FMT "%%.%if"
#define EQUATION_FMT "%s %%c %s = %s\n"

const char *HELP_COMMANDS[] = {"--help", "--h", "help", "h"};
#define NUM_OF_HELP_COMMANDS sizeof(HELP_COMMANDS) / sizeof(HELP_COMMANDS[0])

struct RealNumber
{
    // init as needed
    int is_negative;
    int is_decimal;

    int *is_valid_string;

    // init to 0
    int integer;
    double decimal;
    double decimal_places;
    int num_decimal_places;
};

// Forward declaration of the functions called within main.
void EvaluateNums(struct RealNumber *real1, char operand, struct RealNumber *real2);
struct RealNumber StringToReal(char *src);

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        for (int i = 0; i < NUM_OF_HELP_COMMANDS; i++)
        {
            int got_help = strcmp(argv[1], HELP_COMMANDS[i]);
            // strcmp returns 0 on success (that is, both input strings have equal chars)
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

// Forward declaration of the functions called within StringToReal.
int IsSymbol(char indiv, struct RealNumber *struct_ptr);
void WriteValues(int val, struct RealNumber *struct_ptr);

struct RealNumber StringToReal(char *src)
{
    // Init the return value that we'll mess around with.
    int valid = true;
    struct RealNumber ret = {
        .is_negative = false,
        .is_decimal = false,
        // Need to init this pointer to something, else we segfault
        .is_valid_string = &valid,

        .integer = 0,
    };

    for (int i = 0; src[i] != '\0'; i++)
    {
        if (IsSymbol(src[i], &ret))
        {
            // Jump back to start of loop so we don't perform math
            // on the ascii value of this.
            continue;
        }
        else if (src[i] < '0' || src[i] > '9')
        // Set the is_valid_string ptr to NULL so we can error check.
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

// Pass the pointer to struct_ptr so we can modify it directly
int IsSymbol(char indiv, struct RealNumber *struct_ptr)
{
    // Just in case user explicitly types the '+' sign on a number.
    if (indiv == '+')
    {
        struct_ptr->is_negative = false;
        return true;
    }
    else if (indiv == '-')
    {
        // Flip in case we encounter multiple unary negations
        struct_ptr->is_negative = !struct_ptr->is_negative;
        return true;
    }
    else if (indiv == '.')
    {
        // If we got multiple decimal points, we got a syntax error
        // That is, we set .is_decimal to true beforehand and encountered another decimal point!
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
    // For the whole number section.
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

// Forward declaration of the functions called within EvaluateNums.
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
    // If at least 1 is a decimal, convert them to doubles even if the other is an int
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
    // Use 'x' as commandline mistakes '*' for a pointer
    case 'x':
        ans = num1 * num2;
        break;
    case '/':
        // Due to the nature of division, it's usually better to treat them as doubles
        printf("%i %c %i = %f\n", num1, operand, num2, (double) num1 / num2);
        return;
    case '%':
        ans = num1 % num2;
        break;
    case '^':
        // pow returns doubles, so cast to int if both inputs were ints to begin with
        ans = (int) pow(num1, num2);
        break;
    default:
        printf("Invalid operator '%c'\n", operand);
        return;
    }
    // printf("%i %c %i = %i\n", num1, operand, num2, ans);

    char main_fmt[strlen(EQUATION_FMT)];
    sprintf(main_fmt, EQUATION_FMT, "%i", "%i", "%i");
    printf(main_fmt, num1, operand, num2, ans);
}

// PrecisionString is only called in ComputeDoubles, so I forward declare it here.
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
    // Use 'x' as command-line thinks the char '*' is a pointer or something
    case 'x':
        ans = num1 * num2;
        break;
    case '/':
        ans = num1 / num2;
        return;
    case '%':
        // Modulo with floats/doubles is kinda weird due to the nature of remainders being whole
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

    char main_fmt[strlen(EQUATION_FMT)];
    char *fmt1 = PrecisionString(decimals1);
    char *fmt2 = PrecisionString(decimals2);

    sprintf(main_fmt, EQUATION_FMT, fmt1, fmt2, "%f");
    printf(main_fmt, num1, operand, num2, ans);

    free(fmt1);
    free(fmt2);
}

char *PrecisionString(int decimal_places)
{
    // Can't use static as we need variable amounts of decimal places.
    char *fmt_length = malloc(strlen(DECIMALS_FMT) * sizeof(char));

    // 1st arg is the target or output string
    // 2nd arg must be a format string, so I defined DECIMALS_FMT accordinaly
    // 3rd and above arg will be formatted into the 2nd arg
    sprintf(fmt_length, DECIMALS_FMT, decimal_places);
    return fmt_length;
}