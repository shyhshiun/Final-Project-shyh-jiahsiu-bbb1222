#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toml/toml.h"
#include "fn.h"



void read_player(Player* player, const char* toml_path)
{
    char  errbuf[200];
    FILE* f = fopen(toml_path, "r");
    if (f == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }
    
    if (toml_key_exists(table, "player")) {
        toml_table_t* player_table = toml_table_in(table, "player");
        if (!player) {
            printf("\033[31mERROR: parse player !\033[0m\n");
        }

        toml_array_t* arr = toml_array_in(player_table, "inventory");
        int n_item = toml_array_nelem(arr);
        player->items = (char**)calloc(n_item, sizeof(char*));

        for (int i = 0; i < toml_array_nelem(arr); i++) {
            player->items[i] = toml_string_at(arr, i).u.s;
        }
        player->starter = toml_string_in(player_table, "starter").u.s;
        player->n_item = n_item;
    }
    else {
        printf("\033[31mERROR: parse player !\033[0m\n");
        exit(EXIT_FAILURE);
    }
}


void read_items(Item** items, int* n_item, const char* toml_path)
{
    FILE* f = fopen(toml_path, "r");
    if (!f) {
        printf("\033[31m (read_items) open %s failed\033[0m\n", toml_path);
        exit(EXIT_FAILURE);
    }

    char  errbuf[200];
    FILE* f2 = fopen(toml_path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }

    char          line[1000] = {'\0'};
    size_t        type_len;
    size_t        alias_len;
    char*         alias = NULL;
    Item*         items_ = (Item*)calloc(0, sizeof(Item));
    int           n_item_ = 0;
    toml_table_t* item_table = toml_table_in(table, "item");

    while (fgets(line, 1000, f)) {
        if (line[0] == '[') {
            type_len = strcspn(line, ".]") - 1;
            char type[20] = {'\0'};
            strncpy(type, line + 1, type_len);

            if (strcmp(type, "item") == 0) {
                alias_len = strcspn(line, "\n\0") - 3 - type_len;
                alias = (char*)calloc(alias_len + 1, sizeof(char));
                strncpy(alias, line + 2 + type_len, alias_len);

                toml_table_t* paragraph = toml_table_in(item_table, alias);
                if (!paragraph) {
                    printf("\033[31mERROR: parse item failed !\033[0m\n");
                    exit(EXIT_FAILURE);
                }

                n_item_++;
                items_ = (Item*)realloc(items_, n_item_ * sizeof(Item));
                items_[n_item_ - 1].alias = alias;
                items_[n_item_ - 1].name = toml_string_in(paragraph, "name").u.s;
                items_[n_item_ - 1].desc = toml_string_in(paragraph, "description").u.s;
                items_[n_item_ - 1].icon = toml_string_in(paragraph, "icon").u.s;
            }
        }

        memset(line, '\0', 1000);
    }

    *items = items_;
    *n_item = n_item_;

    fclose(f);
    fclose(f2);
}


void read_scenes(Scene** scenes, int* n_scene, const char* toml_path)
{
    FILE* f = fopen(toml_path, "r");
    if (!f) {
        printf("\033[31m (read_scenes) open %s failed\033[0m\n", toml_path);
        exit(EXIT_FAILURE);
    }

    char  errbuf[200];
    FILE* f2 = fopen(toml_path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }

    char          line[1000] = {'\0'};
    size_t        type_len;
    size_t        alias_len;
    char*         alias = NULL;
    Scene*        scenes_ = (Scene*)calloc(0, sizeof(Scene));
    int           n_scene_ = 0;
    toml_table_t* scene_table = toml_table_in(table, "scene");

    while (fgets(line, 1000, f)) {
        if (line[0] == '[') {
            type_len = strcspn(line, ".]") - 1;
            char type[20] = {'\0'};
            strncpy(type, line + 1, type_len);

            if (strcmp(type, "scene") == 0) {
                alias_len = strcspn(line, "\n\0") - 3 - type_len;
                alias = (char*)calloc(alias_len + 1, sizeof(char));
                strncpy(alias, line + 2 + type_len, alias_len);

                toml_table_t* paragraph = toml_table_in(scene_table, alias);
                if (!paragraph) {
                    printf("\033[31mERROR: parse scene failed !\033[0m\n");
                    exit(EXIT_FAILURE);
                }

                n_scene_++;
                scenes_ = (Scene*)realloc(scenes_, n_scene_ * sizeof(Scene));
                scenes_[n_scene_ - 1].alias = alias;
                scenes_[n_scene_ - 1].name = toml_string_in(paragraph, "name").u.s;
                scenes_[n_scene_ - 1].bg = toml_string_in(paragraph, "background").u.s;
            }
        }

        memset(line, '\0', 1000);
    }

    *scenes = scenes_;
    *n_scene = n_scene_;

    fclose(f);
    fclose(f2);
}


void read_characters(Character** characters, int* n_character, const char* toml_path)
{
    FILE* f = fopen(toml_path, "r");
    if (!f) {
        printf("\033[31m (read_characters) open %s failed\033[0m\n", toml_path);
        exit(EXIT_FAILURE);
    }

    char  errbuf[200];
    FILE* f2 = fopen(toml_path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }

    char          line[1000] = {'\0'};
    size_t        type_len;
    size_t        alias_len;
    char*         alias = NULL;
    Character*    characters_ = (Character*)calloc(0, sizeof(Scene));
    int           n_character_ = 0;
    toml_table_t* character_table = toml_table_in(table, "character");

    while (fgets(line, 1000, f)) {
        if (line[0] == '[') {
            type_len = strcspn(line, ".]") - 1;
            char type[20] = {'\0'};
            strncpy(type, line + 1, type_len);

            if (strcmp(type, "character") == 0) {
                alias_len = strcspn(line, "\n\0") - 3 - type_len;
                alias = (char*)calloc(alias_len + 1, sizeof(char));
                strncpy(alias, line + 2 + type_len, alias_len);

                toml_table_t* paragraph = toml_table_in(character_table, alias);
                if (!paragraph) {
                    printf("\033[31mERROR: parse character failed !\033[0m\n");
                    exit(EXIT_FAILURE);
                }

                n_character_++;
                characters_ = (Character*)realloc(characters_, n_character_ * sizeof(Character));
                characters_[n_character_ - 1].alias = alias;
                characters_[n_character_ - 1].name = toml_string_in(paragraph, "name").u.s;
                characters_[n_character_ - 1].avatar = toml_string_in(paragraph, "avatar").u.s;
            }
        }

        memset(line, '\0', 1000);
    }

    *characters = characters_;
    *n_character = n_character_;

    fclose(f);
    fclose(f2);
}


void read_events(Event** events, int* n_event, const char* toml_path)
{
    FILE* f = fopen(toml_path, "r");
    if (!f) {
        printf("\033[31m (read_events) open %s failed\033[0m\n", toml_path);
        exit(EXIT_FAILURE);
    }

    char  errbuf[200];
    FILE* f2 = fopen(toml_path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }

    char          line[1000] = {'\0'};
    size_t        type_len;
    size_t        alias_len;
    char*         alias = NULL;
    Event*        events_ = (Event*)calloc(0, sizeof(Event));
    int           n_event_ = 0;
    toml_table_t* event_table = toml_table_in(table, "event");

    while (fgets(line, 1000, f)) {
        if (line[0] == '[') {
            type_len = strcspn(line, ".]") - 1;
            char type[20] = {'\0'};
            strncpy(type, line + 1, type_len);

            if (strcmp(type, "event") == 0) {
                alias_len = strcspn(line, "\n\0") - 3 - type_len;
                alias = (char*)calloc(alias_len + 1, sizeof(char));
                strncpy(alias, line + 2 + type_len, alias_len);

                toml_table_t* paragraph = toml_table_in(event_table, alias);
                if (!paragraph) {
                    printf("\033[31mERROR: parse event failed !\033[0m\n");
                    exit(EXIT_FAILURE);
                }

                n_event_++;
                events_ = (Event*)realloc(events_, n_event_ * sizeof(Event));
                events_[n_event_ - 1].alias = alias;
                events_[n_event_ - 1].scene = toml_string_in(paragraph, "scene").u.s;
                events_[n_event_ - 1].dialog = toml_string_in(paragraph, "dialogue").u.s;
                if (toml_key_exists(paragraph, "item")) {
                    events_[n_event_ - 1].item = toml_string_in(paragraph, "item").u.s;
                }
                else {
                    events_[n_event_ - 1].item = NULL;
                }
            }
        }

        memset(line, '\0', 1000);
    }

    *events = events_;
    *n_event = n_event_;

    fclose(f);
    fclose(f2);
}


toml_table_t* get_opts_table(const char* toml_path, const char* alias) {
    FILE* file = fopen(toml_path, "rb");
    if (!file) {
        perror("fopen");
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)calloc(size + 1, sizeof(char));
    if (!content) {
        perror("calloc");
        fclose(file);
    }

    fread(content, 1, size, file);
    content[size] = '\0';
    fclose(file);

    toml_table_t* table = toml_parse(content, NULL, 0);
    free(content);
    if (!table) {
        printf("\033[31mFailed to parse TOML raw file.\033[0m\n");
        exit(EXIT_FAILURE);
    }

    toml_table_t* dialogue_table = toml_table_in(table, "dialogue");
    if (!dialogue_table) {
        printf("\033[31mFailed to parse raw dialogue.\033[0m\n");
        exit(EXIT_FAILURE);
    }

    toml_table_t* opts_table = toml_table_in(dialogue_table, alias);
    if (!opts_table) {
        printf("\033[31mFailed to find '%s' table.\033[0m\n", alias);
        exit(EXIT_FAILURE);
    }

    return opts_table;
}


void read_dialogues(Dialogue** dialogues, int* n_dialogue, const char* toml_path)
{
    FILE* f = fopen(toml_path, "r");
    if (!f) {
        printf("\033[31m (read_events) open %s failed\033[0m\n", toml_path);
        exit(EXIT_FAILURE);
    }

    char  errbuf[200];
    FILE* f2 = fopen(toml_path, "r");
    if (f2 == NULL) {
        printf("\033[31mread %s failed\033[0m\n", toml_path);
    }

    toml_table_t* table = toml_parse_file(f2, errbuf, sizeof(errbuf));
    if (table == NULL) {
        printf("\033[31mparse %s failed\033[0m\n", toml_path);
    }

    char          line[1000] = {'\0'};
    size_t        type_len;
    size_t        alias_len;
    char*         alias = NULL;
    Dialogue*     dialogues_ = (Dialogue*)calloc(0, sizeof(Dialogue));
    int           n_dialogue_ = 0;
    toml_table_t* dialogue_table = toml_table_in(table, "dialogue");

    while (fgets(line, 1000, f)) {
        if (line[0] == '[' && line[1] != '[') {
            type_len = strcspn(line, ".]") - 1;
            char type[20] = {'\0'};
            strncpy(type, line + 1, type_len);

            if (strcmp(type, "dialogue") == 0) {
                alias_len = strcspn(line, "\n\0") - 3 - type_len;
                alias = (char*)calloc(alias_len + 1, sizeof(char));
                strncpy(alias, line + 2 + type_len, alias_len);

                toml_table_t* paragraph = toml_table_in(dialogue_table, alias);
                if (!paragraph) {
                    printf("\033[31mERROR: parse dialogue failed !\033[0m\n");
                    exit(EXIT_FAILURE);
                }
                    
                // options
                int     n_opt = 0;
                Option* opts = NULL;
                toml_table_t* opts_table = get_opts_table(toml_path, alias);
                
                if (toml_key_exists(opts_table, "options")) {
                    toml_array_t* opt_arr = toml_array_in(opts_table, "options");
                    if (!opt_arr) {
                        printf("\033[31mERROR: parse options failed !\033[0m\n");
                        exit(EXIT_FAILURE);
                    }

                    n_opt  = toml_array_nelem(opt_arr);
                    opts   = (Option*)calloc(n_opt, sizeof(Option));

                    for (int i = 0; i < n_opt; ++i) {
                        toml_table_t* opt = toml_table_at(opt_arr, i);

                        opts[i].text = toml_string_in(opt, "text").u.s;
                        if (toml_key_exists(opt, "next")) {
                            opts[i].next = toml_string_in(opt, "next").u.s;
                        }
                        else {
                            opts[i].next = NULL;
                        }
                        if (toml_key_exists(opt, "event")) {
                            opts[i].event = toml_string_in(opt, "event").u.s;
                        }
                        else {
                            opts[i].event = NULL;
                        }
                    }
                }

                // update
                n_dialogue_++;
                dialogues_ = (Dialogue*)realloc(dialogues_, n_dialogue_ * sizeof(Dialogue));
                dialogues_[n_dialogue_ - 1].alias = alias;
                dialogues_[n_dialogue_ - 1].text = toml_string_in(paragraph, "text").u.s;
                size_t len = strlen(dialogues_[n_dialogue_ - 1].text);
                dialogues_[n_dialogue_ - 1].text[len - 1] = '\0';    // 去除 \n
                if (toml_key_exists(paragraph, "character")) {
                    dialogues_[n_dialogue_ - 1].character = toml_string_in(paragraph, "character").u.s;
                }
                else {
                    dialogues_[n_dialogue_ - 1].character = NULL;
                }
                if (n_opt > 0) {
                    dialogues_[n_dialogue_ - 1].n_opt = n_opt;
                    dialogues_[n_dialogue_ - 1].opts = opts;
                }
                else {
                    dialogues_[n_dialogue_ - 1].n_opt = 0;
                }
                if (toml_key_exists(paragraph, "item")) {
                    dialogues_[n_dialogue_ - 1].item = toml_string_in(paragraph, "item").u.s;
                }
                else {
                    dialogues_[n_dialogue_ - 1].item = NULL;
                }
            }
        }

        memset(line, '\0', 1000);
    }

    *dialogues = dialogues_;
    *n_dialogue = n_dialogue_;

    fclose(f);
    fclose(f2);
}


void read_toml(Player* Player,
               Item** items, 
               int* n_item,
               Scene** scenes, 
               int* n_scene,
               Character** characters, 
               int* n_character,
               Event** events, 
               int* n_event,
               Dialogue** dialogs,
               int* n_dialog,
               const char* toml_path)
{
    read_player(Player, toml_path);
    read_items(items, n_item, toml_path);
    read_scenes(scenes, n_scene, toml_path);
    read_characters(characters, n_character, toml_path);
    read_events(events, n_event, toml_path);
    read_dialogues(dialogs, n_dialog, toml_path);
}


Item* find_item(Item* items, int n_item, const char* alias)
{
    for (int i = 0; i < n_item; i++) {
        if (strcmp(items[i].alias, alias) == 0) {
            return &(items[i]);
        }
    }

    printf("\033[31mCannot find the item: '%s'\033[0m\n", alias);
    exit(EXIT_FAILURE);

    return NULL;
}   


Scene* find_scene(Scene* scenes, int n_scene, const char* alias)
{
    for (int i = 0; i < n_scene; i++) {
        if (strcmp(scenes[i].alias, alias) == 0) {
            return &(scenes[i]);
        }
    }

    printf("\033[31mCannot find the scene: '%s'\033[0m\n", alias);
    exit(EXIT_FAILURE);

    return NULL;
}   


Character* find_character(Character* characters, int n_character, const char* alias)
{
    for (int i = 0; i < n_character; i++) {
        if (strcmp(characters[i].alias, alias) == 0) {
            return &(characters[i]);
        }
    }

    printf("\033[31mCannot find the character: '%s'\033[0m\n", alias);
    exit(EXIT_FAILURE);

    return NULL;
}   


Event* find_event(Event* events, int n_event, const char* alias)
{
    for (int i = 0; i < n_event; i++) {
        if (strcmp(events[i].alias, alias) == 0) {
            return &(events[i]);
        }
    }

    printf("\033[31mCannot find the event: '%s'\033[0m\n", alias);
    exit(EXIT_FAILURE);

    return NULL;
}   


Dialogue* find_dialogue(Dialogue* dialogues, int n_dialogue, const char* alias)
{
    for (int i = 0; i < n_dialogue; i++) {
        if (strcmp(dialogues[i].alias, alias) == 0) {
            return &(dialogues[i]);
        }
    }

    printf("\033[31mCannot find the dialogue: '%s'\033[0m\n", alias);
    exit(EXIT_FAILURE);

    return NULL;
}   
