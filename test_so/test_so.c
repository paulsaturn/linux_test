/**
 *	@filename :
 * 		test_so.c
 * 
 * 	@note :
 * 		符号先在本地查找解决.
 *      1.不同的情况下的malloc_stats输出的结果有些差异，这个和加载库有关(查看/proc/xxx/maps), 内存没有泄漏
 * 
 **/
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <malloc.h>

#include "lib_so.h"

int g_test_cnt = 0;

int test_open(void *ptr) {
	g_test_cnt += 2;
	
	return g_test_cnt;
}

int main(int argc, char *argv[]) {
	char *lib_path = "./bin/libtest.so";
	void *handle = NULL;
	const char *sym = HAL_MODULE_INFO_SYM_STR;
	hal_module_info_t *hmi;
	char *error;
    int i = 2;
	
	if (argc > 1) {
		lib_path = argv[1];
	}
	printf("lib path : %s\n", lib_path);
    
    printf("\n");
    malloc_stats();
    
    while (i-- > 0) {
        printf("\n== load lib == \n");
        dlerror();		//clear error code
        //handle = dlopen(lib_path, RTLD_LAZY);
        handle = dlopen(lib_path, RTLD_NOW);
        if (handle == NULL) {
            fprintf(stderr, "%s\n", dlerror());		
            exit(1);
        }
        
        hmi = (hal_module_info_t *)dlsym(handle, sym);
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", dlerror());		
            exit(1);
        }
        
        //info
        printf("========= lib info =========\n");
        printf("%s : ver%d.%d\n", hmi->tagname, hmi->ver_major, hmi->ver_minor);
        printf("%d, %d\n", hmi->open(NULL), g_test_cnt);
        dlclose(handle);
        printf("== unload lib == \n");
    }
    printf("\n");
    malloc_stats();
    
    printf("exit all tests\n");
		
	return 0;
}
