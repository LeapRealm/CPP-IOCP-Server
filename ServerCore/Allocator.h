#pragma once

/*-----------------------
      Base Allocator
-----------------------*/

class BaseAllocator
{
public:
    static void*    Alloc(int32 size);
    static void     Release(void* ptr);
};
