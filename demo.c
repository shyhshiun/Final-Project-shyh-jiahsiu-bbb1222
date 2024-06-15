#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils/fn.h"
#include "utils/toml/toml.h"



int main()
{
    char*      path = "sample.toml";

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

    // 開始劇情
    Event*    event = &(events[0]);
    Dialogue* dialog = NULL;
    Act_Type  type = EVENT;
    int       opt_number;
    Option    opt;
    bool      is_ending = false;

    while (!is_ending) {
        switch (type) {
            case EVENT:
                printf("\033[33m用 SDL 顯示圖:\033[0m %s\n", event->scene);
                dialog = find_dialogue(dialogues, n_dialogue, event->dialog);
                type = DIALOGUE;
                break;
                
            case DIALOGUE:
                printf("\033[35m用 SDL 顯示對話:\033[0m %s\n", dialog->text);
                if (dialog->character) {
                    printf("用 SDL 顯示角色: %s\n", dialog->character);
                }

                if (dialog->n_opt > 0) {
                    printf("\033[32m------------------------ 出現選項 ------------------------\033[0m\n");
                    for (int i = 0; i < dialog->n_opt; i++) {
                        printf("選項(%d): %s\n", i + 1, dialog->opts[i].text);
                    }

                    printf("請選擇一個, 你選擇: ");
                    scanf("%d", &opt_number);
                    opt_number = opt_number - 1 >= dialog->n_opt ? dialog->n_opt - 1 : opt_number - 1;
                    opt = dialog->opts[opt_number];

                    if (opt.event) {
                        event = find_event(events, n_event, opt.event);
                        type = EVENT;
                    }
                    else if (opt.next) {
                        dialog = find_dialogue(dialogues, n_dialogue, opt.next);
                        type = DIALOGUE;
                    }
                    else {
                        printf("\033[31mdialogue's option format is error: '%s'\033[0m\n", dialog->alias);
                    }
                }

                printf("\n");

                if (dialog && strstr(dialog->alias, "ending")) {
                    is_ending = true;
                }

                break;

            default:
                printf("\033[31mUnknow type\033[0m\n");
        }
    }

    printf("\033[35m劇終囉 !!!\033[0m\n");
}