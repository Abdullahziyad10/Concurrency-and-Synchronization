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
sem_t  s_baggage;		//sempahore
sem_t  s_security;	
sem_t  s_flight_attendant;	
sem_t  mutex;
sem_t  mutex2;
sem_t  mutex3; 

int p_count = 0; //counters
int b_count = 0;
int s_count = 0;
int f_count = 0;

typedef struct integer  //reperesent a passenger no
{
	int passenger_no;
}integer;

typedef struct queue //struct Queue
{
	int * queue;	//queue (bounded-buffer)
	int front;
	int rear;
	int itemCount;
	int total_size;
} Queue;


//intializing Queue

Queue baggage = { .queue = NULL,.front = 0,.rear = -1,.itemCount = 0,.total_size = 0 };
Queue security = { .queue = NULL,.front = 0,.rear = -1,.itemCount = 0,.total_size = 0 };
Queue flight_attendant = { .queue = NULL,.front = 0,.rear = -1,.itemCount = 0,.total_size = 0 };

bool isEmpty(Queue * q) { //checking if Queue is empty
   return q->itemCount == 0;
}

bool isFull(Queue * q) { //checking if Queue is full
   return q->itemCount == q->total_size;
}

int size(Queue * q) {  //checking the size ofQueue
   return q->itemCount;
}  

void enqueue_item(Queue * q,int data) { //inserting character in a Queue

   if(!isFull(q)) {
	
      if(q->rear == q->total_size-1) {
         q->rear = -1;            
      }       
      q->rear++;
      q->queue[q->rear] = data;
      
      
      q->itemCount++;
   }
}

int dequeue_item(Queue * q) {  // deleting charcter from a Queue

   if(!isEmpty(q)){
      int data = q->queue[q->front];
    
      q->front++; 
	
      if(q->front == q->total_size) {
         q->front = 0;
       }
	
      q->itemCount--;
      return data; 
   } 
}


void * baggage_program(void * param) //baggage thread
{
	sem_wait(&mutex); // mutex for critical section so no one increase count same time
	int flag=b_count < p_count;
	if(flag)
	{
		b_count++; //counter so baggage deal all passenger
	}
	sem_post(&mutex);
	while(flag)
	{
		
		sem_wait(&s_baggage);
		
		sem_wait(&mutex);
		int x = dequeue_item(&baggage);  //checking which passenger is now came to baggage section
		
		sleep(1);
		printf("Passenger # %d is waiting at baggage processing for a handler.\n",x);
		
		sem_post(&mutex);
		
		sem_wait(&mutex2);
		enqueue_item(&security,x); //sending to security side
		
		sleep(1);
		printf("Passenger # %d got handler now going for security.\n",x);
		sem_post(&mutex2);
		
		sem_post(&s_security); //giving signal to secuirty to become ready for passenger
		
		sem_wait(&mutex);
		flag=b_count < p_count;
		if(flag)
		{
			b_count++;
		}
		sem_post(&mutex);
	}
	
}

void * passenger_program(void * param) //passenger thread
{
	integer * p = (integer *) param; //getting number
	sleep(1);
	printf("Passenger # %d arrived at the terminal.\n",(*p).passenger_no);
	int x = (*p).passenger_no;
	
	
	sem_wait(&mutex);
	
	enqueue_item(&baggage,x);  //sending towards baggage process
	sleep(1); 
	printf("Passenger # %d is going towards baggage processing for a handler.\n",x);
	sem_post(&mutex);
	sem_post(&s_baggage); //giving signal baggage to become ready for passenger
	
}

void * security_program(void * param) //security thread
{
	sem_wait(&mutex2); // mutex for critical section so no one increase count same time
	int flag=s_count < p_count;
	if(flag)
	{
		s_count++;
	}
	sem_post(&mutex2);
	while(flag)
	{
		
		sem_wait(&s_security);
		sem_wait(&mutex2);
		int x = dequeue_item(&security); //checking which passenger is now came to security section
		
		sleep(1);
		printf("Passenger # %d is waiting to be screened by a screener.\n",x);
		
		sem_post(&mutex2);
		
		sem_wait(&mutex3);
		enqueue_item(&flight_attendant,x); //sending towards flight attendant
		
		sleep(1);
		printf("Passenger # %d is screened by a screener.\n",x);
		sem_post(&mutex3);
		
		sem_post(&s_flight_attendant); //giving signal to flight attendant to become ready for passenger
		
		sem_wait(&mutex2);
		flag=s_count < p_count;
		if(flag)
		{
			s_count++;
		}
		sem_post(&mutex2);
	}
	
}

void * flight_attendant_program(void * param)
{
	sem_wait(&mutex3); // mutex for critical section so no one increase count same time
	int flag=f_count < p_count;
	if(flag)
	{
		f_count++;
	}
	sem_post(&mutex3);
	while(flag)
	{
		
	  sem_wait(&s_flight_attendant);
		sem_wait(&mutex3);
		int x = dequeue_item(&flight_attendant); //checking which passenger is now came to flight section
		
		sleep(1);
		printf("Passenger # %d is waiting to board the plane by an attendant.\n",x);
		
		sem_post(&mutex3);
		
		sleep(1);
		printf("Passenger # %d has been seated and relaxes.\n",x);
		
		sem_wait(&mutex3);
		flag=f_count < p_count;
		if(flag)
		{
			f_count++;
		}
		sem_post(&mutex3);
	}
	
}

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		printf("Invalid arguments\n");
		exit(0);
	}
	
	
	//giving size to each actor
	
	int passenger_size = atoi(argv[1]);
	baggage.total_size = atoi(argv[1]);
	security.total_size = atoi(argv[1]);
	flight_attendant.total_size = atoi(argv[1]);
	
	p_count = passenger_size; //keeping check how many passenger came
	
	baggage.queue = (int *) malloc( sizeof(int)* baggage.total_size ); //initlaizing queue
	security.queue = (int *) malloc( sizeof(int)* security.total_size );
	flight_attendant.queue = (int *) malloc( sizeof(int)* flight_attendant.total_size );
	
 
	sem_init(&s_baggage, 0, 0); //initalizing semaphores
	sem_init(&s_security, 0, 0);
	sem_init(&s_flight_attendant, 0, 0); 
	sem_init(&mutex, 0, 1);
	sem_init(&mutex2, 0, 1);
	sem_init(&mutex3, 0, 1);
	
	
	pthread_t baggage_thread[baggage.total_size];  //threads
  pthread_t passenger_thread[passenger_size];
  pthread_t security_thread[security.total_size];
  pthread_t flight_attendant_thread[flight_attendant.total_size];
   
    ///////////////////creating threads//////////////////
   
  for( int i = 0; i < baggage.total_size ; i++) 
  	pthread_create(&baggage_thread[i],NULL,&baggage_program,NULL);
 
  for( int i = 0; i < security.total_size ; i++) 
  	pthread_create(&security_thread[i],NULL,&security_program,NULL);
  	
  for( int i = 0; i < flight_attendant.total_size ; i++) 
  	pthread_create(&flight_attendant_thread[i],NULL,&flight_attendant_program,NULL);
  
  
  
  for( int i = 0; i < passenger_size ; i++)
  {
  	integer * p_no = malloc(sizeof(integer));
  	(*p_no).passenger_no = i+1;
  	
  	pthread_create(&passenger_thread[i],NULL,&passenger_program,(void *) p_no);
  }
  	
  	/////////////////Joining Threads///////////////////////////
  	
  for( int i = 0; i < passenger_size ; i++) 
  	pthread_join(passenger_thread[i],NULL);
  	
  for( int i = 0; i < baggage.total_size ; i++) 
  	pthread_join(baggage_thread[i],NULL);
  
  for( int i = 0; i < security.total_size ; i++) 
  	pthread_join(security_thread[i],NULL);
  
  for( int i = 0; i < flight_attendant.total_size ; i++) 
  	pthread_join(flight_attendant_thread[i],NULL);
  	
  	////////destroying threads///////////

	sem_destroy(&s_baggage); 
	sem_destroy(&s_security);
	sem_destroy(&s_flight_attendant);
	sem_destroy(&mutex);
	sem_destroy(&mutex3);
	sem_destroy(&mutex3);
	
	/////// Free each array  ///////
	
	free(baggage.queue);
	free(security.queue); 
	free(flight_attendant.queue);

return 0;
}
	
	
