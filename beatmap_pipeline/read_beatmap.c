#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct beatmap {
    uint32_t timestamp;
    uint8_t key;
} beatmap_t;

void read_beatmap(char *filename, beatmap_t *beatmap, int beatmap_size) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;

    fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("Error opening file");
    }

    for (int i = 0; i < beatmap_size; i++) {
        // read an int from file for time
        uint32_t timestamp;
        uint8_t key;
        fread(&timestamp, sizeof(int), 1, fp);
        fread(&key, sizeof(uint8_t), 1, fp);
        beatmap[i].timestamp = timestamp;
        beatmap[i].key = key;
    }
}

int main() {
    beatmap_t *beatmap = malloc(sizeof(beatmap_t) * 100);
    read_beatmap("export.beatmap", beatmap, 30);
    for (int i = 0; i < 30; i++) {
        printf("%u %u\n", beatmap[i].timestamp, beatmap[i].key);
    }
    return 0;
}
