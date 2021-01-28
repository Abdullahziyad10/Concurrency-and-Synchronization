

## 1. Bounded-buffer Producer/Consumer problem:
In this program, we have to manage an array in which producer and consumer are inserting and removing data. So, ensuring that there is no clash between consumer and producer while doing there work we use semaphore for critical section. And also use semaophores to signal consumer and producer to start there work according to conditions. Like Consumer will only work when there is data in shared array and Producer will only work when there is empty space in shared array.And all this semphore singal and wait is controlled by Main Function. 

## To run the program 

$ gcc q1.c -lpthread -o q1

$ ./q1
											
---------------------------------------------------------------
## 2. Mother Hubbard (children, mother, father):
In this problem, we have to use some sempahores which will ensure that Mother has done her job for children and now it's Father time to woke up and do his work. When Father have done his work then Mother will woke up. And it will continue in this way till input days.
	

# To run the program 

$ gcc mh.c -lpthread -o mh

$ ./mh 100 

--------------------------------------------------------------										

## 3. Airline Passengers:
In this problem, there are some actors like Passengers, Baggage Handler, Security Screeners and Flight Attendants. Each actors have array which show them that there are passengers on their side and they have to do their work. So, they(passneger) can proceed to next point. For fast wokring, each actor is working parallel (threads). And they get signal everytime when last point send passnenger towards them. If there is no passenger then they will stop. Otherwise keep working till they server last passenger. 

# To Run the Program 

$ gcc Airline.c -lpthread -o Airline

$ ./Airline 10 3 5 2 
