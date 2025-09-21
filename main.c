#include "ouroc.h"
#define OUROC_IMPLI
#include <stdint.h>

#define INIT_HASHMAP_CAPACITY 2

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
    while(map->data[index].dead != true) ++index;
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

#define TEMPLATE_KV(T,U,template_name)      \
    struct template_name {                  \
        T key;                              \
        U value;                            \
        bool dead;                          \
        void* (*alloc)(size_t);             \
        bool (*equal)(int,int);             \
        void (*free)(void*);                \
    }


#define DEC_KV(pair_name,name,k,v)      \
    struct pair_name name = {           \
            .key = (k),                 \
            .value = (v),               \
            .alloc = NULL,              \
            .free = NULL                \
    }

#define DEL_KV(kv)                      \
do{                                     \
    if((kv)->free != NULL)              \
        (kv)->free((kv));               \
}while(0)



#define DEF_HMAP(hmap,bucket)   \
struct hmap {                   \
    struct bucket* data;        \
    size_t capacity;            \
}

#define INIT_HMAP(hmap,bucket)                                                    \
    (hmap)->capacity = INIT_HASHMAP_CAPACITY;                                     \
    (hmap)->data = malloc(INIT_HASHMAP_CAPACITY*sizeof(struct bucket));           \
    for(size_t i = 0 ; i < (hmap)->capacity ; ++i) (hmap)->data[i].dead = true;   \

#define DEL_HMAP(hmap)                                              \
    for(size_t i = 0 ; i < (hmap)->capacity ; ++i){                 \
        if((hmap)->data[i].dead == false) DEL_KV(&(hmap)->data[i]); \
    }                                                               \
    free((hmap)->data)

// Usage: defining the templates used
TEMPLATE_KV(int,int,int_pair);
DEF_HMAP(int_map,int_pair);

int main(void){
    struct int_map test;
    INIT_HMAP(&test,int_pair);
    DEL_HMAP(&test);

    return 0;
    // init <done>
    struct hmap* map = malloc(sizeof(struct hmap));
    map->capacity = INIT_HASHMAP_CAPACITY;
    map->data = malloc(INIT_HASHMAP_CAPACITY*sizeof(struct bucket));
    for(int i = 0 ; i < INIT_HASHMAP_CAPACITY ; ++i) map->data[i].dead = true;
    // insert 
    insert(map,(struct bucket){.key = 15,.value = 1});
    insert(map,(struct bucket){.key = 5,.value = 2});
    // delete 
    delete(map,15);


    int key = 15;

    printf("Look up for %d is %d\n",key,lookup(map,key));

defer:
    free(map->data);
    free(map);
    return 0;
}
