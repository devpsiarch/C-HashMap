# Simple portable generic hash map implementation in pure C
A generic robin hood hash map implementation in C that is moderately annoying to use.
# What will hopefully be implemented
- Many different hashing algorithms.
- Many different Kind and implementation for the hash map to choose from.
# example usage:
```c
// define the template for the HashMap and its name (ull need it each time you wanna perform a operation)
TEMPLATE_DEFINE_HASHMAP(CharCharMap,CharChar,char,char);
// for now it impliments the robin hood hashing more later
// this is some tests as code example of the usage ...
int main(void){
    struct CharCharMap test;
    test.capacity = INIT_HASHMAP_CAPACITY;
    test.data = malloc(INIT_HASHMAP_CAPACITY * sizeof(struct CharChar));
    for (size_t i = 0; i < test.capacity; ++i) test.data[i].dead = true;

    // ---- Insert & Lookup Test ----
    struct CharChar item1 = {.key = 'c', .value = 'a'};
    HMAP_INSERT(CharCharMap, &test, item1);
    char* c = HMAP_LOOKUP(CharCharMap, &test, 'c');
    assert(c != NULL && *c == 'a');
    printf("PASS: Insert & lookup single element\n");

    // ---- Overwrite Existing Key ----
    struct CharChar item2 = {.key = 'c', .value = 'z'};
    HMAP_INSERT(CharCharMap, &test, item2);
    c = HMAP_LOOKUP(CharCharMap, &test, 'c');
    assert(c != NULL && *c == 'z');
    printf("PASS: Overwrite existing key\n");

    // ---- Insert Multiple Keys ----
    struct CharChar item3 = {.key = 'x', .value = '1'};
    struct CharChar item4 = {.key = 'y', .value = '2'};
    HMAP_INSERT(CharCharMap, &test, item3);
    HMAP_INSERT(CharCharMap, &test, item4);
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'x') == '1');
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'y') == '2');
    printf("PASS: Insert multiple keys\n");

    // ---- Delete Key ----
    HMAP_DELETE(CharCharMap, &test, 'c');
    assert(HMAP_LOOKUP(CharCharMap, &test, 'c') == NULL);
    printf("PASS: Delete key\n");

    // ---- Lookup Missing Key ----
    assert(HMAP_LOOKUP(CharCharMap, &test, 'q') == NULL);
    printf("PASS: Lookup missing key\n");

    // ---- Collision Handling ----
    struct CharChar item5 = {.key = 'A', .value = 'X'};
    struct CharChar item6 = {.key = 'B', .value = 'Y'};
    HMAP_INSERT(CharCharMap, &test, item5);
    HMAP_INSERT(CharCharMap, &test, item6);
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'A') == 'X');
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'B') == 'Y');
    printf("PASS: Collision handling\n");
defer:
    free(test.data);
    return 0;
}
```
# performence (soon)
