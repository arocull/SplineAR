#include "src/Program/thread_manager.h"

bool stopping = false;


// Function executed by input thread
void *input_thread_main(void *arg) {
    input_thread_data* data = (input_thread_data*) arg;

    glfwPollEvents();

    printf("testing a %i \n", data->thread_id);
    
    data->mouse_down = (glfwGetMouseButton(data->context, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);

    printf("testing b %i \n", data->thread_id);

    if (data->mouse_down) {
        if (!data->mouse_was_down) data->currentSample = 0;
        glfwGetCursorPos(data->context, &data->x_samples[data->currentSample], &data->y_samples[data->currentSample]);
        data->currentSample++;
        data->mouse_was_down = data->mouse_down;
        printf("Mouse down\n");
    }

    printf("testing c %i \n", data->thread_id);

    if (glfwWindowShouldClose(data->context)) {
        printf("Attempting to close\n");
        stopping = true;
    }
    if (stopping) {
        printf("Closing thread\n");
        free(data);
        pthread_exit(NULL);
    }
}

ThreadManager::ThreadManager(GLFWwindow* glfwContext) {
    context = glfwContext;
}

void ThreadManager::StartThreads() {
    pthread_t thr[MAX_THREADS];

    input_data = (input_thread_data*) calloc(1, sizeof(input_thread_data));
    input_data->thread_id = 0;
    input_data->mouse_down = false;
    input_data->mouse_was_down = false;
    input_data->currentSample = 0;
    input_data->context = context;

    int errorcode = pthread_create(&thr[0], NULL, input_thread_main, &input_data);
    if (errorcode) fprintf(stderr, "error: pthread_create, rc: %d\n", errorcode);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(thr[i], NULL);
    }
}
void ThreadManager::StopThreads() {
    stopping = true;
}