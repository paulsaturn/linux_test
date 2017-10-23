#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

/*
 */
#define wrap(a) __wrap_ ## a

struct list_head {
    struct list_head *prev;
    struct list_head *next;    
};

struct list_node {
    struct list_head *prev;
    struct list_head *next;    
    size_t size;
    size_t addr;
};

static struct list_head g_malloc_head = {&g_malloc_head, &g_malloc_head};

#define MAX_SIZE_T           (~(size_t)0)
#define SIZE_T_SIZE			sizeof(size_t)
#define MEM_SIZE_T_OFFSET	(4)
#define MEM_OFFSET			(SIZE_T_SIZE*MEM_SIZE_T_OFFSET)

static size_t g_mem_used_size = 0;
static int g_mem_debug = 0;

static inline size_t _get_call_addr(void)
{
	volatile int *p;
#if defined(__arm__)	
    __asm__(
        "mov %0, lr\n"
        : "=r"(p)
    );
	return (size_t)p - SIZE_T_SIZE;
#elif defined(__mips__)	
    __asm__(
        "move %0, $ra\n"
        : "=r"(p)
    );
    return (size_t)p - MEM_OFFSET;
#elif defined(__x86_64__)
    __asm__ volatile(
        "mov 8(%%rbp), %0\n"
        : "=r"(p)
    );
    return (size_t)p;
#else	//__i386__
    __asm__ volatile(
        "mov 4(%%ebp), %0\n"
        : "=r"(p)
    );
    return (size_t)p;
#endif    
}
 
/*
 */
void *__real_malloc(size_t size);
void *wrap(malloc)(size_t size)
{	
	struct list_node *ptr;	
	    
	volatile size_t addr = _get_call_addr();

	size += MEM_OFFSET;
	ptr = (struct list_node *)__real_malloc(size);
	if (ptr != NULL) {
		if (g_mem_debug) {
			printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, addr);
		}		
		__sync_fetch_and_add(&g_mem_used_size, size);   
             
		ptr->size = size;	
		ptr->addr = addr;	
        ptr->prev = &g_malloc_head;
        ptr->next = g_malloc_head.next;
        g_malloc_head.next = (struct list_head *)ptr;
        
		ptr++;
	}
	
	return (void *)ptr;
}

void __real_free(void *ptr);
void wrap(free)(void *ptr)
{
	size_t size;
	struct list_node *p;
	
	if (ptr == NULL) {
		return;
	}
	
	p = (struct list_node *)ptr;
	p--;
	size = p->size;
	if (g_mem_debug) {
		printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, p->addr);
	}	
	__sync_fetch_and_sub(&g_mem_used_size, size);
    
	p->next->prev = p->prev;
    p->prev->next = p->next;
    
	__real_free((void *)p);
}

void *wrap(realloc)(void *ptr, size_t size)
{
	void *new_ptr = NULL;
	size_t old_size;
	
	if (size == 0) {
		__wrap_free(ptr);
		return NULL;
	} 
	
	volatile size_t addr = _get_call_addr();
		
	size_t req_size = size + MEM_OFFSET;
	new_ptr = __real_malloc(req_size);
	if (new_ptr != NULL) {
		if (g_mem_debug) {
			printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, addr);
		}		
		__sync_fetch_and_add(&g_mem_used_size, req_size);
        
        struct list_node *p = (struct list_node *)new_ptr;
        
		p->size = size;	
		p->addr = addr;	
        p->prev = &g_malloc_head;
        p->next = g_malloc_head.next;       
        g_malloc_head.next = (struct list_head *)p; 
        
		new_ptr = (char *)new_ptr + MEM_OFFSET;		
		
		if (ptr != NULL) {
			old_size = *((size_t *)ptr - 1) - MEM_OFFSET;
			if (old_size > size) {
				old_size = size;
			}
			memcpy(new_ptr, ptr, old_size);
		}
		wrap(free)(ptr);
	}
	
	return new_ptr;	
}

void* wrap(calloc)(size_t n_elements, size_t elem_size) {
	void* mem;
	size_t req = 0;
	
	if (n_elements != 0) {
		req = n_elements * elem_size;
		if (((n_elements | elem_size) & ~(size_t)0xffff) &&
			(req / n_elements != elem_size)) {
			req = MAX_SIZE_T; /* force downstream failure on overflow */
		}
	}
	
	//printf("__wrap_calloc\n");
	volatile size_t addr = _get_call_addr();
	
	size_t req_size = req + MEM_OFFSET;
	mem = __real_malloc(req_size);
	if (mem != NULL) {		
		if (g_mem_debug) {
			printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, addr);
		}		
		__sync_fetch_and_add(&g_mem_used_size, req_size);
        
        struct list_node *p = (struct list_node *)mem;
		p->size = req_size;	
		p->addr = addr;	
        p->prev = &g_malloc_head;
        p->next = g_malloc_head.next;       
        g_malloc_head.next = (struct list_head *)p; 
        
		mem = (char *)mem + MEM_OFFSET;		

		memset(mem, 0, req);
	}
	
	return mem;	
}

#if defined(__x86_64__)
void *wrap(_Znwm)(size_t size)
#else
void *wrap(_Znwj)(size_t size)
#endif
{
	size_t *ptr;	
	    
	volatile size_t addr = _get_call_addr();

	size += MEM_OFFSET;
	ptr = (size_t *)__real_malloc(size);
	if (ptr != NULL) {
		if (g_mem_debug) {
			printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, addr);    
		}		
		__sync_fetch_and_add(&g_mem_used_size, size);
        
        struct list_node *p = (struct list_node *)ptr;
		p->size = size;	
		p->addr = addr;	
        p->prev = &g_malloc_head;
        p->next = g_malloc_head.next;       
        g_malloc_head.next = (struct list_head *)p; 
        
		ptr += MEM_SIZE_T_OFFSET;
	}
	
	return (void *)ptr;
}

void wrap(_ZdlPv)(void *ptr)
{	
	size_t size;
	struct list_node *p;
	
	if (ptr == NULL) {
		return;
	}
	
	p = (struct list_node *)ptr;
	p--;
	size = p->size;
	if (g_mem_debug) {
		printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, p->addr);
	}	
	__sync_fetch_and_sub(&g_mem_used_size, size);
    
	p->next->prev = p->prev;
    p->prev->next = p->next;
    	
	__real_free((void *)p);
}

#if defined(__x86_64__)
void *wrap(_Znam)(size_t size)
#else
void *wrap(_Znaj)(size_t size)
#endif
{
	size_t *ptr;	
	    
	volatile size_t addr = _get_call_addr();

	size += MEM_OFFSET;
	ptr = (size_t *)__real_malloc(size);
	if (ptr != NULL) {
		if (g_mem_debug) {
			printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, addr);
		}		
		__sync_fetch_and_add(&g_mem_used_size, size);
        
        struct list_node *p = (struct list_node *)ptr;
		p->size = size;	
		p->addr = addr;	
        p->prev = &g_malloc_head;
        p->next = g_malloc_head.next;       
        g_malloc_head.next = (struct list_head *)p;
        
		ptr += MEM_SIZE_T_OFFSET;
	}
	
	return (void *)ptr;
}

void wrap(_ZdaPv)(void *ptr)
{
	size_t size;
	struct list_node *p;
	
	if (ptr == NULL) {
		return;
	}
	
	p = (struct list_node *)ptr;
	p--;
	size = p->size;
	if (g_mem_debug) {
		printf("%s 0x%"__PRIPTR_PREFIX"x\n", __func__, p->addr);
	}	
	__sync_fetch_and_sub(&g_mem_used_size, size);

	p->next->prev = p->prev;
    p->prev->next = p->next;
    	
	__real_free((void *)p);
}

char *wrap(strndup)(const char *src, size_t len)
{
	char* dst = (char*) wrap(malloc)(len + 1);
	if (dst) {
		strncpy(dst, src, len + 1);
	}
	
	return dst; 
}

char *wrap(__strdup)(const char *src)
{
	size_t len = strlen(src);
	
	return wrap(strndup)(src, len); 
}

char *wrap(strdup)(const char *src)
{
	size_t len = strlen(src);
	
	return wrap(strndup)(src, len); 
}

static __attribute__((destructor(101))) void wrap_malloc_exit( void );  // 0-100(包括100),是内部保留的, 101开始
static __attribute__((constructor)) void wrap_malloc_enter( void );

static void wrap_malloc_enter(void)
{
	printf("--> %s\n", __func__);
}

static void wrap_malloc_exit(void) 
{
	printf("--> %s : memory used size = 0x%"__PRIPTR_PREFIX"x\n", __func__, g_mem_used_size);
    if (g_mem_used_size != 0) {
        struct list_node *p;
        p = (struct list_node *)g_malloc_head.next;
        while (p != (struct list_node *)&g_malloc_head) {
            printf("0x%"__PRIPTR_PREFIX"x addr = 0x%"__PRIPTR_PREFIX"x\n", p->size, p->addr);
            p = (struct list_node *)p->next;
        }
    }
}

void wrap_malloc_info(void) 
{
	printf("memory used size = 0x%"__PRIPTR_PREFIX"x\n", g_mem_used_size);
}

void wrap_malloc_debug(int enable) 
{
	g_mem_debug = enable;
}
