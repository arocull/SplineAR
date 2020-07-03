#include "src/Program/gpu_memory.h"

GPUMemory::GPUMemory(GPU* gpuReference, int amount, size_t item_size, cl_mem_flags flags) {
    gpu = gpuReference;
    mem_size = item_size * amount;

    gpuMemory = gpu->AllocateMemory(amount, item_size, flags);
    memory = malloc(mem_size); // Creates empty set of data (needs clearing)

    #ifdef DEBUG
    freed = false;
    #endif
}

size_t GPUMemory::GetSize() { return mem_size; }
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