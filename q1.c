#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/stat.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h>
#define MAX_SIZE 500

sem_t  s_p; //sempahores
sem_t  s_c;
sem_t  s_mutex;
sem_t  full;
sem_t  empty;

int p_count = 0; //count to stop
int c_count = 0;
int total_time = 0 ;


char * queue;  // Queue Bounded Buffer
int front = 0;
int rear = -1;
int itemCount = 0;
int total_size;



bool isEmpty() {//checking if queue empty
   return itemCount == 0;
}

bool isFull() {//checking if queue is full
   return itemCount == total_size;
}

int size() {  //return queue current size
   return itemCount;
}  

void enqueue_item(char data) { //inserting a character

   if(!isFull()) {
	
      if(rear == total_size-1) {
         rear = -1;            
      }       
      rear++;
      queue[rear] = data;
      itemCount++;
   }
}

char dequeue_item() {//deleting a character from a queue

   if(!isEmpty()){
      char data = queue[front];
      front++; 
	
      if(front == total_size) {
         front = 0;
       }
	
      itemCount--;
      return data; 
   } 
}

void * produce ( void * param)  //producer progrma
{
	while(1)
	{
		sem_wait(&s_p); //one producer can work at a time
		sem_wait(&s_mutex); //critical section start
		
		char randomletter = 'A' + (random() % 26); //generating random character
		enqueue_item(randomletter);
		
		sleep(1); //sleep so we can observe output easily
		
		printf("%c is produced by producer\n",randomletter);
		
		p_count++;  //count to stop after given time
		if(p_count == total_time)
		{
			sem_post(&s_mutex);
			pthread_exit(NULL);
		}
		
		sem_post(&s_mutex);  ///critical secion end
		
	}
	
}

void * consume (void * param) //consumer program
{
	while(1){
		sem_wait(&s_c); ///one consumer at a time
		sem_wait(&s_mutex); //critical section
		
		char x = dequeue_item();
		
		sleep(1); //sleep to observe output easily
		
		printf("%c was consumed by consumer\n",x);
		
		c_count++;//count to stop after given time
		if(c_count == total_time)
		{
			sem_post(&s_mutex);
			pthread_exit(NULL);
		}
		sem_post(&s_mutex); //critical section end
	}
}



int main(int argc, char *argv[])
{
	total_time = 20; //that much character produce and consume
	
	queue = (char *) malloc( sizeof(char)* MAX_SIZE );  //initalizing queue
  total_size = MAX_SIZE;
 
	sem_init(&s_p, 0, 0); //initliazing semaphore
	sem_init(&s_c, 0, 0); 
	sem_init(&s_mutex, 0, 1); 	
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, 0);
	
	pthread_t consumer_thread; //threads
  pthread_t producer_thread;
    
  pthread_create(&consumer_thread,NULL,&consume,NULL); //thread create
  pthread_create(&producer_thread,NULL,&produce,NULL);
  
  while(1) //controlling producer and consumer
  {
  	sem_wait(&s_mutex); //critical section start
  	
  	if(!isEmpty() && !isFull())
  	{
  		sem_post(&s_c);
  		sem_post(&s_p);
  	}
  	else
  	{
  		if(isFull())
  		{
  			sem_post(&s_c);
  		}
  		if(isEmpty())
  		{
  			sem_post(&s_p);
  		}
  	}
  	
  	if(p_count == total_time && total_time == c_count) //break if produce and consume all 
  	{
  		break;
  	}
  	sem_post(&s_mutex);//critical section end
  	
  	sleep(1); //sleep to observe
  }
    
  pthread_join(consumer_thread,NULL); //threads join
  pthread_join(producer_thread,NULL);

	sem_destroy(&s_p); //sempahore destroy
	sem_destroy(&s_c);
	sem_destroy(&s_mutex);
	
	free(queue);//free queue space

return 0;
}
	
	
