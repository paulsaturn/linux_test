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

/**
 **/
typedef struct {
	char *buffer;
	volatile uint32_t write_index;	/* 可以大于frame_count-1, 但小于frame_count*2-1 */
	volatile uint32_t read_index;	/* 可以大于frame_count-1, 但小于frame_count*2-1 */
	uint32_t big_mask;			/**< Used for wrapping indices with extra bit to distinguish full/empty. */
	uint32_t small_mask;		/**< Used for fitting indices to buffer. */
	uint32_t frame_count;
	uint32_t frame_size;
	pthread_mutex_t lock;
	pthread_cond_t cond;
}buffer_cblk_t;

/**
 **/
static buffer_cblk_t g_buffer_cblk;
static int g_client_runnig = 1;

/**
 **/
static void buffer_cblk_init(buffer_cblk_t *pcblk, char *pbuf, uint32_t frame_count, uint32_t frame_size)
{
    pcblk->buffer = pbuf;
    pcblk->read_index = 0;
    pcblk->write_index = 0;
    pcblk->frame_count = frame_count;
    pcblk->frame_size = frame_size;
    pcblk->small_mask = frame_count - 1;
    pcblk->big_mask = frame_count*2 - 1;
}

static uint32_t frames_read_available(buffer_cblk_t *pcblk) { //可读数
	return ((pcblk->write_index - pcblk->read_index) & pcblk->big_mask);
}

static uint32_t frames_write_available(buffer_cblk_t *pcblk) { //可写数
	return (pcblk->frame_count - frames_read_available(pcblk));
}

static uint32_t get_read_buffer(buffer_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2) 
{	
    uint32_t   index;
    uint32_t   available = frames_read_available( pcblk );

    if ( count > available ) {
    	count = available;
    }

    /* Check to see if read is not contiguous. */
    index = pcblk->read_index & pcblk->small_mask;
    if ( (index + count) > pcblk->frame_count ) {
        /* Write data in two blocks that wrap the buffer. */
        uint32_t first_half = pcblk->frame_count - index;

        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
        *psize1 = first_half;
        *pdata2 = (void *)&pcblk->buffer[0];
        *psize2 = count - first_half;
    } else {
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
        *psize1 = count;
        *pdata2 = NULL;
        *psize2 = 0;
    }	

    return count;
}


static uint32_t get_write_buffer(buffer_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2) 
{	
    uint32_t   index;
    uint32_t   available = frames_write_available( pcblk );

    if ( count > available ) {
    	count = available;
    }

    /* Check to see if write is not contiguous. */
    index = pcblk->write_index & pcblk->small_mask;
    if ( (index + count) > pcblk->frame_count ) {
        /* Write data in two blocks that wrap the buffer. */
        uint32_t   first_half = pcblk->frame_count - index;

        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
        *psize1 = first_half;
        *pdata2 = (void *)&pcblk->buffer[0];
        *psize2 = count - first_half;
    } else {
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
        *psize1 = count;
        *pdata2 = NULL;
        *psize2 = 0;
    }	

    return count;
}


static uint32_t advance_write_index(buffer_cblk_t *pcblk, uint32_t frames) 
{
	return pcblk->write_index = ((pcblk->write_index + frames) & pcblk->big_mask);
}

static uint32_t advance_read_index(buffer_cblk_t *pcblk, uint32_t frames) 
{			
	return pcblk->read_index = ((pcblk->read_index + frames) & pcblk->big_mask);
}

static void *thread_client(void *pdata) {
	buffer_cblk_t *pcblk = (buffer_cblk_t *)pdata;
	uint32_t frames_avail;
	uint32_t frames_req;
	void *ptr1;
	uint32_t size1;
	void *ptr2;
	uint32_t size2;	
	
	while (g_client_runnig) {
	    pthread_mutex_lock(&pcblk->lock);
		frames_avail = frames_write_available(pcblk);
		if (frames_avail == 0) {
			printf("[c]wait server\n");
			pthread_cond_wait(&pcblk->cond, &pcblk->lock);
			
			if (g_client_runnig == 0) {
				break;
			}			
			frames_avail = frames_write_available(pcblk);			
		}
		pthread_mutex_unlock(&pcblk->lock);

		frames_req = BUF_FRAME_REQ;
		if (frames_req > frames_avail) {
			frames_req = frames_avail;
		}
		
		frames_req = get_write_buffer(pcblk, frames_req, &ptr1, &size1, &ptr2, &size2);
		if (frames_req > 0) {
			printf("[c]frames_req = 0x%x\n", frames_req);
			printf("[c]buf1 = 0x%x, size1 = 0x%x, buf2 = 0x%x, size2 = 0x%x\n", (unsigned int)ptr1, size1, (unsigned int)ptr2, size2);		
			advance_write_index(pcblk, frames_req);		
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
	buffer_cblk_t *pcblk = &g_buffer_cblk;
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
	
	memset(pcblk, 0, sizeof(*pcblk));

    pthread_mutex_init(&pcblk->lock, NULL);
    pthread_cond_init(&pcblk->cond, NULL);
    
	buf = malloc(BUF_FRAME_COUNT*BUF_FRAME_SIZE);
    buffer_cblk_init(pcblk, buf, BUF_FRAME_COUNT, BUF_FRAME_SIZE);

	printf("[init] buf = 0x%x, end = 0x%x\n", (unsigned int)pcblk->buffer, (unsigned int)pcblk->buffer + BUF_FRAME_SIZE*BUF_FRAME_COUNT);

	pthread_create(&tid, NULL, thread_client, pcblk);

	cnt = 0;
	while (is_running && g_client_runnig) {
		frames_rdy = frames_read_available(pcblk);        
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
		
		frames_req = get_read_buffer(pcblk, frames_req, &ptr1, &size1, &ptr2, &size2);
		if (frames_req > 0) {
			printf("[s]frames_req = 0x%x\n", frames_req);
			printf("[s]buf1 = 0x%x, size1 = 0x%x, buf2 = 0x%x, size2 = 0x%x\n", (unsigned int)ptr1, size1, (unsigned int)ptr2, size2);
            
            pthread_mutex_lock(&pcblk->lock);            
            
			advance_read_index(pcblk, frames_req);
            
            pthread_cond_signal(&pcblk->cond);	
            pthread_mutex_unlock(&pcblk->lock);
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
	pthread_cond_signal(&pcblk->cond);
	pthread_join(tid, NULL);

	pthread_cond_destroy(&pcblk->cond);
	pthread_mutex_destroy(&pcblk->lock);

	if (pcblk->buffer) {
		free(pcblk->buffer);
		pcblk->buffer = NULL;
	}
    
    return 0;
}


