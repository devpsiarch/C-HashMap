# Simple portable generic hash map implementation in pure C
A fast generic robin hood hash map implementation in C that is moderately annoying to use.
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
    HMAP_CREATE(&test);

    // ---- Insert & Lookup Test ----
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'c', .value = 'a'}));
    char* c = HMAP_LOOKUP(CharCharMap, &test, 'c');
    assert(c != NULL && *c == 'a');
    printf("PASS: Insert & lookup single element\n");

    // ---- Overwrite Existing Key ----
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'c', .value = 'z'}));
    c = HMAP_LOOKUP(CharCharMap, &test, 'c');
    assert(c != NULL && *c == 'z');
    printf("PASS: Overwrite existing key\n");

    // ---- Insert Multiple Keys ----
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'x', .value = '1'}));
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'y', .value = '2'}));
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
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'A', .value = 'X'}));
    HMAP_INSERT(CharCharMap, &test, ((struct CharChar){.key = 'B', .value = 'Y'}));
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'A') == 'X');
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'B') == 'Y');
    printf("PASS: Collision handling\n");
defer:
    free(test.data);
    return 0;
}
```
# performance
## Hashmap Benchmark Comparison (robin hood version)

This benchmark compares a custom **Robin Hood Hashmap** implementation against the standard **C++ STL `unordered_map`** using 500,000 elements on a AMD RYZEN 7 PRO 4.5 GHZ.

Output results using the `build.c` build file:
```
[OUROC_INFO]: executing [./main].
Time taken to insert 500000 elements: 0.114232.
Time taken to lookup 500000 elements: 0.013966.
Time taken to delete 500000 elements: 0.010190.
[OUROC_INFO]: execution done.
[OUROC_INFO]: executing [./stl].
Time taken to insert 500000 elements: 0.142615.
Time taken to lookup 500000 elements: 0.037740.
Time taken to delete 500000 elements: 0.034559.
[OUROC_INFO]: executing done.
```
---

### Results

| Operation | Robin Hashmap | STL `unordered_map` | Speedup |
|-----------|---------------|----------------------|---------|
| Insert    | **0.114 s**   | 0.143 s             | ~1.25× faster |
| Lookup    | **0.014 s**   | 0.038 s             | ~2.7× faster |
| Delete    | **0.010 s**   | 0.035 s             | ~3.4× faster |

---

### Analysis

- **Insert:**
  Robin Hood hashing is ~25% faster. Both spend time on resizing and memory, but STL pays extra cost for node allocations and pointer indirection.

- **Lookup:**
  Robin Hood hashing is ~3× faster. Cache-friendly open addressing keeps probes short and predictable, unlike STL’s pointer-heavy chaining.

- **Delete:**
  Robin Hood hashing is ~3.5× faster. STL struggles here due to node deletion and re-linking, while Robin Hood simply marks slots as deleted.
