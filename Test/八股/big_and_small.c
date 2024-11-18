#include<stdio.h>


union big_small{
    int i;
    char c[2];
};


int main(){
    union big_small bs;
    bs.i = 1;

    if(bs.c[0] == 1){
        printf("big\n");
    }
    else{
        printf("small\n");
    }
}