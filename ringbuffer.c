/**
	@filename :
		ringbuffer.c
		
	@note :
	
**/
#include <stdio.h>

#include "ringbuffer.h"

/*
*/
#ifdef __RBUF_FRAME_SUPPORT__
void rbuf_init(rbuf_cblk_t *pcblk, char *pbuf, uint32_t frame_count, uint32_t frame_size)
#else
void rbuf_init(rbuf_cblk_t *pcblk, char *pbuf, uint32_t frame_count)
#endif
{
    pcblk->buffer = pbuf;
    pcblk->frame_count = frame_count;    
#ifdef __RBUF_FRAME_SUPPORT__    
    pcblk->frame_size = frame_size;
#endif    
    pcblk->read_index = 0;
    pcblk->write_index = 0;
    pcblk->small_mask = frame_count - 1;
}

/*
*/
void rbuf_reset(rbuf_cblk_t *pcblk)
{
    pcblk->read_index = 0;
    pcblk->write_index = 0;    
}

/*
*/
uint32_t rbuf_read_available(rbuf_cblk_t *pcblk) {
	return (pcblk->write_index - pcblk->read_index);	
}

/*
*/
uint32_t rbuf_get_read_buffer(rbuf_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2) 
{	
    uint32_t   index;
    uint32_t   available = rbuf_read_available( pcblk );

    if ( count > available ) {
    	count = available;
    }

    /* Check to see if read is not contiguous. */
    index = pcblk->read_index & pcblk->small_mask;
    if ( (index + count) > pcblk->frame_count ) {
        /* Write data in two blocks that wrap the buffer. */
        uint32_t first_half = pcblk->frame_count - index;
    #ifdef __RBUF_FRAME_SUPPORT__    
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
    #else
        *pdata1 = (void *)&pcblk->buffer[index];
    #endif        
        *psize1 = first_half;
        *pdata2 = (void *)&pcblk->buffer[0];
        *psize2 = count - first_half;
    } else {
    #ifdef __RBUF_FRAME_SUPPORT__            
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
    #else
        *pdata1 = (void *)&pcblk->buffer[index];
    #endif        
        *psize1 = count;
        *pdata2 = NULL;
        *psize2 = 0;
    }	

    return count;
}

/*
*/
uint32_t rbuf_advance_read_index(rbuf_cblk_t *pcblk, uint32_t frames) 
{			
	return pcblk->read_index = pcblk->read_index + frames;
}

/*
*/
uint32_t rbuf_write_available(rbuf_cblk_t *pcblk) { 
	return (pcblk->frame_count - rbuf_read_available(pcblk));
}

/*
*/
uint32_t rbuf_get_write_buffer(rbuf_cblk_t *pcblk, uint32_t count, 
		void **pdata1, uint32_t *psize1, void **pdata2, uint32_t *psize2) 
{	
    uint32_t   index;
    uint32_t   available = rbuf_write_available( pcblk );

    if ( count > available ) {
    	count = available;
    }

    /* Check to see if write is not contiguous. */
    index = pcblk->write_index & pcblk->small_mask;
    if ( (index + count) > pcblk->frame_count ) {
        /* Write data in two blocks that wrap the buffer. */
        uint32_t   first_half = pcblk->frame_count - index;
    #ifdef __RBUF_FRAME_SUPPORT__    
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
    #else
        *pdata1 = (void *)&pcblk->buffer[index];
    #endif    
        *psize1 = first_half;
        *pdata2 = (void *)&pcblk->buffer[0];
        *psize2 = count - first_half;
    } else {
    #ifdef __RBUF_FRAME_SUPPORT__    
        *pdata1 = (void *)&pcblk->buffer[index*pcblk->frame_size];
    #else        
        *pdata1 = (void *)&pcblk->buffer[index];
    #endif        
        *psize1 = count;
        *pdata2 = NULL;
        *psize2 = 0;
    }	

    return count;
}


/*
*/
uint32_t rbuf_advance_write_index(rbuf_cblk_t *pcblk, uint32_t frames) 
{
	return pcblk->write_index = pcblk->write_index + frames;
}
