#include "template_hmap.h"
#define TEMPPLATE_HMAP_IMPL

#include <time.h>
#include <assert.h>

TEMPLATE_DEFINE_HASHMAP(IntIntMap,IntInt,int,int);

#define get_diff(t1,t2) (double)((t2)-(t1))/CLOCKS_PER_SEC

int main(){
    const size_t insertion_count = 500*1000;
    struct IntIntMap map;
    HMAP_CREATE(IntIntMap,&map);
    clock_t start_time,end_time;
    // testing the insertion speed 
    start_time = clock();
    for(size_t i = 0 ; i < insertion_count ; i++){
        HMAP_INSERT(IntIntMap,&map,((struct IntInt){.key = i+1,.value = (i+1)*2}));
    }
    end_time = clock();
    printf("Time taken to insert %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));
    // testing the lookup speed 
    start_time = clock();
    for(size_t i = 0 ; i < insertion_count ; i++){
        assert(*HMAP_LOOKUP(IntIntMap,&map,i+1) == (i+1)*2);
    }
    end_time = clock();
    printf("Time taken to lookup %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));
    // testing the deleting speed 
    start_time = clock();
    for(size_t i = 0 ; i < insertion_count ; i++){
        HMAP_DELETE(IntIntMap,&map,i+1);
    }
    end_time = clock();
    printf("Time taken to delete %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));

defer:
    free(map.data);
    return 0;
}
