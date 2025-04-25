#include "header.h"


int randomValue(int min, int max);

char *fifo = "/tmp/cal_fifo";

int main(int args , char ** argv){
    srand(time(NULL) ^ getpid());


    if (args < 2){
        perror("there is no min max value");
    }
   int  min_range = atoi(argv[1]);
    int max_range = atoi(argv[2]);
    char message[BUFSIZ];


   // printf("hello from child %d get %s\n",getpid(),argv[0]);

    int f;
    int energy;
    float range;
    if ((f = open(fifo, O_RDWR )) == -1) {
        perror("fifo open error");
        exit(-3);
    }

    else {
        energy = randomValue(min_range,max_range);

        for (int i = 0; i < 20; i++) //
        {
            range = (float)randomValue(min_range,max_range)/max_range;

            if(energy < 0 || (range< 0.02) )
            energy = 0;
        
        
        sprintf(message,"%d:%d",energy--,getpid());
       // printf("the message sent is %s\n",message);
        if(write(f,message,sizeof(message))==-1){
            perror("write error");
            exit(-2);
        }
        sleep(1);
    }
    }
   // sleep(8);
    close(f);


    return 0;



}


int randomValue(int min , int max){
 int value =-1;

  if ( min == max ) {
    printf("Please provide a range that is not equal to 0!\n");
    return value;

    exit(-2);
  }


    value = (int) (min + (rand() % (max - min)));

   // printf("value = %d\n", value);
    return value;
  

}