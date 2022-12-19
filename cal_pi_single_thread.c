#include<stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

/*
 * total means how many numbers of pai to cal
 */
int cal_pai(int total)
{
    long a = 10000, b = 0, d, e = 0, g;
    long c = total;
    long *f = (long *)malloc(sizeof(long) * (c +1));
    if (f == NULL) {
        return -1;
    } 
    for( ; b-c ; ) {
        f[b++] = a/5;
    }
    
    for( ;d=0, g=c*2; c-=14, printf("%.4d", e+d/a),e = d%a) {
        for(b=c;d+=f[b]*a,f[b]=d%--g,d/=g--,--b;d*=b);
    }
    return 0;
}

void 
main(int argc, char **argv)
{

    clock_t startTime = clock(), endTime;
    int ret = cal_pai(70000);
    if (ret != 0) {
        printf("error calculate pai\n");
        return;
    }
    endTime = clock();
    printf("\n time:%f\n", (double)(endTime - startTime));
}
