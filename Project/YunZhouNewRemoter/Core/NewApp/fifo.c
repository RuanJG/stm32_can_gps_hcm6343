#include "fifo.h"

void fifo_init(fifo_t *fifo, char *buf, int buf_len)
{
  fifo->head = 0;
  fifo->tail = 0;
  fifo->buf = buf;
  fifo->len = buf_len;
	fifo->count = 0;
	fifo->enable = 1;
	fifo->error_overflow = 0;
}

int fifo_avail(fifo_t *fifo)
{
  //return (fifo->len + fifo->head - fifo->tail) % fifo->len;
	return fifo->count;
}
int fifo_free(fifo_t *fifo)
{
  //return (fifo->len - 1 - fifo_avail(fifo));
	return fifo->len - 1 - fifo->count;
}
char fifo_enabled(fifo_t *fifo)
{
  return  fifo->enable ;
}
char is_fifo_overflow(fifo_t *fifo)
{
  return  fifo->error_overflow ;
}


int fifo_put(fifo_t *fifo, char c)
{
  int next;

  // check if FIFO has room
  next = (fifo->head + 1) % fifo->len;
	#if 1 // for irq get(put) and user function put(get) complication, so just modify "head" , witch not read "tail"
  if (next == fifo->tail) {
    // full
		fifo->error_overflow = 1;
    return 0;
  }
	#endif

  fifo->buf[fifo->head] = c;
  fifo->head = next;
	fifo->count++;

  return 1;
}

int fifo_recovery_put(fifo_t *fifo, char c)
{
  int next;

  // check if FIFO has room
  next = (fifo->head + 1) % fifo->len;
  if (next == fifo->tail) {
    // full
		fifo->error_overflow = 1;
		fifo->tail=(fifo->tail + 1) % fifo->len;
		fifo->count--;
    //return 0;
  }

  fifo->buf[fifo->head] = c;
  fifo->head = next;
	fifo->count++;

  return 1;
}


int fifo_get(fifo_t *fifo, char *pc)
{
  int next;

  // check if FIFO has data
	#if 1 // for irq get(put) and user function put(get) complication, so just modify "tail" , witch not read "head"
  if (fifo->head == fifo->tail) {
    return 0;
  }
	#endif

  next = (fifo->tail + 1) % fifo->len;

  *pc = fifo->buf[fifo->tail];
  fifo->tail = next;
  fifo->count--;
	
  return 1;
}
