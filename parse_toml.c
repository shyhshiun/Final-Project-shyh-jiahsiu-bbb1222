#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include "utils/toml/toml.h"



static void error(const char* msg, const char* msg1)
{
    fprintf(stderr, "ERROR: %s%s\n", msg, msg1?msg1:"");
    exit(EXIT_FAILURE);
}


int main(int arc, char* arv[])
{
    FILE* fp;
    char  errbuf[200];

    // read toml
    fp = fopen("story.toml", "r");
    if (!fp) {
        error("cannot open %s ", strerror(errno));
    }

    // parse toml
    toml_table_t* table = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);
    if (!table) {
        error("cannot parse - ", errbuf);
    }

    // get content of [city]
    toml_table_t* city = toml_table_in(table, "city");
    if (!city) {
        error("missing [city]", "");
    }

    // get value of key
    toml_datum_t img = toml_string_in(city, "img");
    if (!img.ok) {
        error("cannot read img", "");
    }
    toml_datum_t desc = toml_string_in(city, "desc");
    if (!desc.ok) {
        error("cannot read desc", "");
    }
    toml_datum_t event = toml_string_in(city, "event");
    if (!event.ok) {
        error("cannot read event", "");
    }

    // print
    printf("img: %s\n", img.u.s);
    printf("desc: %s\n", desc.u.s);
    printf("event: %s\n", event.u.s);

    // 4. Free memory
    free(img.u.s);
    free(desc.u.s);
    free(event.u.s);
    toml_free(table);
    return 0;
}