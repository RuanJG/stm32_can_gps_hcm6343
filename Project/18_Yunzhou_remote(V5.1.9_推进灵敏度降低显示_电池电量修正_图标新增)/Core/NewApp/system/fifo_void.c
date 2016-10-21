#include "fifo_void.h"


void fifo_void_init(fifo_void_t *fifo,int buf_len, fifo_void_put_callback pcb, fifo_void_get_callback gcb)
{
  fifo->head = 0;
  fifo->tail = 0;
  fifo->len = buf_len;
	fifo->enable = 1;
	fifo->error_overflow = 0;
	fifo->put_cb = pcb;
	fifo->get_cb = gcb;
}

int fifo_void_avail(fifo_void_t *fifo)
{
  return (fifo->len + fifo->head - fifo->tail) % fifo->len;
}
int fifo_void_free(fifo_void_t *fifo)
{
  return (fifo->len - 1 - fifo_void_avail(fifo));
}
char fifo_void_enabled(fifo_void_t *fifo)
{
  return  fifo->enable ;
}
char is_fifo_void_overflow(fifo_void_t *fifo)
{
  return  fifo->error_overflow ;
}

int fifo_void_put(fifo_void_t *fifo, void *data)
{
  int next;

  // check if FIFO has room
  next = (fifo->head + 1) % fifo->len;
  if (next == fifo->tail) {
    // full
		fifo->error_overflow = 1;
    return 0;
  }

  //fifo->buf[fifo->head] = c;
	fifo->put_cb(fifo->head,data);
  fifo->head = next;

  return 1;
}

int fifo_void_recovery_put(fifo_void_t *fifo, void *data)
{
  int next;

  // check if FIFO has room
  next = (fifo->head + 1) % fifo->len;
  if (next == fifo->tail) {
    // full
		fifo->error_overflow = 1;
		fifo->tail = (fifo->tail + 1) % fifo->len;
    //return 0;
  }

  //fifo->buf[fifo->head] = c;
	fifo->put_cb(fifo->head,data);
  fifo->head = next;

  return 1;
}

int fifo_void_get(fifo_void_t *fifo, void *dst)
{
  int next;

  // check if FIFO has data
  if (fifo->head == fifo->tail) {
    return 0;
  }

  next = (fifo->tail + 1) % fifo->len;

  //*pc = fifo->buf[fifo->tail];
	fifo->get_cb(fifo->tail,dst);
  fifo->tail = next;

	
  return 1;
}

