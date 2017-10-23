/**
 * @filename :
 * 		remove_if.c
 * @note :
 * 
 * 
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    struct node * next;  
    void *data;
} node;  
 
typedef int (* remove_fn)(node const * v);  

static node * g_head_node = NULL;

static void add_item(node ** head, node * item)
{
	if (item == NULL) {
		return ;
	}
	
	printf("%s\n", __func__);
	item->next = *head;
	*head = item;
} 

static void show_items(node ** head)
{
	node *curr = *head;
	int i = 0;
	
	while (curr != NULL) {
		curr = curr->next;
		i++;
		printf("%d\n", i);
	}
}

static int remove_all(node const * v)
{
	return 1;
}

// Remove all nodes from the supplied list for which the   
// supplied remove function returns true.  
// Returns the new head of the list.  
static void remove_if_0(node ** head, remove_fn rm)  
{  	
	node * curr = *head;
	
    for (node * prev = NULL; curr != NULL; ) {  
        node * next = curr->next;  
        if (rm(curr)) {  
            if (prev) {
                prev->next = curr->next;  
            } else {  
                *head = curr->next;
			}  
			printf("%s\n", __func__);
            free(curr);  
        } else {  
            prev = curr;
		}  
        curr = next;  
    }      
}

static void remove_if_1(node ** head, remove_fn rm)  
{  
    for (node ** curr = head; *curr; ) {  
        node * entry = *curr;  
        if (rm(entry)) {  
            printf("%s\n", __func__);
            *curr = entry->next;  
            free(entry);  
        } else {  
            curr = &entry->next;
		}  
    }  
}

int main(int argc, char *argv[])
{
	node *item;
	
	/*1*/
	item = calloc(1, sizeof(node));
	add_item(&g_head_node, item);

	/*2*/
	item = calloc(1, sizeof(node));
	add_item(&g_head_node, item);
	
	show_items(&g_head_node);
	
	remove_if_0(&g_head_node, remove_all);
	if (g_head_node == NULL) {
		printf("Remove All OK!\n");
	} else {
		printf("Remove All Fail!\n");
	}
	
	printf("\n");
	
	/*1*/
	item = calloc(1, sizeof(node));
	add_item(&g_head_node, item);

	/*2*/
	item = calloc(1, sizeof(node));
	add_item(&g_head_node, item);
	
	show_items(&g_head_node);
	
	remove_if_1(&g_head_node, remove_all);
	if (g_head_node == NULL) {
		printf("Remove All OK!\n");
	} else {
		printf("Remove All Fail!\n");
	}	
			
	return 0;
}
