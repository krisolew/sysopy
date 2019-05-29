#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

int **image = NULL;
int **result = NULL;
double **filter = NULL;

int i_height;
int i_width;
int filter_size;

int thread_count;
int *threads;

int max(int a, int b){
    return a > b ? a : b;
}

double calculate_pixel(int x, int y){
    int i,j;
    double sum = 0;
    for(i = 0; i < filter_size; i++){
        for(j = 0; j < filter_size; j++){
            int h_expr = max(0, x - ceil(filter_size / 2.0) + i);
            int w_expr = max(0, y - ceil(filter_size / 2.0) + j);
            if (h_expr < i_height && w_expr < i_width)
               sum += image[h_expr][w_expr] * filter[i][j];
        }
    }
    return round(sum);
}

struct timeval *thread_calculation_block(void *args){
    int *arg = (int*) args;
    int k = arg[0];
    struct timeval *start = malloc(sizeof(struct timeval));
    struct timeval *end = malloc(sizeof(struct timeval));
    struct timeval *res = malloc(sizeof(struct timeval));
    gettimeofday(start,NULL);
    int i,j;
    int x_start = ceil(k * i_width / (1.0*thread_count));
    int x_end = ceil((k + 1) * i_width / (1.0*thread_count)) - 1;
    for(i = x_start; i <= x_end; i++){
        for(j = 0; j < i_height; j++){
            result[j][i] = abs(calculate_pixel(j, i));
        }
    }
    gettimeofday(end,NULL);
    timersub(end,start,res);
    free(start);
    free(end);
    return res;
}

struct timeval *thread_calculation_interleaved(void *args){
    int *arg = (int*) args;
    int k = arg[0];
    struct timeval *start = malloc(sizeof(struct timeval));
    struct timeval *end = malloc(sizeof(struct timeval));
    struct timeval *res = malloc(sizeof(struct timeval));
    gettimeofday(start,NULL);
    int i,j;
    for(i = k; i < i_width; i += thread_count){
        for(j = 0; j < i_height; j++){
            result[j][i] = abs(calculate_pixel(j, i));
        }
    }
    gettimeofday(end,NULL);
    timersub(end,start,res);
    free(start);
    free(end);
    return res;
}


int load_image(FILE* file){
    int i,j;
    int max_color;
    if(getc(file) != 'P') return -1;
    if (getc(file) != '2') return -1;
    while(getc(file) != '\n');

    while(getc(file) == '#'){
        while(getc(file) != '\n');
    }
    fseek(file, -1, SEEK_CUR);


    if(fscanf(file, "%d", &i_width) != 1) return -1;
    if(fscanf(file, "%d", &i_height) != 1) return -1;
    if(fscanf(file, "%d", &max_color) != 1) return -1;
    if(max_color > 255) return -1;

    image = malloc(i_height * sizeof(char*));
    result = malloc(i_height * sizeof(char*));

    for(i = 0; i < i_height; i++) {
        image[i] = malloc(i_width * sizeof(int));
        result[i] = malloc(i_width * sizeof(int));
    }

    for(i = i_height - 1; i >= 0; i--){
        for(j = 0 ; j < i_width; j++){
            if(fscanf(file, "%d", &image[i][j]) != 1) return -1;
        }
    }
    return 0;
}

int load_filter(FILE* file){
    int i,j;
    if(fscanf(file, "%d", &filter_size) != 1) return 1;

    filter = malloc(filter_size*sizeof(double*));
    for(i = 0; i < filter_size; i++)
      filter[i] = malloc(filter_size*sizeof(double));

    for(i = 0; i < filter_size; i++){
        for(j = 0; j < filter_size; j++){
            if(fscanf(file, "%lf", &filter[i][j]) != 1) return 1;
        }
    }
    return 0;
}

int save_result(FILE* file){
    int i,j;
    fprintf(file,"P2\n%d %d\n%d\n",i_width,i_height,255);
    for(i = i_height - 1; i >= 0; i--){
        for(j = 0 ; j < i_width; j++){
            if(j % 10 == 0) fprintf(file,"\n");
            fprintf(file, "%d ", result[i][j]);
        }
    }
    return 0;
}

void finish_work(void){
    int i;
    if(image != NULL){
        for(i = 0; i < i_height; i++) free(image[i]);

        free(image);
    }

    if(result != NULL){
        for(i = 0; i < i_height; i++) free(result[i]);

        free(result);
    }

    if(filter != NULL){
        for(i = 0; i < filter_size; i++) free(filter[i]);

        free(filter);
    }
}

int main(int argc, char **argv){
    if(argc != 6)
    {
      perror("Expected 5 arguments: thread count, calculation mode, input image, filter definition and output image name");
      return -1;
    }

    FILE *file = fopen(argv[3],"r");
    if(file == NULL)
    {
      perror("Error during file opening");
      return -1;
    }

    thread_count = strtol(argv[1], NULL, 10);
    threads = malloc(thread_count*sizeof(int));
    int i;
    for(i = 0; i < thread_count; i++) threads[i] = i;

    atexit(finish_work);

    if(load_image(file) != 0)
    {
      perror("Unsupported file format");
      return -1;
    }

    fclose(file);
    file = fopen(argv[4],"r");
    if(file == NULL)
    {
      perror("Error during file opening");
      return -1;
    }

    if(load_filter(file) != 0)
    {
      perror("Unsupported file format");
      return -1;
    }

    fclose(file);

    pthread_t thread_IDs[thread_count];
    if(strcmp(argv[2],"block") == 0){
        for(i = 0; i < thread_count; i++){
            if(pthread_create(&thread_IDs[i],NULL,(void*)(void*)thread_calculation_block,&threads[i]) !=0)
            {
               perror("Error during thread creation");
               return -1;
            }
            else{
                printf("Thread %ld started \n",thread_IDs[i]);
            }
        }
    }
    else if(strcmp(argv[2],"interleaved") == 0){
        for(i = 0; i < thread_count; i++){
            if(pthread_create(&thread_IDs[i],NULL,(void*)(void*)thread_calculation_interleaved,&threads[i]) != 0)
            {
               perror("Error during thread creation");
               return -1;
            }
            else{
                printf("Thread %ld started \n",thread_IDs[i]);
            }
        }
    }
    else
    {
      perror("Unsupported calculation mode");
      return -1;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    }

    for(i = 0; i < thread_count; i++){
        struct timeval *work_time;
        if(pthread_join(thread_IDs[i],(void*)&work_time) != 0)
        {
           perror("Error on thread join");
           return -1;
        }
        else{
            printf("Thread %ld worked %ld.%.6ld seconds \n",thread_IDs[i],work_time->tv_sec,work_time->tv_usec);
        }
    }

    file = fopen(argv[5],"w");
    if(file == NULL)
    {
      perror("Error during file opening");
      return -1;
    }
    if(save_result(file) != 0)
    {
      perror("Error on result save");
      return -1;
    }
    else{
        printf("File saved successfully \n");
    }
    fclose(file);

    return 0;
}
