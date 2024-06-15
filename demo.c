#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/fn.h"
#include "utils/toml/toml.h"



int main()
{
    char*      path = "story.toml";

    Item*      items = NULL;
    int        n_item;

    Scene*     scenes = NULL;
    int        n_scene;

    Character* characters = NULL;
    int        n_character;

    Event*     events = NULL;
    int        n_event;

    Dialogue*  dialogues = NULL;
    int        n_dialogue;

    Player player;

    // read all the information in toml
    read_toml(&player, 
              &items, &n_item,
              &scenes, &n_scene,
              &characters, &n_character,
              &events, &n_event,
              &dialogues, &n_dialogue,
              path);

    printf("-------------------------------------------------------\n");
    printf("\033[35mnumber of item: %d\033[0m\n", n_item);
    printf("\033[35mnumber of scene: %d\033[0m\n", n_scene);
    printf("\033[35mnumber of character: %d\033[0m\n", n_character);
    printf("\033[35mnumber of event: %d\033[0m\n", n_event);
    printf("\033[35mnumber of dialogue: %d\033[0m\n", n_dialogue);

    // find the target item
    Item* item = find_item(items, n_item, "envelope");
    printf("-------------------------------------------------------\n");
    printf("Found the item: 'envelope'\n");
    printf("name: %s\n", item->name);
    printf("description: %s\n", item->desc);
    printf("icon: %s\n", item->icon);

    // find the target scene
    Scene* scene = find_scene(scenes, n_scene, "street");
    printf("-------------------------------------------------------\n");
    printf("Found the scene: 'street'\n");
    printf("name: %s\n", scene->name);
    printf("background: %s\n", scene->bg);

    // find the target character
    Character* character = find_character(characters, n_character, "frienddad");
    printf("-------------------------------------------------------\n");
    printf("Found the character: 'frienddad'\n");
    printf("name: %s\n", character->name);
    printf("avatar: %s\n", character->avatar);
    printf("tachie: %s\n", character->tachie);

    // find the target event
    Event* event = find_event(events, n_event, "findtreasure");
    printf("-------------------------------------------------------\n");
    printf("Found the event: 'findtreasure'\n");
    printf("scene: %s\n", event->scene);
    printf("dialog: %s\n", event->dialog);

    // find the target dialogue
    Dialogue* dialogue = find_dialogue(dialogues, n_dialogue, "scene23");
    printf("-------------------------------------------------------\n");
    printf("Found the dialogue: 'scene23'\n");
    if (dialogue->character) {
        printf("character: %s\n", dialogue->character);
    }
    printf("text: %s\n", dialogue->text);
    for (int i = 0; i < dialogue->n_opt; i++) {
        printf("option (%d):\n", i + 1);
        printf("    text: %s\n", dialogue->opts[i].text);
        if (dialogue->opts[i].event) {
            printf("    event: %s\n", dialogue->opts[i].event);
        }
        if (dialogue->opts[i].next) {
            printf("    next: %s\n", dialogue->opts[i].next);
        }
    }
}