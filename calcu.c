#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief The number of command-line arguments our program needs.
 * @note If we got 2 arguments, the user is probably trying to look for "--help". 
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

/**
 * @brief A list of aliases for help commands.
 * @note Only used if we got only 2 command-line arguments.
*/
const char *HELP_COMMANDS[] = {"--help", "--h", "help", "h"};
#define NUM_OF_HELP_COMMANDS sizeof(HELP_COMMANDS) / sizeof(HELP_COMMANDS[0])

struct real_number
{
    
    bool is_negative;           /** * @note Init to false */
    bool is_decimal;            /** * @note Init to false */
    bool *is_valid_string;      /** * @note Init to valid address, set to NULL on error. */ 
    
    int integer;                /** * @note Init this to 0 on construction. */ 

    int num_decimal_places;     /** * @note Init to 0 if is_decimal is true. */
    double decimal;             /** * @note Init to 0.0 if is_decimal is true. */
    double decimal_places;      /** * @note Init to 0.0 if is_decimal is true. */
};
/**
 * @struct RealNumber
 * @brief Represent real numbers! Integers and Decimals (also negatives).
 * @note Members starting with 'is_' are set to false on construction, 
 * but set to true if the right conditions are met. 
 */
typedef struct real_number RealNumber;

void EvaluateNums(RealNumber *real1, char operand, RealNumber *real2);
RealNumber StringToReal(char *src);
/**
 * @brief Perform math on 2 input numbers.
 * @param argc how many command-line arguments we got.
 * @param argv what command-line argument/s we got.
 * @return 0 on success, 1 on failure.
 * @note The way I use forward declarations from here on, 
 * they are for functions called within the nearest 'major' function.
 * e.g. forward declare EvaluateNums and StringToReal to be used within main
 */
int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        for (int i = 0; i < NUM_OF_HELP_COMMANDS; i++)
        {
            /** 
             * @brief Compare our input using strcmp and see if it matched a help command.
             * @param argv our command-line args array, check the 2nd element
             * @param HELPCOMMANDS our predefined array of aliases for the "--help" command
             * @note strcmp returns 0 on success
             * (that is, both input strings have equal chars) 
            */ 
            int got_help = strcmp(argv[1], HELP_COMMANDS[i]);
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

        /** * @note Use '%zu' as size_t may not always be unsigned long on all platforms. */
        // printf("DECIMALS\n\t_FMT: %s\n\t_BUFF: %zu\n", DECIMALS_FMT, DECIMALS_BUFF);
        // printf("EQUATION\n\t_FMT: %s\n\t_BUFF: %zu\n", EQUATION_FMT, EQUATION_BUFF);
        return 0;
    }

    RealNumber num1 = StringToReal(argv[1]);
    char operand = argv[2][0];
    RealNumber num2 = StringToReal(argv[3]);

    if (num1.is_valid_string == NULL || num2.is_valid_string == NULL)
    {
        // printf("Invalid command-line inputs!\n");
        return 1;
    }

    EvaluateNums(&num1, operand, &num2);
    return 0;
}

bool IsSymbol(char indiv, RealNumber *struct_ptr);
void WriteValues(int val, RealNumber *struct_ptr);
/**
 * @brief 
 * @param src is a string representing (in theory anyway) a real number.
 * @return An instance of RealNumber with appropriate members filled out.
 * @note The member pointer .is_valid_string will be set to NULL on error.
 */
RealNumber StringToReal(char *src)
{
    bool valid = true;
    RealNumber ret = {
        .is_negative = false,
        .is_decimal = false,
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

/**
 * @brief Determine if an input char is a unary positive, unary negative or decimal point.
 * @param indiv a specific char from the source string.
 * @param struct_ptr a pointer to an instance of RealNumber.
 * @return true if the input char is one of the above 3 symbols, otherwise false.
 * @note Only considers + - .
 */
bool IsSymbol(char indiv, RealNumber *struct_ptr)
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

/**
 * @brief Set the values of the .integer and .decimal members based on the .is_ members.
 * @param val ASCII-value-of-character - ASCII-value-of-character-0
 * @param struct_ptr pointer to our RealNumber instance so we can modify it's members  directly.
 */
void WriteValues(int val, RealNumber *struct_ptr)
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
/**
 * @brief Check the type for both real1 and real2, then run the operation represented by operand.
 * @param real1 the first number you entered.
 * @param operand a simple mathematical operation.
 * @param real2 the second number you entered.
 */
void EvaluateNums(RealNumber *real1, char operand, RealNumber *real2)
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

/**
 * @brief Check the value of operand then run the appropriate calculation.
 * @param num1 
 * @param operand the desired calculation to run.
 * @param num2 
 * @return Forces a return if the operand is invalid.
 * @note Division prints the answer as a float then returns.
 */
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
/**
 * @brief Check the value of operand then run the appropriate calculation.
 * @param num1 
 * @param decimals1 num1's decimal places.
 * @param operand the desired calculation to run.
 * @param num2 
 * @param decimals2 num2's decimal places.
 * @return Forces a return if the operand is invalid.
 * @note Modulo casts both nums to ints.
 */
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
/**
 * @brief Create a formatted string for how many decimal places we want to print out.
 * @param decimal_places represents the number in %.(int)f
 * @return Said formatted string.
 * @note This returns a malloc'd pointer. Remember to free it!
 */
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