/**
 *	@filename :
 * 		test_so.c
 * 
 * 	@note :
 * 		1).没有调用execl之前， 子进程和父进程共享内存，如果子进程有修改，才会分配新的内存(copy-and-write).
 *      2).如果调用execl, 子进程重新申请内存, 子进程不会共享父进程的内存.
 * 		 (1). 没有调用libtest_so1.so里的函数，libtest_so1.so没有被加载到进程空间(通过命令行cat /proc/xxxx/maps).
		10367 libtest_init: 1
		10367 : test_init
		parent
		10367 : cnt = 1
		child
		10368 : cnt = 2
		10367 : cnt = 1
		10367 : test_uninit
		10367 libtest_uninit: 0

 * 		 (2). 调用libtest_so1.so里的函数, 重新加载libtest_so1.so.
		10509 libtest_init: 1
		10509 : test_init
		parent
		10509 : cnt = 1
		child
		10510 : cnt = 2
		10510 libtest_init: 1
		test_so1_1 : 1
		10510 libtest_uninit: 0
		10509 : cnt = 1
		10509 : test_uninit
		10509 libtest_uninit: 0
 
 * 		3).如果没有调用execl，子进程和父进程退出的时候，都会调用析构函数; 
  		9950 libtest_init: 1
		9950 : test_init
		parent
		9950 : cnt = 1
		child
		9951 : cnt = 2
		9951 : test_uninit
		9951 libtest_uninit: 1
		9950 : cnt = 1
		9950 : test_uninit
		9950 libtest_uninit: 0

 * 
 *      4).如果调用了execl，那么子进程退出时，就不会调用test_uninit(内存空间没有共享); 如果子进程加载了libtest_so1.so, 那么
 * 			libtest_so1.so中的析构函数也会被调用.
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "lib_so.h"

static __attribute__((constructor)) void test_init(void);
static __attribute__((destructor)) void test_uninit(void);

static void test_init(void) {
	printf("%d : %s\n", getpid(), __FUNCTION__);
}

static void test_uninit(void) {
	printf("%d : %s\n", getpid(), __FUNCTION__);
}

int main(int argc, char *argv[]) {		
	pid_t pid;
	int ret = 0;
	
	pid = fork();
	if (pid < 0) {
		printf("fork error\n");
		return -1;
		
	} else if (pid == 0) {
		printf("child\n");
		printf("%d : cnt = %d\n", getpid(), libtest_inc_cnt());
		ret = execl("./bin/test_so1_1", "test_so1_1", NULL);		
	} else {
		printf("parent\n");
		printf("%d : cnt = %d\n", getpid(), libtest_get_cnt());
		waitpid(pid, NULL, 0);
		printf("%d : cnt = %d\n", getpid(), libtest_get_cnt());
	}
	
	return ret;
}
