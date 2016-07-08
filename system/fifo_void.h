#ifndef __VOID_FIFO_H
#define __VOID_FIFO_H

typedef void (*fifo_void_put_callback)(int id, void * dst);
typedef void (*fifo_void_get_callback)(int id, void * msg);
typedef struct {
  int         head;
  int         tail;
  int	      len;
	char				enable;
	//for check 
	char error_overflow;
	
	fifo_void_put_callback put_cb;
	fifo_void_get_callback get_cb;
} fifo_void_t;

void fifo_void_init(fifo_void_t *fifo,int len,fifo_void_put_callback put_cb,fifo_void_get_callback get_cb);
int fifo_void_put(fifo_void_t *fifo, void *dst);
int fifo_void_recovery_put(fifo_void_t *fifo, void *data);
int fifo_void_get(fifo_void_t *fifo, void *dst);
int fifo_void_avail(fifo_void_t *fifo);
int fifo_void_free(fifo_void_t *fifo);
char fifo_void_enabled(fifo_void_t *fifo);
char is_fifo_void_overflow(fifo_void_t *fifo);

#define FIFO_VOID_DEF(name) fifo_void_t #name={.enable = 0,.head=0,.tail=0,};



#endif //__VOID_FIFO_H