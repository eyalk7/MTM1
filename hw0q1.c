#include <stdlib.h>
#include <string.h>
#include <assert.h>

char *stringDuplicator(char *s, int times) {
    // (1)bug changed to assert without !
    assert(s);
    assert(times > 0);
    // (1)convension LEN -> len
    int len = strlen(s);
    // (2)bug +1 in the malloc for end of string
    char *out = malloc(len*times+1);
    // (2)convension assert not for internal function problems like in NULL malloc
    if (out == 0) return NULL;
    for (int i = 0; i < times; i++) {
    // (3)bug not to change the out pointer, instead use interator
        strcpy(out+i*len, s);
    }
    return out;
}
