#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toml/toml.h"
#include "fn.h"



Act init_Act()
{
    Act act;
    act.type = NULL;
    act.name = NULL;
    act.scene = NULL; 
    act.desc = NULL;
    act.text = NULL;
    act.character = NULL;
    act.dialogue = NULL;
    act.choices = NULL;
    act.options = NULL;
    act.n_choice = 0;
    act.n_option = 0;
    return act;
}


static void error(const char* msg, const char* msg1)
{
    fprintf(stderr, "ERROR: %s%s\n", msg, msg1 ? msg1 : "");
    exit(EXIT_FAILURE);
}


void parse_event(Act* act, toml_table_t* paragraph)
{    
    if (toml_key_exists(paragraph, "scene")) {
        act->scene = toml_string_in(paragraph, "scene").u.s;
    } 
    if (toml_key_exists(paragraph, "description")) {
        act->desc = toml_string_in(paragraph, "description").u.s;
    } 
    if (toml_key_exists(paragraph, "dialogue")) {
        act->dialogue = toml_string_in(paragraph, "dialogue").u.s;
    } 
    if (toml_key_exists(paragraph, "choices")) {
        toml_array_t* choices  = toml_array_in(paragraph, "choices");
        act->n_choice = toml_array_nelem(choices);
        act->choices  = (Choice*)calloc(act->n_choice, sizeof(Choice));
        for (int i = 0; i < act->n_choice; i++) {
            Choice sChoice;
            toml_table_t* choice = toml_table_at(choices, i);
            sChoice.desc = toml_string_in(choice, "description").u.s;
            sChoice.outcome = toml_string_in(choice, "outcome").u.s;
            act->choices[i] = sChoice;
        }
    } 
}


void parse_dialogue(Act* act, toml_table_t* paragraph)
{
    if (toml_key_exists(paragraph, "character")) {
        act->character = toml_string_in(paragraph, "character").u.s;
    } 
    if (toml_key_exists(paragraph, "text")) {
        act->text = toml_string_in(paragraph, "text").u.s;
    } 
    if (toml_key_exists(paragraph, "options")) {
        toml_array_t* options  = toml_array_in(paragraph, "options");
        act->n_option = toml_array_nelem(options);
        act->options  = (Option*)calloc(act->n_option, sizeof(Option));
        for (int i = 0; i < act->n_option; i++) {
            Option sOption;
            toml_table_t* option = toml_table_at(options, i);
            sOption.text = toml_string_in(option, "text").u.s;
            sOption.next = toml_string_in(option, "next").u.s;
            act->options[i] = sOption;
        }
    } 
}


void free_Acts(Act* acts, int n_act)
{
    for (int i = 0; i < n_act; i++) {
        if (acts[i].type != NULL) {
            free(acts[i].type);
        }
        if (acts[i].name != NULL) {
            free(acts[i].name);
        }
        if (acts[i].scene != NULL) {
            free(acts[i].scene);
        }
        if (acts[i].desc != NULL) {
            free(acts[i].desc);
        }
        if (acts[i].text != NULL) {
            free(acts[i].text);
        }
        if (acts[i].character != NULL) {
            free(acts[i].character);
        }
        if (acts[i].dialogue != NULL) {
            free(acts[i].dialogue);
        }
        if (acts[i].n_choice != 0) {
            size_t  n_choice = acts[i].n_choice;
            Choice* choices = acts[i].choices;
            for (int k = 0; k < n_choice; k++) {
                if (choices[k].desc != NULL) {
                    free(choices[k].desc);
                }
                if (choices[k].outcome != NULL) {
                    free(choices[k].outcome);
                }
            }
            free(choices);
        }
        if (acts[i].n_option != 0) {
            size_t  n_option = acts[i].n_option;
            Option* options = acts[i].options;
            for (int k = 0; k < n_option; k++) {
                if (options[k].text != NULL) {
                    free(options[k].text);
                }
                if (options[k].next != NULL) {
                    free(options[k].next);
                }
            }
            free(options);
        }
    }

    if (acts != NULL) {
        free(acts);
    }
}


int read_toml(Act** acts, int* n_act, const char* path)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("\033[31mread %s failed\033[0m\n", path);
        return -1;
    }

    char  errbuf[200];
    FILE* f2 = fopen(path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        error("\033[31mparse %s failed !\033[0m\n", "");
    }

    char          line[1000] = {'\0'};
    char*         type = NULL;
    char*         name = NULL;
    size_t        type_len;
    size_t        name_len;
    toml_table_t* paragraph = NULL;
    Act*          _acts = (Act*)calloc(0, sizeof(Act));
    int           _n_act  = 0;

    while (fgets(line, 1000, f)) {
        if (line[0] == '[') {
            type_len = strcspn(line, ".\n\0") - 1;
            name_len = strcspn(line, "\n\0") - type_len - 3;
            type = (char*)calloc(type_len + 1, sizeof(char));
            name = (char*)calloc(name_len + 1, sizeof(char));
            strncpy(type, &(line[1]), type_len);
            strncpy(name, &(line[1 + type_len + 1]), name_len);
            
            Act act = init_Act();
            act.type = type;
            act.name = name;
            
            paragraph = toml_table_in(toml_table_in(table, act.type), act.name);
            if (paragraph == NULL) {
                printf("\033[31mERROR !\033[0m\n");
                return -1;
            }

            if (strcmp(act.type, "event") == 0) {
                parse_event(&act, paragraph);
            }
            else if (strcmp(act.type, "dialogue") == 0) {
                parse_dialogue(&act, paragraph);
            }
            else {
                printf("\033[31munknown act type: %s\033[0m\n", act.type);
                return -1;
            }

            _n_act++;
            _acts = (Act*)realloc(_acts, _n_act * sizeof(Act));
            _acts[_n_act - 1] = act;

            memset(line, '\0', 1000);
        }
    }   

    *acts = _acts;
    *n_act = _n_act;

    fclose(f);
    fclose(f2);
    toml_free(table);

    return 0;
}


void print_Act(Act* act)
{
    printf("[%s.%s]\n", act->type, act->name);
    
    if (strcmp(act->type, "event") == 0) {
        if (act->scene != NULL) {
            printf("scene: %s\n", act->scene);
        } 
        if (act->desc != NULL) {
            printf("description: %s\n", act->desc);
        } 
        if (act->dialogue != NULL) {
            printf("dialogue: %s\n", act->dialogue);
        } 
        for (int i = 0; i < act->n_choice; i++) {
            printf("Choices: %d\n", i + 1);
            printf("    description: %s\n", act->choices[i].desc);
            printf("    outcome: %s\n", act->choices[i].outcome);
        }
    }
    else if (strcmp(act->type, "dialogue") == 0) {
        if (act->character != NULL) {
            printf("character: %s\n", act->character);
        } 
        if (act->text != NULL) {
            printf("text: %s\n", act->text);
        } 
        for (int i = 0; i < act->n_option; i++) {
            printf("Choices: %d\n", i + 1);
            printf("    text: %s\n", act->options[i].text);
            printf("    next: %s\n", act->options[i].next);
        }
    }
}


void print_Act2(Act* act)
{
    printf("[%s.%s]\n", act->type, act->name);
    
    if (strcmp(act->type, "event") == 0) {
        if (act->scene != NULL) {
            printf("scene: %s,  %ld\n", act->scene, strlen(act->scene));
        } 
        if (act->desc != NULL) {
            printf("description: %s,  %ld\n", act->desc, strlen(act->desc));
        } 
        if (act->dialogue != NULL) {
            printf("dialogue: %s,  %ld\n", act->dialogue, strlen(act->dialogue));
        } 
        for (int i = 0; i < act->n_choice; i++) {
            printf("Choices: %d\n", i + 1);
            printf("    description: %s,  %ld\n", act->choices[i].desc, strlen(act->choices[i].desc));
            printf("    outcome: %s,  %ld\n", act->choices[i].outcome, strlen(act->choices[i].outcome));
        }
    }
    else if (strcmp(act->type, "dialogue") == 0) {
        if (act->character != NULL) {
            printf("character: %s,  %ld\n", act->character, strlen(act->character));
        } 
        if (act->text != NULL) {
            printf("text: %s,  %ld\n", act->text, strlen(act->text));
        } 
        for (int i = 0; i < act->n_option; i++) {
            printf("Choices: %d\n", i + 1);
            printf("    text: %s,  %ld\n", act->options[i].text, strlen(act->options[i].text));
            printf("    next: %s,  %ld\n", act->options[i].next, strlen(act->options[i].next));
        }
    }
}

