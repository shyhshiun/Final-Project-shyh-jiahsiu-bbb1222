#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "toml/toml.h"



typedef struct Item {
    char* alias;
    char* name;
    char* desc;   // description
    char* icon;
} Item;


typedef struct Player {
    char*  starter;
    char** items;
    int    n_item;
} Player;


typedef struct Scene {
    char* alias;
    char* name;
    char* bg;    // background
} Scene;


typedef struct Character {
    char* alias;
    char* name;
    char* avatar; 
} Character;


typedef struct Event {
    char* alias;
    char* scene;    
    char* dialog;
    char* item;
} Event;


typedef struct Option {
    char* text;
    char* event;  // 可能是 NULL 
    char* next;   // 可能是 NULL
} Option;


typedef struct Dialogue {
    char*   alias;
    char*   text;
    char*   character;
    Option* opts;
    int     n_opt;   // number of option
    char*   item;
} Dialogue;


typedef enum Act_Type {
    EVENT = 0,
    DIALOGUE
} Act_Type;


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
               const char* toml_path);

Item* find_item(Item* items, int n_item, const char* alias);

Scene* find_scene(Scene* scenes, int n_scene, const char* alias);

Character* find_character(Character* characters, int n_character, const char* alias);

Event* find_event(Event* events, int n_event, const char* alias);

Dialogue* find_dialogue(Dialogue* dialogues, int n_dialogue, const char* alias);

bool add_item(char*** items, int* n_item, char* new_item);
