#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "taxtable.h"

#define EPSILON 1.0E-5

taxtable *init_table()
{
    taxtable *table = malloc(sizeof(taxtable));
    return table;
}

double get_overall_tax(taxtable *table, double income)
{
    if (income <= 0)
    {
        fputs("Income must be a positive number", stderr);
        exit(EXIT_FAILURE);
    }

    double tax = calculate_tax(table, income);
    return tax / income;
}

double search_incomes(taxtable *table, double target_tax, double lower, double upper)
{
    double mid = lower + (upper - lower) / 2;
    double current_tax = get_overall_tax(table, mid);
    if (fabs(current_tax - target_tax) <= EPSILON)
        return mid;

    if (target_tax > current_tax)
    {
        return search_incomes(table, target_tax, mid, upper);
    }
    return search_incomes(table, target_tax, lower, mid);
}

double search_brackets(taxtable *table
        , double target_tax
        , double income
        , int lower_index
        , int upper_index
)
{
    if (lower_index > upper_index)
        return -1;

    int search_index = lower_index + (upper_index - lower_index) / 2;
    income = table->brackets[search_index]->cap;
    double current_tax = get_overall_tax(table, income);
    if (fabs(current_tax - target_tax) <= EPSILON)
        return income;

    int next_index = (int) fmin(search_index + 1, table->rows - 1);
    double next_cap = table->brackets[next_index]->cap;
    double upper_bound = get_overall_tax(table, next_cap);

    if (target_tax > current_tax && target_tax < upper_bound)
        return search_incomes(table, target_tax, income, next_cap);
    else
    {
        if (target_tax > current_tax)
            return search_brackets(table, target_tax, income, search_index + 1, upper_index);
        return search_brackets(table, target_tax, income, lower_index, search_index - 1);
    }
}

double calculate_income(taxtable *table, double target_tax)
{
    if (target_tax >= table->brackets[table->rows - 1]->rate)
    {
        fputs("No such income value exists", stderr);
        exit(EXIT_FAILURE);
    }
    return search_brackets(table, target_tax, 0.0, 0, table->rows - 1);
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

int get_line_count(FILE *fp)
{
    int line_count = 0;
    for (int ch = 0; ch != EOF; ch = fgetc(fp))
    {
        if (ch == '\n' || ch == '\r')
            ++line_count;
    }
    fseek(fp, 0, SEEK_SET);
    return line_count;
}

void parse_brackets(taxtable *table, char *path)
{
    FILE *brackets_fp = fopen(path, "r");
    if (brackets_fp == NULL)
    {
        perror("Failed to read brackets file");
        exit(EXIT_FAILURE);
    }

    table->rows = get_line_count(brackets_fp);
    table->brackets = malloc(sizeof(bracket*) * table->rows);

    int index = 0;
    char *line = malloc(sizeof(char) * BUFSIZ);
    while (fgets(line, BUFSIZ, brackets_fp) != NULL)
    {
        char *cap = strtok(line, "\t ");
        char *rate = strtok(NULL, "\t ");

        bracket *bracket = malloc(sizeof(bracket));
        if (index == table->rows - 1)
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
