#pragma once

#include "src/config.h"

#include <CL/cl.h>

#include "src/Program/gpu.h"


// GPU Memory - Platform that handles memory objects on both CPU and GPU for easy manipulation and management
class GPUMemory {
    public:
        // Initializes the memory on both the GPU and CPU
        GPUMemory(GPU* gpuReference, int amount, size_t item_size, cl_mem_flags flags = CL_MEM_READ_WRITE);

    private:
        // Reference to GPU object
        GPU* gpu;
        // Size of the memory block
        size_t mem_size;

        // Memory object on the GPU
        cl_mem gpuMemory;
        // Memory object on the CPU
        void* memory;

        // Number of items in the memory block
        int maxAmount;
        // Memory flags of the memory block (for reallocation)
        cl_mem_flags memFlags;

        #ifdef DEBUG
        // Catches if the memory objects have already been freed or not (DEBUG ONLY)
        bool freed;
        #endif
    
    public:
        // Returns the size of the data block
        size_t GetSize();
        // Returns the max numerical amount of items data block
        int GetMaxAmount();
        // Returns the data of the data block on the CPU (must be cast to appropiate type)
        void* GetData();
        // Returns the cl_mem object of the data block on the GPU
        cl_mem* GetGPUData();

        void SetDataAtIndex(int index, void* value);

        // Copies buffer data on CPU into the GPU memory block
        void CopyDataToGPU(bool blockingWrite = false);
        // Copies buffer data on GPU into the CPU memory block
        void CopyDataFromGPU(bool blockingWrite = false);

        // Reallocates the buffer on CPU and GPU
        // - Allocates a new CPU array, and if 'doCopy' is true it then copies GPU buffer onto the new CPU array
        // - Then deallocates both CPU and GPU arrays
        // - Finally allocates new GPU array, and if 'doCopy' is true it then copies CPU buffer onto GPU buffer
        // Returns true if reallocation was successful
        bool Reallocate(int newSize, bool doCopy = false);

        // Deallocates the memory, and if in debug mode, disables copy and fetch functions
        void FreeMemory();
};