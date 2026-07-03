/*

if(last)                     
  {
    last->next = block;
  }

if base_head is NULL, means linked list has not been made yet, so we create new blocks, but when base_head is not NULL, that means there is a linked list and we first need to check for free blocks rather than making new one.

*/


#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

struct header_block
{
    size_t size;
    struct header_block * next;
    int free;           
    int magic;          // debug

  };
  
void * base_head = NULL;

#define HEADER_SIZE sizeof(struct header_block)

// function of data type header_block.
struct header_block * find_free_block(struct header_block **last, size_t size)
{ 
    struct header_block *current = base_head;
    while( current && !(current->free && current->size >= size))
    {
      *last = current;
      current = current->next;
    }  

    // when the block both free and big enough, we turn that TRUE --> FALSE, so that WHILE loop can fail and stop.
    // and return that block pointer to user.
    return current;
}

struct header_block *append_new_block(struct header_block* last, size_t size)
{
  struct header_block *block;
  block = sbrk(0);
  void *request = sbrk(size + HEADER_SIZE);
  assert((void*)block == request);
  if(request == (void*)-1)
  {
    return NULL; 
  }
  if(last)                      // null on first request; explaination on top
  {
    last->next = block;
  }
  block->size = size;
  block->next = NULL;
  block->free = 0;             // not free, block is filled now
  block->magic = 0x12345678;
  return block;
}



void * malloc(size_t size)
{

  struct header_block * block;
  if(size <= 0)
  {
    return NULL;
  }
  if( !base_head)
  {
    block = append_new_block(NULL, size);
    if(!block)
    {
      return NULL;
    }
    base_head = block;
  }
  else
  {
    struct header_block *last = base_head;
    block = find_free_block(&last, size);
    if( !block )
    {
      block = append_new_block(last, size);
      if(!block)
      {
        return NULL;
      }
    }
    else
    {
      block->free = 0;
      block->magic = 0x77777777;
    }
    return (block+1);
  }


  void *ptr = sbrk(0);
  void *request = sbrk(size + HEADER_SIZE);
  if (request == (void*) -1) 
  {
    return NULL; 
  } 
  else 
  {
    assert(ptr == request);
    void *data_ptr = (char*) request + HEADER_SIZE;  
    return data_ptr;
  }
}

void free(void *ptr)
{

}