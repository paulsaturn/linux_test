/**	
	@filename :
        test_strcmp.c
        
    @note :    
        ./bin/test_strcmp /etc/udev/rules.d
        输出是按开始数字从小到大排列
**/
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <dirent.h>   
#include <sys/types.h>

#include "list.h"    

#define NAME_SIZE       128

//
struct device {
    struct list_head node;
    char path[NAME_SIZE];
};

static LIST_HEAD(device_list);

static int add_device(char *name)
{
    struct device *loop_device;
    struct device *new_device;

    printf("insert: '%s'\n", name);

    list_for_each_entry(loop_device, &device_list, node) {
        if (strcmp(loop_device->path, name) > 0) {
            break;
        }
    }

    new_device = malloc(sizeof(struct device));
    if (new_device == NULL) {
        printf("error malloc");
        return -1;
    }            

    strcpy(new_device->path, name);
    list_add_tail(&new_device->node, &loop_device->node);
    printf("add '%s'\n" , new_device->path);

    return 0;
}

//
static int scan_devices(char *path)
{
    DIR *dir;
    struct dirent *dent;

    dir = opendir(path);
    if (dir != NULL) {    
        for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
            if (dent->d_name[0] == '.')
                continue;

            add_device(dent->d_name);
        }        
    }

    return 0;
}

static void output_devices(void)
{
    struct device *loop_device;
    int i = 0;

    printf("=============================================\n");
    list_for_each_entry(loop_device, &device_list, node) {  
        printf("[%02d] %s\n", i, loop_device->path);
        i++;
    }
}

static void cleanup_devices(void)
{
    struct device *loop_device;
    struct device *tmp_device;

    list_for_each_entry_safe(loop_device, tmp_device, &device_list, node) {
        list_del(&loop_device->node);
        free(loop_device);
    }    
}

//
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Error : please input dir!\n");
        return -1;
    }

    scan_devices(argv[1]);
    output_devices();
    cleanup_devices();

    return 0;
}    
