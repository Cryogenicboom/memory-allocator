/*

if(last)                     
  {
    last->next = block;
  }

if base_head is NULL, means linked list has not been made yet, so we create new blocks, but when base_head is not NULL, that means there is a linked list and we first need to check for free blocks rather than making new one.
-----------------------------------------------------------

after allocating the block, the pointer is saved as block_ptr, and ptr is incremented by 1 bytes, so when next time a allocation is requested, it starts with ptr, not block_ptr. Usage: 

return (block+1);
-----------------------------------------------------------
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

// function to get allocated block pointer.
struct header_block* get_block_ptr(void *ptr)
{
  return (struct header_block*)ptr - 1;
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
  }
  return (block+1);
}

void free(void *ptr)
{
  if (!ptr)
  {
    return;
  }
  struct header_block* block_ptr = get_block_ptr(ptr);
  assert(block_ptr->free == 0);

  assert(block_ptr->magic == 0x7777777 || block_ptr->magic == 0x12345678);
  block_ptr->free = 1;
  block_ptr->magic = 0x55555555;

}

void *realloc(void *ptr, size_t size)
{
  // ptr --- > null, it acts as malloc
  if(!ptr)
  {
    return malloc(size);
  }

  struct header_block* block_ptr = get_block_ptr(ptr);
  
  // block is lredy big so nothing to add more. 
  if(block_ptr->size >= size)
  {
    return ptr;
  }

  void *new_ptr;
  new_ptr = mlloc(size);

  if(!new_ptr)
  {
    return NULL;
  }
  memcpy(new_ptr, ptr, block_ptr->size);
  free(ptr);
  return new_ptr;

}


int main()
{
  return 0;
}