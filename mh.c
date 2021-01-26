#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>


/*

						 HELPS IN OBESRVERING OUTPUTS 

*/



sem_t  father;		//producer sempahore
sem_t  mother;		//consumer semaphore
sem_t  mutex; //mutex

int cycle = 0; // #cycle/day

void * father_program ( void * param) //producer program
{
	
	for(int i = 0 ; i < cycle ; i++)
	{
		sem_wait(&father); //father at sleep
	
		printf("\nFather woke up.\n\n");
		
		for(int x = 0 ; x < 12 ; x++)
		{
			sem_wait(&mutex);
			printf("Child # %d is being read a book.\n",x+1);
			usleep(100);
		}
		
		for(int x = 0 ; x < 12 ; x++)
		{
			printf("Child # %d is being tucked in bed.\n",x+1);
			usleep(100);
		}
		
		printf("\nFather is going to sleep and waking up Mother to take care of the children.\n");
		
		sem_post(&mother); //waking mother up
	}
	
}

void * mother_program (void * param) //consumer program
{
	for( int i = 0 ; i < cycle ; i++)
	{
		sem_wait(&mother); //mother at sleep
		
		printf("\nThis is day #%d of a day in the life of Mother Hubbard.\n\n", i+1);		
		
		printf("Mother woke up and starting working while Father is sleeping.\n\n");
		
		for(int x = 0 ; x < 12 ; x++)
		{
			printf("Child # %d is being woken up.\n",x+1);
			usleep(100);
		}
		
		for(int x = 0 ; x < 12 ; x++)
		{
			printf("Child # %d is being fed breakfast.\n",x+1);
			usleep(100);
		}
		
		for(int x = 0 ; x < 12 ; x++)
		{
			printf("Child # %d is sent to school\n",x+1);
			usleep(100);
		}
		
		for(int x = 0 ; x < 12 ; x++)
		{
			printf("Child # %d is being given dinner.\n",x+1);
			usleep(100);
		}
		
		sem_post(&father); //waking up father
		
		for(int x = 0 ; x < 12 ; x++)
		{
			usleep(100);
			printf("Child # %d is being given a bath.\n",x+1);
			
			sem_post(&mutex); //giving one child is done from bath
		}
		
		printf("\nMother is going to sleep while Father is working.\n\n");
		
		
	}
}


int main(int argc, char *argv[])
{
	
	if (argc != 2)
	{
		printf("Invalid arguments\n");
		exit(0);
	}
	
	cycle = atoi(argv[1]);
 
	sem_init(&father, 0, 0); //initalizing semaphores
	sem_init(&mother, 0, 1); 
	sem_init(&mutex, 0, 0);
	
	pthread_t father_thread;  //threads
  pthread_t mother_thread;
    
  pthread_create(&father_thread,NULL,&father_program,NULL); //creating threads
  pthread_create(&mother_thread,NULL,&mother_program,NULL);
    
  pthread_join(father_thread,NULL); //joining threads
  pthread_join(mother_thread,NULL);

	sem_destroy(&father); //destroying threads
	sem_destroy(&mother);
	sem_destroy(&mutex);

return 0;
}
	
	
