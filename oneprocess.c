#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main( int argc, char *argv[] )  {

    long L = strtol(argv[1], NULL, 10); // number of positive integers in txt file (>=10000) test=10000
    long H = strtol(argv[2], NULL, 10); // number of hidden keys to find (<20)
    long NP = strtol(argv[3], NULL, 10); // max number of processes
    if(H>19){
        H=19;
        printf("Your command argument for hidden keys to find is too high and has been limited to 19\n");
    }

    if( argc < 4 ) {
        printf("Too few arguemnts supplied");
        exit(0);
    }
    else if( argc > 4 ) {
        printf("Too many arguments supplied.\n");
        exit(0);
    }

    //initialize array
    int size = L + 50;
    int *arr;  
    arr = (int *) malloc(sizeof(int) * size);



    //convert text file to array
    FILE *inputFile;
    inputFile = fopen("input.txt", "r");
    int i=0;

    for (i = 0; i < size; i++) {
        fscanf(inputFile, "%d", &arr[i]);
    }
    fclose(inputFile);



    //one process solve
    int sum = 0;
    int rarg = 1;
    int pid = getpid();
    int foundh = 0;
    int ppid = getppid();
    int max = 1;

    printf("Hi I am process %d with return arg %d and my parent is %d\n", pid, rarg, ppid);

    for (i = 0; i < size; i++) {
        sum = sum + arr[i];

        //find max
        if (arr[i] > max){
            max = arr[i];
        }

        //find hidden keys
        if ((arr[i] == -1) && (foundh < H)){
            printf("Hi I am process %d with return arg %d and I found the hidden key in position A[%d]\n", pid, rarg, i);
            foundh++;
        }
    }
    int avg = sum/size;
    printf("Max=%d, Avg=%d", max, avg);
   
    free(arr);
    return 0;
}