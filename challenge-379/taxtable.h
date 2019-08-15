#ifndef TAXTABLE_H
#define TAXTABLE_H

typedef struct {
    double cap;
    double rate;
} bracket;

typedef struct {
    int rows;
    bracket **brackets;
} taxtable;

taxtable *init_table();

double calculate_tax(taxtable *table, double income);
double calculate_income(taxtable *table, double tax);

void free_table(taxtable *table);
void parse_brackets(taxtable *table, char *path);

#endif
