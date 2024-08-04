#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> 

void main(int argc, char * argv[]) {
    int NowLen = atoi(argv[1]);
    int MaxLen = atoi(argv[2]);
    static int MaxSize = 0;
    if (MaxSize == 0) {
        int Temp = MaxLen;
        while (Temp / 10) {
            MaxSize++;
            Temp = Temp / 10;
        }
        MaxSize++;
    } else {
        printf("\033[1A"); //先回到上一行
        printf("\033[K");  //清除该行
    }
    switch (MaxSize) {
        case 1:
            printf("Now:%1d / Max: %1d\n", NowLen, MaxLen);
            break;
        case 2:
            printf("Now:%2d / Max: %2d\n", NowLen, MaxLen);
            break;
        case 3:
            printf("Now:%3d / Max: %3d\n", NowLen, MaxLen);
            break;
        case 4:
            printf("Now:%4d / Max: %4d\n", NowLen, MaxLen);
            break;
        case 5:
            printf("Now:%5d / Max: %5d\n", NowLen, MaxLen);
            break;
        case 6:
            printf("Now:%6d / Max: %6d\n", NowLen, MaxLen);
            break;
        case 7:
            printf("Now:%7d / Max: %7d\n", NowLen, MaxLen);
            break;
        case 8:
            printf("Now:%8d / Max: %8d\n", NowLen, MaxLen);
            break;
        case 9:
            printf("Now:%9d / Max: %9d\n", NowLen, MaxLen);
            break;
        case 10:
            printf("Now:%10d / Max: %10d\n", NowLen, MaxLen);
            break;
    }
    if (NowLen >= MaxLen) {
        MaxSize = 0;
    }
}