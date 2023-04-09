#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <math.h>
#include <signal.h>

int main( int argc, char *argv[] )  {

    long L = strtol(argv[1], NULL, 10); // number of positive integers in txt file (>=10000) test=10000
    long H = strtol(argv[2], NULL, 10); // number of hidden keys to find (<20)
    long NP = strtol(argv[3], NULL, 10); // max number of processes
    if(H>19){
        H=19;
        printf("Your command argument for hidden keys to find is too high and has been limited to 19\n");
    }
    if(NP>50){
        NP=50;
        printf("Your command argument for processses to use is too high and has been limited to 50\n");
    }
    int foundh = 0;
    int maxpipe[2], hpipe[2], sumpipe[2]; //create necessary pipes
    pipe(maxpipe);
    pipe(hpipe);
    write(hpipe[1], &foundh, sizeof(foundh));
    pipe(sumpipe);

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

    //
    //one child to each parent solution
    //
    
    int rarg = -1;
    int relation = 0;
    
    //creates all processes
    for (i = 1; i < NP; i++) {
        
        if (relation == 0){
            rarg = rarg + 1;
            relation = fork();      
        }
    }
    if (relation == 0){ //gives final process correct rarg;
        rarg = rarg + 1;     
    }

    int pid = getpid();
    int ppid = getppid();
    printf("Hi I am process %d with return arg %d and my parent is %d\n", pid, rarg, ppid);
    
    
    int scan_size = ceil(size/NP);
    int startpoint = rarg*scan_size;
    int sum = 0;
    int max = 1;
    //analyze array most processes
    if (rarg < NP - 1){
        int endpoint = startpoint + scan_size;
        for (i = startpoint; i < endpoint; i++) {
        sum = sum + arr[i];

        //find max
        if (arr[i] > max){
            max = arr[i];
            
        }

        //find hidden keys
        if ((arr[i] == -1) && (foundh < H)){
            read(hpipe[0], &foundh, sizeof(foundh)); 
            foundh = foundh+1;
            write(hpipe[1], &foundh, sizeof(foundh));
            if((foundh - 1) < H)
                printf("Hi I am process %d with return arg %d and I found the hidden key in position A[%d]\n", pid, rarg, i);
        }
        }
    }

    //analyze array with last process (because of ceiling function of scan_size this process has unique size to analyze)
    if (rarg == (NP - 1)){
        
        for (i = startpoint; i < size; i++) {
            sum = sum + arr[i];
            
            //find max
            if (arr[i] > max){
                max = arr[i];
                
            }

            
            //find hidden keys
            if ((arr[i] == -1) && (foundh < H)){
                read(hpipe[0], &foundh, sizeof(foundh)); 
                foundh = foundh+1;
                write(hpipe[1], &foundh, sizeof(foundh));
                if((foundh - 1) < H)
                    printf("Hi I am process %d with return arg %d and I found the hidden key in position A[%d]\n", pid, rarg, i);

            }
        }
    }


    wait(NULL); //wait for all processes to stop scanning

    if(rarg>0){ //kill all children off original process and send their max sums to pipe
        write(sumpipe[1], &sum, sizeof(sum));
        write(maxpipe[1], &max, sizeof(max));
        kill(pid, SIGKILL);
        printf("Kill unsuccessful, return value: %d\n", rarg);
    }
        close(maxpipe[1]);
        close(sumpipe[1]);
        close(hpipe[1]);
    //find max and avg of text file
    if(rarg == 0){ 
        int tempsum =0;
        int tempmax =1;
        
        for (i = 0; i < NP-1; i++) {
            read(maxpipe[0], &tempmax, sizeof(tempmax));
            if(max <= tempmax)
                max = tempmax;
        }
        for (i = 0; i < NP-1; i++) {
            read(sumpipe[0], &tempsum, sizeof(tempsum));
            sum = sum + tempsum;
        }  

    int avg = sum/size;
    printf("Max=%d Avg=%d", max, avg);
    } 
    
    return 0;
}