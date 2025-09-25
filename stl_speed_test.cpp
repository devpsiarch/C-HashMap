#include <iostream>
#include <unordered_map>

using namespace std;

#include <time.h>
#define get_diff(t1,t2) (double)((t2)-(t1))/CLOCKS_PER_SEC


int main(void){
    const size_t insertion_count = 500*1000;
    clock_t start_time,end_time;
    unordered_map<int,int> m;
    
    // testing the insertion speed 
    start_time = clock();
    for(size_t i = 0 ; i < insertion_count ; i++){
        m[i+1] = (i+1)*2;
    }
    end_time = clock();
    printf("Time taken to insert %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));
    // testing the lookup speed 
    start_time = clock();
    int v;
    for(size_t i = 0 ; i < insertion_count ; i++){
        v = m[i+1];
    }
    end_time = clock();
    printf("Time taken to lookup %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));
    // testing the deleting speed 
    start_time = clock();
    for(size_t i = 0 ; i < insertion_count ; i++){
        m[i+1] = -1;
    }
    end_time = clock();
    printf("Time taken to delete %zu elements: %lf.\n",insertion_count,get_diff(start_time,end_time));



    return 0;
}
