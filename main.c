#include "ouroc.h"
#define OUROC_IMPLI
#include <stdint.h>
#include <assert.h>

#define INIT_HASHMAP_CAPACITY 512

uint32_t jenkins_one_at_a_time_hash(void* key,size_t length){
    const uint8_t* bytes = (const uint8_t*)key;
    size_t i = 0;
    uint32_t hash = 0;
    while(i != length){
        hash += bytes[i++];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}

struct bucket {
    int key;
    int value;
    bool dead;
};

struct hmap {
    struct bucket* data;
    size_t capacity;
};


void insert(struct hmap* map,struct bucket value){
    value.dead = false;
    uint32_t index = jenkins_one_at_a_time_hash(&(value.key),sizeof(value.key)) % map->capacity;
    printf("%u\n",index);
    while(index < map->capacity && !(map->data[index].key == value.key) && map->data[index].dead != true) ++index;
    map->data[index] = value;
}

int lookup(struct hmap* map,int key){
    uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;
    printf("%u\n",index);
    uint32_t i = index;
    while(map->data[index].key != key){
        ++index;
        index %= map->capacity;
        if(i == index) return -1;
    }
    if(!map->data[index].dead)return map->data[index].value;
    else return -1;
}

void delete(struct hmap*map,int key){
    uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;
    printf("%u\n",index);
    uint32_t i = index;
    while(map->data[index].key != key){
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



int main(void){
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
    assert(lookup(map, 15) == 1);
    assert(lookup(map, 5)  == 2);
    assert(lookup(map, 25) == 3);
    printf("✅ Insert + lookup passed\n");

    // overwrite value
    insert(map, (struct bucket){.key = 15, .value = 99});
    assert(lookup(map, 15) == 99);
    printf("✅ Overwrite passed\n");

    // delete key
    delete(map, 15);
    assert(lookup(map, 15) == -1); // assuming -1 means "not found"
    printf("✅ Delete passed\n");

    // lookup non-existing key
    int missing = 42;
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
