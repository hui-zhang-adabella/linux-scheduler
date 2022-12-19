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


/*
 * total means how many numbers of pai to cal
 * f is a array
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
    " -m    mode, 1 means subsequense run, 2 means multi-thread\n"
    ""
    , program_name);
}



#define SUBSEQUENSE_MODE    1
#define MULTI_THREAD_MODE   2

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
            char thread_num[128];
            
            if (pthread_create(&tid, NULL, (void*)thread_do_cal_pai, &i) != 0) {
                printf("pthread_create error, thread_num:%d\n", i);
                exit(EXIT_FAILURE);
            }
            char *rev = NULL;
            pthread_join(tid, (void *)&rev);
        }       
        printf("\n create thread %d, run success\n", g_thread_num);
    } else {
        printf("mode:%d unknown\n", g_mode);
        exit(EXIT_FAILURE);
    }  

    endTime = clock();
    printf("\n time:%f\n", (double)(endTime - startTime));
}
