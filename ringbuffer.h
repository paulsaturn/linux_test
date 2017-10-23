/**
	@filename :
		ringbuffer.h
		
	@note :
		The frame_count of buffer must be 2^n. 
**/
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

typedef uint32_t rbuf_index_t;

#define __RBUF_FRAME_SUPPORT__

typedef struct {
	char *buffer;
	volatile rbuf_index_t write_index;	/*  */
	volatile rbuf_index_t read_index;	/*  */
	rbuf_index_t big_mask;				/**< Used for wrapping indices with extra bit to distinguish full/empty. Not Used */
	rbuf_index_t small_mask;			/**< Used for fitting indices to buffer. */
	uint32_t frame_count;
#ifdef __RBUF_FRAME_SUPPORT__
	uint32_t frame_size;
#endif	
}rbuf_cblk_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __RBUF_FRAME_SUPPORT__
void rbuf_init(rbuf_cblk_t *pcblk, char *pbuf, uint32_t frame_count, uint32_t frame_size);
#else	
void rbuf_init(rbuf_cblk_t *pcblk, char *pbuf, uint32_t frame_count);
#endif

void rbuf_reset(rbuf_cblk_t *pcblk);

uint32_t rbuf_read_available(rbuf_cblk_t *pcblk);
uint32_t rbuf_get_read_buffer(rbuf_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2);
uint32_t rbuf_advance_read_index(rbuf_cblk_t *pcblk, uint32_t frames);		

uint32_t rbuf_write_available(rbuf_cblk_t *pcblk);
uint32_t rbuf_get_write_buffer(rbuf_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2);
uint32_t rbuf_advance_write_index(rbuf_cblk_t *pcblk, uint32_t frames);

#ifdef __cplusplus
}
#endif

#endif //__RINGBUFFER_H__

