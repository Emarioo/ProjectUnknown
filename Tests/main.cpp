#include <stdio.h>
// #include "Engone/Utilities/Alloc.h"
#include "Engone/FileModule/FileReader.h"
// #include "Engone/PlatformModule/PlatformLayer.h"

int main(){
    
    using namespace engone;
    
    // APIFile* file = FileOpen("file.txt");
    
    // Memory<char> test;
    
    // test.resize(20);
    
    // printf("%d %d\n",test.max, test.used);
    
    FileReader reader("file.txt");
    
    while(1){
        int temp[6];
        bool yes = reader.read_int(temp,2);
        if(!yes)
            break;
        
        // for(int val : temp){
        //     printf("%d\n",*val);
        // }
        
        for(int i=0;i<6;i++)
            printf("eh %d, ",temp[i]);
        printf("\n");
    }
    printf("Final Error: %s\n",ToString(reader.getError()));
    
    
    // printf("okay %p\n",file);
}