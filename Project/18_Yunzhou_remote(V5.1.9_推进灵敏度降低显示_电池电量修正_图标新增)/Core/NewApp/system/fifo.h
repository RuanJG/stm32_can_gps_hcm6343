#ifndef __FIFO_H
#define __FIFO_H

typedef struct {
  int         head;
  int         tail;
  char      *buf;
  unsigned int	len;
	unsigned int count;
	char				enable;
	//for check 
	char error_overflow;
} fifo_t;

void fifo_init(fifo_t *fifo, char *buf,int len);
int fifo_put(fifo_t *fifo, char c);
int fifo_recovery_put(fifo_t *fifo, char c);
int fifo_get(fifo_t *fifo, char *pc);
int fifo_avail(fifo_t *fifo);
int fifo_free(fifo_t *fifo);
char fifo_enabled(fifo_t *fifo);
char is_fifo_overflow(fifo_t *fifo);

#define FIFO_DEF(name) fifo_t #name={.enable = 0,.head=0,.tail=0,.count=0,};



#endif //__FIFO_H