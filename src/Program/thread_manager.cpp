#include "src/Program/thread_manager.h"

bool stopping = false;

// NOTE: glfw cannot be used in any threads outside of main thread
// Function executed by input thread
void *input_thread_main(void *arg) {
    input_thread_data* data = (input_thread_data*) arg;

    printf("testing a %i \n", data->thread_id);

    return 0;
}

ThreadManager::ThreadManager() {

}

void ThreadManager::StartThreads() {
    pthread_t thr[MAX_THREADS];

    input_data = (input_thread_data*) calloc(1, sizeof(input_thread_data));
    input_data->thread_id = 0;
    input_data->mouse_down = false;
    input_data->mouse_was_down = false;
    input_data->currentSample = 0;

    int errorcode = pthread_create(&thr[0], NULL, input_thread_main, &input_data);
    if (errorcode) fprintf(stderr, "error: pthread_create, rc: %d\n", errorcode);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thr[i], NULL);
    }
}
void ThreadManager::StopThreads() {
    stopping = true;
}