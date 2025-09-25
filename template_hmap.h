#ifndef TEMPLATE_HMAP_H
#define TEMPLATE_HMAP_H

#include "ouroc.h"
#ifndef OUROC_IMPLI
#define OUROC_IMPLI
#endif
#include <stdint.h>


/*
TEMPLATE_HMAP.H

The whole idea for implimenting a hash map in this file is to basicly define a **protocol** for the data structures that work together. then utilize macros to **have them write the boilerplate code**.
We have a template macro that **declares and defined** all of the following for each template hash map you define:
- pair struct
- hmap struct
- insert / delete / lookup / rehash function.
(check source code for refrence).

 */

#define INIT_HASHMAP_CAPACITY 512 

uint32_t jenkins_one_at_a_time_hash(void* key,size_t length);

#define TEMPLATE_DEFINE_HASHMAP(name_of_hash_map,name_of_pair,T,U)                          \
    struct name_of_pair {                                                                   \
        T key;                                                                              \
        U value;                                                                            \
        bool dead;                                                                          \
    };                                                                                      \
    struct name_of_hash_map {                                                               \
        struct name_of_pair* data;                                                          \
        size_t capacity;                                                                    \
    };                                                                                      \
    void name_of_hash_map##_create(struct name_of_hash_map*map);                            \
    void name_of_hash_map##_rehash(struct name_of_hash_map* map);                           \
    void name_of_hash_map##_insert(struct name_of_hash_map* map,struct name_of_pair value); \
    U* name_of_hash_map##_lookup(struct name_of_hash_map* map,T key);                       \
    void name_of_hash_map##_delete(struct name_of_hash_map*map,T key);                      \
    void name_of_hash_map##_create(struct name_of_hash_map*map){                            \
        map->data = malloc(INIT_HASHMAP_CAPACITY*sizeof(struct name_of_pair));              \
        map->capacity = INIT_HASHMAP_CAPACITY;                                              \
        for (size_t i = 0; i < map->capacity; ++i) map->data[i].dead = true;                \
    }                                                                                       \
    void name_of_hash_map##_rehash(struct name_of_hash_map* map){                           \
        struct name_of_pair* old_data = map->data;                                          \
        size_t old_size = map->capacity;                                                    \
        map->capacity *= 2;                                                                 \
        map->data = malloc(sizeof(struct name_of_pair)*map->capacity);                      \
        for(size_t i = 0 ; i < map->capacity ; ++i) map->data[i].dead = true;               \
        /*copy back the old_data into the new data*/                                        \
        for(size_t j = 0 ; j < old_size ; ++j){                                             \
            name_of_hash_map##_insert(map,old_data[j]);                                     \
        }                                                                                   \
        free(old_data);                                                                     \
    }                                                                                       \
    void name_of_hash_map##_insert(struct name_of_hash_map* map,struct name_of_pair value){ \
        value.dead = false;                                                                 \
        uint32_t index = jenkins_one_at_a_time_hash(&(value.key),sizeof(value.key)) % map->capacity;\
        uint32_t saved = index;                                                             \
        while(!(map->data[index].key == value.key) && !map->data[index].dead){              \
            ++index;                                                                        \
            index %= map->capacity;                                                         \
            if(index == saved){                                                             \
                name_of_hash_map##_rehash(map);                                             \
                name_of_hash_map##_insert(map,value);                                       \
                return;                                                                     \
            }                                                                               \
        }                                                                                   \
        map->data[index] = value;                                                           \
    }                                                                                       \
    U* name_of_hash_map##_lookup(struct name_of_hash_map* map,T key){                       \
        uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;      \
        uint32_t i = index;                                                                 \
        while(map->data[index].dead == false && map->data[index].key != key){               \
            ++index;                                                                        \
            index %= map->capacity;                                                         \
            if(i == index){                                                                 \
                return NULL;                                                                \
            }                                                                               \
        }                                                                                   \
        if(map->data[index].dead == false) return &map->data[index].value;                  \
        else return NULL;                                                                   \
    }                                                                                       \
    void name_of_hash_map##_delete(struct name_of_hash_map*map,T key){                      \
        uint32_t index = jenkins_one_at_a_time_hash(&key,sizeof(key)) % map->capacity;      \
        uint32_t i = index;                                                                 \
        while(map->data[index].key != key){                                                 \
            ++index;                                                                        \
            index %= map->capacity;                                                         \
            if(i == index) return; /*it wasnt even there*/                                  \
        }                                                                                   \
        map->data[index].dead = true;                                                       \
    }


#define HMAP_CREATE(name_of_hash_map,map_p) name_of_hash_map##_create(map_p)
#define HMAP_REHASH(name_of_hash_map,map_p) name_of_hash_map##_rehash(map_p)
#define HMAP_INSERT(name_of_hash_map,map_p,value) name_of_hash_map##_insert(map_p,value)
#define HMAP_LOOKUP(name_of_hash_map,map_p,key) name_of_hash_map##_lookup(map_p,key)
#define HMAP_DELETE(name_of_hash_map,map_p,key) name_of_hash_map##_delete(map_p,key)


#endif
#ifndef TEMPLATE_HMAP_IMPL
#define TEMPLATE_HMAP_IMPL

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

#endif
