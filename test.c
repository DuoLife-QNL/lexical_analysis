#include<stdio.h>

const int CNT_NUM = 5;

int as$aa;

int main(){
    int a[CNT_NUM];

    for (int i = 0; i < CNT_NUM; i++){
        a[i] = i;
    }
    for (int i = 0; i < CNT_NUM - 1; i++){
        printf("%d\"test\"\t", a[i]);
    }
    printf("%d\n", a[CNT_NUM - 1]);
}//mark

/*sdfjasdif*/