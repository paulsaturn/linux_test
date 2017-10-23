/**
   @filename :
   		test_buffer.c
**/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include <pthread.h>

#define BUF_USER_ID 	0x00
#define BUF_SERVER_ID	0x01

#define BUF_FRAME_COUNT 4096
#define BUF_FRAME_SIZE  0x01

#define BUF_FRAME_REQ	1600
#define BUF_FRAME_REQ_S	2048

#define DELAY_C			20
#define DELAY_S			100

#include "ringbuffer.c"        

/**
 **/
static pthread_mutex_t g_lock;
static pthread_cond_t g_cond;
/**
 **/
static rbuf_cblk_t g_buffer_cblk;
static int g_client_runnig = 1;

static void *thread_client(void *pdata) {
	rbuf_cblk_t *pcblk = (rbuf_cblk_t *)pdata;
	uint32_t frames_avail;
	uint32_t frames_req;
	void *ptr1;
	uint32_t size1;
	void *ptr2;
	uint32_t size2;	
	
	while (g_client_runnig) {
	    pthread_mutex_lock(&g_lock);
		frames_avail = rbuf_write_available(pcblk);
		if (frames_avail == 0) {
			printf("[c]wait server\n");
			pthread_cond_wait(&g_cond, &g_lock);
			
			if (g_client_runnig == 0) {
				break;
			}			
			frames_avail = rbuf_write_available(pcblk);			
		}
		pthread_mutex_unlock(&g_lock);

		frames_req = BUF_FRAME_REQ;
		if (frames_req > frames_avail) {
			frames_req = frames_avail;
		}
		
		frames_req = rbuf_get_write_buffer(pcblk, frames_req, &ptr1, &size1, &ptr2, &size2);
		if (frames_req > 0) {
			printf("[c]frames_req = 0x%x\n", frames_req);
		#ifdef __RBUF_FRAME_SUPPORT__
			size1 = size1 * pcblk->frame_size;
			if (size2 != 0) {
				size2 = size2 * pcblk->frame_size;					
			}
		#endif	
			printf("[c]buf1 = 0x%x, size1 = 0x%x, buf2 = 0x%x, size2 = 0x%x\n", (unsigned int)ptr1, size1, (unsigned int)ptr2, size2);		
			memset(ptr1, 0, size1);
			if (size2 != 0) {
				memset(ptr2, 0, size2);				
			}			
			rbuf_advance_write_index(pcblk, frames_req);		
		}
				
		usleep(DELAY_C*1000);
	}
	
	return NULL;
}

/**
 **/
int main(int argc, char *argv[]) {
	pthread_t tid;
	int is_running = 1;
	uint32_t frames_rdy;
	uint32_t frames_req;
	rbuf_cblk_t *pcblk = &g_buffer_cblk;
	void *ptr1;
	uint32_t size1;
	void *ptr2;
	uint32_t size2;	
	uint32_t delay = -1;
	uint32_t cnt;
    char *buf = NULL;

	if (argc > 1) {
		delay = atoi(argv[1]);
	}
	
    pthread_mutex_init(&g_lock, NULL);
    pthread_cond_init(&g_cond, NULL);
    
	buf = malloc(BUF_FRAME_COUNT*BUF_FRAME_SIZE);

#ifdef __RBUF_FRAME_SUPPORT__
	rbuf_init(pcblk, buf, BUF_FRAME_COUNT, BUF_FRAME_SIZE);
#else	
    rbuf_init(pcblk, buf, BUF_FRAME_COUNT*BUF_FRAME_SIZE);
#endif    

	printf("[init] buf = 0x%x, end = 0x%x\n", (unsigned int)pcblk->buffer, (unsigned int)pcblk->buffer + BUF_FRAME_SIZE*BUF_FRAME_COUNT);

	pthread_create(&tid, NULL, thread_client, pcblk);

	cnt = 0;
	while (is_running && g_client_runnig) {
		frames_rdy = rbuf_read_available(pcblk);        
		if (frames_rdy == 0) {
            printf("[s]wait client\n");
			usleep(100);
			continue;
		}

        frames_req = rand() & (BUF_FRAME_REQ_S - 1);
        if (frames_req == 0) {            
		  frames_req = BUF_FRAME_REQ_S;
        }
		if (frames_req > frames_rdy) {
			frames_req = frames_rdy;
		}
		
		frames_req = rbuf_get_read_buffer(pcblk, frames_req, &ptr1, &size1, &ptr2, &size2);
		if (frames_req > 0) {
			printf("[s]frames_req = 0x%x\n", frames_req);
			printf("[s]buf1 = 0x%x, size1 = 0x%x, buf2 = 0x%x, size2 = 0x%x\n", (unsigned int)ptr1, size1, (unsigned int)ptr2, size2);			
            
            pthread_mutex_lock(&g_lock);            
            
			rbuf_advance_read_index(pcblk, frames_req);
            
            pthread_cond_signal(&g_cond);	
            pthread_mutex_unlock(&g_lock);
		}
		
		usleep(DELAY_S*1000);
		cnt++;
		if (cnt >= (1000/DELAY_S)) {
			cnt = 0;
			delay--;
			if (delay == 0) {
				printf("exit main loop\n");
				is_running = 0;
			}
		}
	}

	g_client_runnig = 0;
	pthread_cond_signal(&g_cond);
	pthread_join(tid, NULL);

	pthread_cond_destroy(&g_cond);
	pthread_mutex_destroy(&g_lock);

	if (buf) {
		free(buf);
		buf = NULL;
	}
    
    return 0;
}


