#include <stdio.h>
#include <stdlib.h>
#include "taxtable.h"

int main(int argc, char *argv[])
{
    taxtable *table = init_table();

    char *brackets_path = argv[2];
    parse_brackets(table, brackets_path);

    double income = atof(argv[1]);
    double tax = calculate_tax(table, income);
    printf("Income: %f, Tax: %f\n", income, tax);

    free_table(table);
    return EXIT_SUCCESS;
}
