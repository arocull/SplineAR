#include "src/Program/gpu_memory.h"

GPUMemory::GPUMemory(GPU* gpuReference, int amount, size_t item_size, cl_mem_flags flags) {
    gpu = gpuReference;
    
    maxAmount = amount;
    memFlags = flags;
    mem_size = item_size * amount;

    gpuMemory = gpu->AllocateMemory(amount, item_size, flags);
    memory = malloc(mem_size); // Creates empty set of data (needs clearing)

    #ifdef DEBUG
    freed = false;
    #endif
}

size_t GPUMemory::GetSize() { return mem_size; }
int GPUMemory::GetMaxAmount() { return maxAmount; }
void* GPUMemory::GetData() {
    #ifdef DEBUG
    if (freed) { // If in Debug mode, catch attempts to reference non-existant data
        perror("ERROR: Attempted to read GPU Memory object that has already been freed!\n");
        return nullptr;
    }
    #endif
    return memory;
}
cl_mem* GPUMemory::GetGPUData() {
    #ifdef DEBUG
    if (freed) { // If in Debug mode, catch attempts to reference non-existant data
        perror("ERROR: Attempted to read GPU Memory object that has already been freed!\n");
        return nullptr;
    }
    #endif
    return &gpuMemory;
}


void GPUMemory::CopyDataToGPU(bool blockingWrite) {
    #ifdef DEBUG
    if (freed) {  // Do not attempt to write to GPU when memory has already been freed
        perror("ERROR: Attempted to write to memory block on GPU even though it has already been freed!\n");
        return;
    }
    #endif
    gpu->WriteMemory(gpuMemory, memory, mem_size, blockingWrite);
}
void GPUMemory::CopyDataFromGPU(bool blockingWrite) {
    #ifdef DEBUG
    if (freed) { // Do not attempt to interact with GPU or write to CPU if memory has already been freed
        perror("ERROR: Attempted to read memory block off of GPU even though it has already been freed!\n");
        return;
    }
    #endif
    clEnqueueReadBuffer(gpu->queue, gpuMemory, blockingWrite, 0, mem_size, memory, 0, NULL, NULL);
}


bool GPUMemory::Reallocate(int newSize, bool doCopy) {
    #ifdef DEBUG
    if (freed) { // Do not attempt to interact with GPU or write to CPU if memory has already been freed
        perror("ERROR: Attempted to reallocate GPU memory block even though it has already been freed!\n");
        return false;
    }
    #endif

    size_t item_size = mem_size / (size_t) maxAmount;

    void* newMem = malloc(item_size * newSize);
    if (newMem) {

        if (doCopy) { // Copy data from GPU to the new memory
            cl_int code = clEnqueueReadBuffer(gpu->queue, gpuMemory, true, 0, mem_size, newMem, 0, NULL, NULL);
            if (code != CL_SUCCESS) {
                perror("Failed to copy data from GPU in memory block reallocation:\n");
                printf("\t(CL error code %i)\n", (int) code);
                free(newMem); // Free new memory to prevent memory leak
                return false;
            }
        }

        FreeMemory(); // Free old memory
        memory = newMem; // Change pointer address to new memory address
        gpuMemory = gpu->AllocateMemory(newSize, item_size, memFlags); // Allocate new memory on the GPU
        // Update tracking variables
        maxAmount = newSize;
        mem_size = (size_t) newSize * item_size;

        // Mark data as unfreed since we are still using it (just new memory blocks)
        #ifdef DEBUG
            freed = false;
        #endif

        if (doCopy) {
            CopyDataToGPU(true);
        }

        return true;
    } else {
        perror("ERROR: Failed to reallocate GPU memory block (not enough CPU malloc failed)!\n");
        return false;
    }
}


void GPUMemory::FreeMemory() {
    #ifdef DEBUG
    if (freed) {
        perror("ERROR: Attempted to free memory block that has already been freed!\n");
        return;
    }
    freed = true;
    #endif
    clReleaseMemObject(gpuMemory);
    free(memory);
}