#include "ouroc.h"
#define OUROC_IMPLI

#define FILE_TO_BUILD "speed_test.c"
#define STL_FILE "stl_speed_test.cpp"
#define SOLUTION_BIN "main"

int main(int argc,char** argv){
    OUROC(main,SOLUTION_BIN,FILE_TO_BUILD,"template_hmap.h","build.c");
    OUROC_BUILD_CMD(&main,"clang -Wall -Wextra -fsanitize=address -O3",FILE_TO_BUILD,"-o",SOLUTION_BIN);

    OUROC(run,NULL,SOLUTION_BIN);
    OUROC_BUILD_CMD(&run,"./"SOLUTION_BIN);

    OUROC(stl,"stl",STL_FILE,"build.c");
    OUROC_BUILD_CMD(&stl,"g++ -Wall -Wextra -fsanitize=address",STL_FILE,"-o","stl");

    OUROC(run_stl,NULL,"stl");
    OUROC_BUILD_CMD(&run_stl,"./stl");


    ouroc_run_cmd(&main);
    ouroc_run_cmd(&stl);
    ouroc_run_cmd(&run);
    ouroc_run_cmd(&run_stl);

    OUROC_KILL(&main);
    OUROC_KILL(&stl);
    OUROC_KILL(&run_stl);
    OUROC_KILL(&run);
    return 0;
}
