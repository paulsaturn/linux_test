/**
 *	@filename :
 * 		libtest_so.cpp
 * 
 * 	@note :
 * 		在cpp文件中定义的全局变量，编译之后符号没有变化, 而定义的函数有变化.
 * 		(1). Displays the entries in symbol table section of the file :
 * 			readelf -s ./bin/libtest_so.so
 * 		(2). Displays the contents of the file's dynamic section :
 * 			readelf -d ./bin/libtest_so.so
 **/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "lib_so.h"

/*
 * 
 */
#define TAG_NAME		"hello world"

//#define __STATIC_EN__
class ClassB
{
public:
	 ClassB();
	 ~ClassB();
};

ClassB::ClassB() {
	 printf("class B\n");
}

ClassB::~ClassB() {
	 printf("~class B\n");
}

/*
 *
 */
int g_test_cnt = 0;
static ClassB g_class_b;

#if 1
class ClassA
{
public:
	 ClassA();
	 ~ClassA();
     void test();
};

ClassA::ClassA() {
	 printf("class A\n");
}

ClassA::~ClassA() {
	 printf("~class A\n");
}

void ClassA::test() {
    printf("test\n");
}

/* 
 (1).通过__attribute__((destructor)) 定义的函数来释放
 
 (2).定义
        static ClassA g_class_a;
    把
        g_class_a_ptr = new ClassA;
    改为
        g_class_a_ptr = &g_class_a;
    在库退出的时候调用g_class_a的析构函数
 */
//#define _CLASS_INIT_EN_
#ifdef _CLASS_INIT_EN_
static ClassA g_class_a;
#endif
static ClassA* g_class_a_ptr;

static __attribute__((destructor)) void _exit( void );
static void _exit(void)
{
    printf("---> exit <---\n");
    if (g_class_a_ptr != NULL) {
    #ifdef _CLASS_INIT_EN_
    #else
        delete g_class_a_ptr;
    #endif
        g_class_a_ptr = NULL;
    }
}

static ClassA& get_ref() {    
    if (NULL == g_class_a_ptr) {
    #ifdef _CLASS_INIT_EN_
        g_class_a_ptr = &g_class_a;     //在dlclose之后是调用g_class_a析构函数
    #else
        g_class_a_ptr = new ClassA;   //在dlclose之后是没有释放申请的空间, 没有调用析构函数, 可以通过上面的_exit()函数来释放
    #endif    
    }
    
    return *g_class_a_ptr;
}

#else

#include "strong_pointer.h"

template<typename T>
sp<T>::sp(T* other)
    : m_ptr(other)
{
    if (other) other->incStrong(this);
}

template<typename T>
sp<T>::sp(const sp<T>& other)
    : m_ptr(other.m_ptr)
{
    if (m_ptr) m_ptr->incStrong(this);
}

template<typename T>
sp<T>::~sp()
{
    if (m_ptr) m_ptr->decStrong(this);
}

class ClassA : public LightRefBase<ClassA>
{
public:
	 ClassA();
	 ~ClassA();
     void test();
};

ClassA::ClassA() {
	 printf("class A\n");
}

ClassA::~ClassA() {
	 printf("~class A\n");
}

void ClassA::test() {
    printf("test\n");
}

static ClassA& get_ref() {
    static ClassA* g_class_a_ptr;
    
    if (NULL == g_class_a_ptr) {
        g_class_a_ptr = new ClassA;
    }    
    
    static sp<ClassA> lpOut = g_class_a_ptr;    //类似于第一种方法的(2)
    
    return *lpOut.get();
}
#endif

/*
 * 
 */  
#ifdef __STATIC_EN__ 
static int test_open(void *ptr) {
#else
int test_open(void *ptr) {
#endif	
	g_test_cnt++;

    get_ref().test();
    
    char *ptr_str = strdup("hello");        //调用了wrap_malloc.c中的strdup
    if (ptr_str != NULL) {
        free(ptr_str);                      //调用了wrap_malloc.c中的free
    }

	return g_test_cnt;
}

/*
 * 
 */
#if 0
hal_module_info_t HAL_MODULE_INFO_SYM = {
	.tagname = TAG_NAME,
	.ver_major = 1,
	.ver_minor = 0,
	.open = test_open,
};

#else
hal_module_info_t HAL_MODULE_INFO_SYM = {
	tagname : TAG_NAME,
	ver_major : 1,
	ver_minor : 0,
	open : test_open,
};

#endif

