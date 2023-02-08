#include "reactphysics3d/reactphysics3d.h"
#include <stdio.h>

void PrintSome();

int main(void){
    printf("Start\n");
    
    rp3d::PhysicsCommon* common = new rp3d::PhysicsCommon();       
    
    printf("End %p\n",common);
}