#include <stdio.h>
#include <stdlib.h>
#include "semantic_tokenizer.h"

static void show(const char *s) {
    int tokens[128];
    int n = semtok_line(s, tokens, 128);
    printf("%s =>", s);
    for (int i = 0; i < n; i++) printf(" %s", GLYPH_NAMES[tokens[i]]);
    printf(" [%d]\n", n);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s CAVE_VOCAB_PATH\n", argv[0]);
        return 2;
    }
    int aliases = 0, unique = 0, conflicts = 0;
    for (int i = 0; SEM_WORD_MAP[i].word; i++) {
        aliases++;
        int first = 1;
        for (int j = 0; j < i; j++) {
            if (!strcmp(SEM_WORD_MAP[j].word, SEM_WORD_MAP[i].word)) {
                first = 0;
                if (strcmp(SEM_WORD_MAP[j].glyph, SEM_WORD_MAP[i].glyph)) conflicts++;
            }
        }
        unique += first;
    }
    printf("glyphs=%d aliases=%d unique_aliases=%d conflicting_duplicate_entries=%d\n", GLYPH_COUNT, aliases, unique, conflicts);
    show("the sun rose and the birds started singing");
    show("Count Dracula stood in the dark castle");
    show("she wrote code all night and found the bug");
    show("the old dog stretched by the fireplace and fell asleep");
    show("she started a new company and worked on it day and night");
    show("AI BE ai be computer is machine being");
    show("music song miss wish street little decide way");
    show("water river ocean dream rain rain");
    show("trees dreamed working walked loved walking");
    show("love unknown unknown love");
    show("вода огонь любовь silence");
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    char line[128]; int mismatches = 0, n = 0;
    if (!fgets(line, sizeof line, f)) { fclose(f); return 1; }
    int declared = atoi(line);
    while (fgets(line, sizeof line, f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (n >= GLYPH_COUNT || strcmp(line, GLYPH_NAMES[n])) {
            printf("vocab mismatch at %d: shipped=%s canonical=%s\n", n, line, n < GLYPH_COUNT ? GLYPH_NAMES[n] : "<none>");
            mismatches++;
        }
        n++;
    }
    fclose(f);
    printf("shipped_vocab_declared=%d lines=%d mismatches=%d\n", declared, n, mismatches);
    return 0;
}
