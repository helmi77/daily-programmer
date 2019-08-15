#include <stdio.h>
#include <stdlib.h>
#include "taxtable.h"

int main(int argc, char *argv[])
{
    taxtable *table = init_table();

    char *brackets_path = argv[1];
    parse_brackets(table, brackets_path);

    double tax = atof(argv[2]);
    double income = calculate_income(table, tax);
    printf("Tax: %.2f, Income: %.2f", tax, income);

    free_table(table);
    return EXIT_SUCCESS;
}
