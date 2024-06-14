#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toml/toml.h"



typedef struct Choice {
    char* desc;
    char* outcome;
} Choice;


typedef struct Option {
    char* text;
    char* next;
} Option;


typedef struct Act {
    char*   type;
    char*   name;
    char*   scene;
    char*   desc;
    Choice* choices;
    size_t  n_choice;
    char*   text;
    Option* options;
    size_t  n_option;
    char*   character;
    char*   dialogue;
} Act;

void free_Acts(Act* acts, int n_act);

int read_toml(Act** acts, int* n_act, const char* path);

void print_Act(Act* act);

void print_Act2(Act* act);
