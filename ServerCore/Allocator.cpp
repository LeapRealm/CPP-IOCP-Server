#include "pch.h"
#include "Allocator.h"

/*-----------------------
      Base Allocator
-----------------------*/

void* BaseAllocator::Alloc(int32 size)
{
    return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
    ::free(ptr);
}
