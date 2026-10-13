#define main leo_main
#include "leo.c"
#undef main

int main(void) {
    Leo *leo = calloc(1, sizeof(*leo));
    if (!leo) return 1;
    leo_school_init(&leo->school);
    printf("BEFORE nebula=%d river=%d\n", leo_school_glyph(leo,"nebula"), leo_school_glyph(leo,"river"));
    leo_school_hear(leo, "nebula river ocean");
    int at = leo_school_word_index(&leo->school, "nebula");
    printf("HEAR nebula heard=%u glyph=%d\n", at >= 0 ? leo->school.word[at].heard : 0, leo_school_glyph(leo,"nebula"));
    strcpy(leo->school.pending, "nebula");
    leo->school.pending_glyph = leo_glyph_find("fire");
    int closed = leo_school_close_question(leo, "river ocean");
    printf("ANSWER closed=%d nebula=%s guesses=%u hits=%u complex=%.6f pending=%s\n",
           closed, LEO_GLYPH_NAME[leo_school_glyph(leo,"nebula")], leo->school.guesses,
           leo->school.guess_hits, (double)leo->chamber[LEO_COMPLEX], leo->school.pending);
    free(leo);
    return 0;
}
