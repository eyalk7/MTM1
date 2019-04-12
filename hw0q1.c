#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <stdio.h>

char* stringDuplicator(char* str, int times);

int main() {
    char* out = stringDuplicator("Hello", 3);
    if (out)
        printf("%s", out);

    return 0;
}

char *stringDuplicator(char *str, int times) {
    assert(str);
    assert(times > 0);
    int len = strlen(str);
    char *out = malloc(len * times + 1);
    if (out == NULL) return NULL;
    for (int i = 0; i < times; i++) {
        strcpy(out + (i * len), str);
    }
    return out;
}

