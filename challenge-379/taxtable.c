#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "taxtable.h"

taxtable *init_table()
{
    taxtable *table = malloc(sizeof(taxtable));
    return table;
}

double calculate_tax(taxtable *table, double income)
{
    double prev_bracket = 0, tax = 0, taxable = income;
    for (int i = 0; i < table->rows && taxable > 0; ++i)
    {
        double bracket = table->brackets[i]->cap - prev_bracket;
        prev_bracket = table->brackets[i]->cap;

        tax += fmin(taxable, bracket) * table->brackets[i]->rate;
        taxable -= bracket;
    }
    return floor(tax);
}

void free_table(taxtable *table)
{
    if (table != NULL)
    {
        for (int i = 0; i < table->rows; ++i)
        {
            free(table->brackets[i]);
        }
        free(table->brackets);
        free(table);
    }
}

void parse_brackets(taxtable *table, char *path)
{
    FILE *brackets_fp = fopen(path, "r");
    if (brackets_fp == NULL)
    {
        perror("Failed to read brackets file");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    for (int ch = 0; ch != EOF; ch = fgetc(brackets_fp))
    {
        if (ch == '\n' || ch == '\r')
            ++line_count;
    }
    fseek(brackets_fp, 0, SEEK_SET);

    table->rows = line_count;
    table->brackets = malloc(sizeof(bracket*) * line_count);

    int index = 0;
    char *line = malloc(sizeof(char) * BUFSIZ);
    while (fgets(line, BUFSIZ, brackets_fp) != NULL)
    {
        char *cap = strtok(line, "\t ");
        char *rate = strtok(NULL, "\t ");

        bracket *bracket = malloc(sizeof(bracket));
        if (index == line_count - 1)
        {
            bracket->cap = LONG_MAX;
            bracket->rate = atof(cap);
        }
        else
        {
            bracket->cap = atof(cap);
            bracket->rate = atof(rate);
        }
        table->brackets[index++] = bracket;
    }
    free(line);
    fclose(brackets_fp);
}
