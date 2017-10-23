/**
 *  @filename :
 *      test_data0.c
 *
 *  @note :
 *    结构体中data[0], 不占空间
 *    结构体中data[], 不占空间
 **/
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define ITME_DATA_LEN   16

typedef struct {
    int num;
    char data[0];
}item_t;

typedef struct {
    int num;
    union {
        uint64_t cas;   
        // uint32_t cas;
        char end;
    } data[];
}item1_t;

int main(int argc, char *argv[])
{
    item_t *pitem;
    item1_t *pitem1;

    printf("sizeof(uint32_t) = %d\n", sizeof(uint32_t));
    printf("sizeof(uint64_t) = %d\n", sizeof(uint64_t));
    printf("\n");
    printf("sizeof(item_t) = %d\n", sizeof(item_t));    //sizeof(item) = 4
    printf("offsetof(data) = %d\n", offsetof(item_t, data));
    pitem = (item_t *)malloc(sizeof(item_t) + ITME_DATA_LEN);
    if (pitem) {
        // pitem->data = 0;        //compiler error
        pitem->data[0] = 0;
        pitem->num = ITME_DATA_LEN;        
        sprintf((void *)(pitem+1), "hello world\n");
        printf("sizeof(data) = %d\n", sizeof(pitem->data)); //sizeof(pitem->data) = 0
        printf("strlen(data) = %d\n", strlen(pitem->data)); //12
        printf("%s\n", pitem->data);   //hello world         

        free(pitem);
    }

    printf("sizeof(item1_t) = %d\n", sizeof(item1_t));    //sizeof(item1) = 8 (uint64_t对齐) or 4 (uint32_t对齐)
    printf("offsetof(data) = %d\n", offsetof(item1_t, data));
    pitem1 = (item1_t *)malloc(sizeof(item1_t) + ITME_DATA_LEN);
    if (pitem1) {
        pitem1->num = ITME_DATA_LEN;        
        sprintf((void *)(pitem1+1), "hello world\n");
        printf("%s", pitem1->data);   //hello world         
        printf("%s\n", (char *)&((pitem1)->data));   //hello world

        free(pitem1);
    }

    return 0;
}