#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <getopt.h> 

int g_thread_num = 1;
int g_pai_num = 56000;
int g_mode = 1; /* mode 1: 顺序执行， mode 2: 多线程执行 */
int g_file_num = 1;
int g_file_size = (1024 * 1024 * 512);

/*
 * total means how many numbers of pai to cal
 * f is a array
 *
 * 
 */
int cal_pai(int total, long *f)
{
    long a = 10000, b = 0, d, e = 0, g;
    long c = total;
    //long *f = (long *)malloc(sizeof(long) * (c +1));
    if (f == NULL) {
        return -1;
    } 
    for( ; b-c ; ) {
        f[b++] = a/5;
    }
    
    //for( ;d=0, g=c*2; c-=14, printf("%.4d", e+d/a),e = d%a) {
    for( ;d=0, g=c*2; c-=14,e = d%a) {
        for(b=c;d+=f[b]*a,f[b]=d%--g,d/=g--,--b;d*=b);
    }
    return 0;
}

int do_cal_pai(int pai_num)
{
    long *array_f = (long *)malloc(sizeof(long) * (pai_num +1));
    if (array_f == NULL) {
        printf("error malloc\n");
        return -1;
    }
    int ret = cal_pai(pai_num, array_f);
    if (ret != 0) {
        printf("error calculate pai\n");
        return -1;
    }
   
    return 0;
}

void* thread_do_cal_pai(void *arg) 
{
    int thread_num = *((int *)arg);
    pid_t pid = getpid();
    if (do_cal_pai(g_pai_num) != 0) {
        printf("error calculate pai in thread num:%d, pid:%u\n", thread_num, pid);
        return NULL;
    }
    return NULL; 
}

void usage(int argc, char *argv[])
{
    char *program_name = argv[0];
    char *const slash = strrchr(program_name, '/');
    if (slash) {
        program_name = slash + 1;
    }

    printf("Usage: %s [Options]\n"
    "Options:\n"
    " -n    how many times to run or how many thread create\n"
    " -p    pai num, how many pai's value\n"
    " -m    mode, 1 means subsequense run cpu burst, 2 means multi-thread run cpu burst, 3 means subsequense run io burst, 4 means multi thread run io burst\n"
    " -s    io file size, default 10M = 10*1024*1024"
    " -h    help information"
    "\n"
    , program_name);
}

#define TEST_IO_PATH "./test_io/"

int do_file_io(char *file_path, int g_file_size)
{
    char content[512];
    FILE *f = fopen(file_path, "wb+");
    if (f == NULL ) {
        printf("do_file_io open file error:%s", file_path);
        exit(EXIT_FAILURE);
    }
    int write_size = 0;
    int write_count = g_file_size/sizeof(content);
    int i = 0;
    for (i = 0; i < write_count; i++) {
        memset(content, i, sizeof(content));
        if (fwrite(content, 1, sizeof(content), f) != sizeof(content)) {
            printf("do_file_io write error\n");
            exit(EXIT_FAILURE);
        }
        fflush(f);
    }
    int last = g_file_size % sizeof(content);

    if (fwrite(content, 1, last, f) != last) {
        printf("do_file_io write error\n");
        exit(EXIT_FAILURE);
    }
    fflush(f);
    fclose(f);
    
    f = fopen(file_path, "wb+");
    if (f == NULL) {
        printf("do_file_io open file error:%sn", file_path);
        exit(EXIT_FAILURE);
    }
    while(!feof(f)) {
        size_t read_len = fread(content, sizeof(char), sizeof(content), f);    
        if (read_len != sizeof(content)) {
            printf("read file end:%d\n", read_len);
        }
    }
    fclose(f);
}

void *thread_do_file_io(void *arg)
{
    int thread_num = *((int *)arg);
    pid_t pid = getpid();
    char file_path[128];
    snprintf(file_path, sizeof(file_path), "%s%d", TEST_IO_PATH, thread_num);
    printf("file full path:%s\n", file_path); 
    if (do_file_io(file_path, g_file_size) != 0) {
        printf("error do file io in thread num:%d, pid:%u\n", thread_num, pid);
        return NULL;
    }  
    return NULL;
}

#define SUBSEQUENSE_MODE        1
#define MULTI_THREAD_MODE       2
#define IO_SUBSEQUENSE_MODE     3
#define IO_MULTI_THREAD_MODE    4

void main(int argc, char *argv[])
{

    int ch = 0;
    while ((ch = getopt(argc, argv, "hn:p:m:")) != -1) {
        switch (ch) {
        case 'n':
            printf("option thread num:%s\n", optarg);
            g_thread_num = atoi(optarg);
            break;
        case 'p':
            printf("option pai num:%s\n", optarg);
            g_pai_num = atoi(optarg);
            break;
        case 'm':
            printf("option mode:%s\n", optarg);
            g_mode = atoi(optarg);
            break;
        case 'h':
            usage(argc, argv);
            exit(0);
        default:
            printf("option invalid:%c\n", ch);
            usage(argc, argv);
            exit(EXIT_FAILURE);
        }
    
    }



    clock_t startTime = clock(), endTime;
    
    int i = 0;
    if (g_mode == SUBSEQUENSE_MODE) {
        for (i = 0; i < g_thread_num; i++) {
            int ret = do_cal_pai(g_pai_num);
            if (ret != 0) {
                printf("\n error run do_cal_pai\n");
                exit(EXIT_FAILURE);
            }
        }
        printf("\nsubsequense run %d times\n", g_thread_num); 
    } else if (g_mode == MULTI_THREAD_MODE) {
        for (i = 0; i < g_thread_num; i++) {
            pthread_t tid;
            
            if (pthread_create(&tid, NULL, (void*)thread_do_cal_pai, &i) != 0) {
                printf("pthread_create error, thread_num:%d\n", i);
                exit(EXIT_FAILURE);
            }
            char *rev = NULL;
            pthread_join(tid, (void *)&rev);
        }       
        printf("\n create thread %d, run success\n", g_thread_num);
    } else if (g_mode == IO_SUBSEQUENSE_MODE) {
        for (i = 0; i < g_thread_num; i++) {
            char file_path[128];
            snprintf(file_path, sizeof(file_path), "%s%d", TEST_IO_PATH, i);
            int ret = do_file_io(file_path, g_file_size);
            if (ret != 0) {
                printf("\n error run do_file_io\n");
                exit(EXIT_FAILURE);
            }
        } 
        printf("\nsubsequense run file io %d times\n", g_thread_num); 

    } else if (g_mode == IO_MULTI_THREAD_MODE) {
        for (i = 0; i < g_thread_num; i++) {
            pthread_t tid;
            
            if (pthread_create(&tid, NULL, (void*)thread_do_file_io, &i) != 0) {
                printf("pthread_create error, thread_num:%d\n", i);
                exit(EXIT_FAILURE);
            }
            char *rev = NULL;
            pthread_join(tid, (void *)&rev);
        }       
        printf("\n create io thread %d, run success\n", g_thread_num);
    }
    
    else {
        printf("mode:%d unknown\n", g_mode);
        exit(EXIT_FAILURE);
    }  

    endTime = clock();
    printf("\n time:%f\n", (double)(endTime - startTime));
}
