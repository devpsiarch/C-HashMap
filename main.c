#include "ouroc.h"
#define OUROC_IMPLI

#include "template_hmap.h"
#define TEMPLATE_HMAP_IMPL

#include <assert.h>


struct bucket {
    int key;
    int value;
    bool dead;
};

struct hmap {
    struct bucket* data;
    size_t capacity;
};

/*
name_hash_table => declare && defined function that do the job
    macros => select the functions to call < based on name of the hashtable
*/

struct IntInt {
    int key;
    int value;
    bool dead;
};
struct IntIntMap {
    struct IntInt* data;
    size_t capacity;
};


void rehash(struct hmap* map);
void insert(struct hmap* map,struct bucket value);
int lookup(struct hmap* map,int key);
void delete(struct hmap*map,int key);

// triggered when we exceed buckets or some other metric
void rehash(struct hmap* map){
    struct bucket* old_data = map->data;
    size_t old_size = map->capacity;
    map->capacity *= 2;
    map->data = malloc(sizeof(struct bucket)*map->capacity);
    for(size_t i = 0 ; i < map->capacity ; ++i) map->data[i].dead = true;
    // copy back the old_data into the new data
    for(size_t j = 0 ; j < old_size ; ++j){
        insert(map,old_data[j]);
    }
    free(old_data);
}

// needs only void* and size of said pointer to achieve the hash
void insert(struct hmap* map,struct bucket value){
    value.dead = false;
    uint32_t index = jenkins_one_at_a_time_hash(&(value.key),sizeof(value.key)) % map->capacity;
    printf("%u\n",index);
    printf("key : %d ,size %zu\n",&(value.key),sizeof(value.key));
    uint32_t saved = index;
    while(!(map->data[index].key == value.key) && !map->data[index].dead){
        printf("Inserting rn ...\n");
        ++index;
        index %= map->capacity;
        if(index == saved){
            rehash(map);
            insert(map,value);
            return;
        }
    }
    
    printf("We put it in %u\n",index);
    map->data[index] = value;
}


// we only need the pointer to the key and its size
int lookup(struct hmap* map,int key){
    uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;
    printf("%u\n",index);
    uint32_t i = index;
    while(map->data[index].dead == false && map->data[index].key != key){
        printf("Searching rn ...\n");
        ++index;
        index %= map->capacity;
        if(i == index){
            printf("WE HAVE WRAP AROUND , RETURNNING -1\n");
            return -1;
        }
    }
    if(map->data[index].dead == false) return map->data[index].value;
    else return -1;
}


// void* to key and its size
void delete(struct hmap*map,int key){
    uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;
    printf("%u\n",index);
    uint32_t i = index;
    while(map->data[index].key != key){
        printf("deleting ...\n");
        ++index;
        index %= map->capacity;
        if(i == index) return; // it wasnt even there
    }
    map->data[index].dead = true;
}

#define TEMPLATE_KV(T,U,template_name)                                          \
    struct template_name {                                                      \
        T key;                                                                  \
        U value;                                                                \
        bool dead;                                                              \
        void (*alloc)(struct template_name*);                                   \
        bool (*equal)(struct template_name*,struct template_name*);             \
        void (*free)(struct template_name*);                                    \
    }


#define DEC_KV(pair_name,name,k,v)      \
    struct pair_name name = {           \
            .key = (k),                 \
            .value = (v),               \
            .alloc = NULL,              \
            .free = NULL,               \
            .equal = NULL               \
    }


#define DEC_COMPLEX_KV(pair_name,name,a,f,e)\
    struct pair_name name = {               \
            .alloc = a,                     \
            .free = f,                      \
            .equal = e                      \
    }



#define DEL_KV(kv)                      \
do{                                     \
    if((kv)->free != NULL)              \
        (kv)->free((kv));               \
}while(0)


// Usage: defining the templates used
TEMPLATE_KV(int,int,int_pair);

TEMPLATE_DEFINE_HASHMAP(CharCharMap,CharChar,char,char);

int main(void){
    // THANKS TO LLM for writting the tests !!!
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
    // Assuming your hash function might collide chars with same hash index
    // You can force collision by inserting different keys until they land same bucket
    struct CharChar item5 = {.key = 'A', .value = 'X'};
    struct CharChar item6 = {.key = 'B', .value = 'Y'};
    HMAP_INSERT(CharCharMap, &test, item5);
    HMAP_INSERT(CharCharMap, &test, item6);
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'A') == 'X');
    assert(*HMAP_LOOKUP(CharCharMap, &test, 'B') == 'Y');
    printf("PASS: Collision handling\n");

    free(test.data);
    printf("All tests passed ✅\n");    return 0;
    return 0;
    // init <done>
    struct hmap* map = malloc(sizeof(struct hmap));
    map->capacity = INIT_HASHMAP_CAPACITY;
    map->data = malloc(INIT_HASHMAP_CAPACITY*sizeof(struct bucket));
    for(int i = 0 ; i < INIT_HASHMAP_CAPACITY ; ++i) map->data[i].dead = true;
    // insert 
    // insert
    insert(map, (struct bucket){.key = 15, .value = 1});
    insert(map, (struct bucket){.key = 5,  .value = 2});
    insert(map, (struct bucket){.key = 25, .value = 3});

    // lookups
    printf("Search for 5 looks right and gives %d \n",lookup(map, 5));
    assert(lookup(map, 15) == 1);
    assert(lookup(map, 5)  == 2);
    assert(lookup(map, 25) == 3);
    printf("✅ Insert + lookup passed\n");

    // overwrite value
    insert(map, (struct bucket){.key = 15, .value = 99});
    assert(lookup(map, 15) == 99);
    printf("✅ Overwrite passed\n");

    // many insert 
    for(int i = 0 ; i < 100 ; i++){
        insert(map,(struct bucket){.key = (i+1) ,.value = (i+1)*100 });
    }
    for(int i = 0 ; i < 100 ; i++){
        assert(lookup(map, (i+1)) == (i+1)*100);
    }
    printf("✅ Many insert passed\n");
    // delete key
    delete(map, 15);
    assert(lookup(map, 15) == -1); // assuming -1 means "not found"
    printf("✅ Delete passed\n");

    // lookup non-existing key
    int missing = -50;
    assert(lookup(map, missing) == -1);
    printf("✅ Missing key lookup passed\n");

    // multiple deletions
    delete(map, 5);
    delete(map, 25);
    assert(lookup(map, 5)  == -1);
    assert(lookup(map, 25) == -1);
    printf("✅ Multiple deletes passed\n");

    printf("🎉 All tests passed!\n");
defer:
    free(map->data);
    free(map);
    return 0;
}
