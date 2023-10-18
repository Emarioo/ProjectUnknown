#include <stdio.h>

#include "Engone/Logger.h"

#include "Engone/PlatformModule/PlatformLayer.h"

int main(){
    
    // using namespace engone;
    
    // APIFile* file = FileOpen("file.txt");
    
    // Memory<char> test;
    
    // test.resize(20);
    
    // printf("%d %d\n",test.max, test.used);
    
    // FileReader reader("file.txt");
    // FileWriter writer("file.txt");

    void* ptr = engone::Allocate(25);

    int num = 92;
    printf("right");
    // log::out << "Hello "<<log::BLUE<<"It works!"<<"\n";
    // log::out.useThreadReports(true);
    // log::out.setReport("other");
    // log::out << log::PURPLE<<"Sweet I say! "<<num<<"\n";
    
    // while(1){
    //     int temp[6]{5,9,6,4};
    //     // bool yes = reader.read(temp,2);
    //     bool yes = writer.write(temp,2);
    //     if(!yes)
    //         break;
        
    //     for(int i=0;i<6;i++)
    //         printf("eh %d, ",temp[i]);
    //     printf("\n");
    //     break;
    // }
    // // printf("Final Error: %s\n",ToString(reader.getError()));
    // printf("Final Error: %s\n",ToString(writer.getError()));
    
    
    // printf("okay %p\n",file);
}