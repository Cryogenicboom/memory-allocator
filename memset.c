#include <unistd.h>
#include <stdio.h>

typedef struct
{
    long int *start_ptr;
    long int *data_ptr;
    int data_size;
}head;

head block;

int* getmem(int req_size)               // getmem == malloc
{
    // gdb start 
    void * memory = sbrk(0);
    // end 
    
    int head_size = sizeof(head);
    block.start_ptr = sbrk(req_size + head_size);
    block.data_ptr = block.start_ptr + head_size;
    block.data_size = req_size;

    return block.start_ptr;
}   

int main()
{
    int * mem1 = getmem(10);
    int * mem2 = getmem(20);
    return 0;
}