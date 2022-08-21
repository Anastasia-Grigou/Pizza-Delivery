#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> 
#include "3180037-pizza2.h"

pthread_mutex_t  lock;
pthread_cond_t  cond;
pthread_mutex_t  lock1;
pthread_cond_t  cond1;
pthread_mutex_t  lock2;
pthread_cond_t  cond2;
struct timespec finish,delta;
struct timespec finish1,delta1;
struct thread_args {
	int id;
	int Tdel;
	struct timespec start,start1;
};
int max = 0;  
int sum = 0; 
int avtime;
int max1 = 0;  
int sum1 = 0; 
int avtime1; 

enum { NS_PER_SECOND = 1000000000 };

void sub_timespec(struct timespec t1, struct timespec t2, struct timespec *td)
{
    td->tv_nsec = t2.tv_nsec - t1.tv_nsec;
    td->tv_sec  = t2.tv_sec - t1.tv_sec;
    if (td->tv_sec > 0 && td->tv_nsec < 0)
    {
        td->tv_nsec += NS_PER_SECOND;
        td->tv_sec--;
    }
    else if (td->tv_sec < 0 && td->tv_nsec > 0)
    {
        td->tv_nsec -= NS_PER_SECOND;
        td->tv_sec++;
    }
}


 
void  *order(void * ptr){
	struct thread_args *a;
	a = (struct thread_args *) ptr;
	int id = a-> id;
	int Tdel = a-> Tdel;
	struct timespec start = a -> start;
	struct timespec start1 = a -> start1;
	int rc;
	int rc1;
	int rc2;

	rc = pthread_mutex_lock(&lock);
	while (Ncook == 0 ){
	rc = pthread_cond_wait(&cond, &lock);
	}
	Ncook--;
	rc = pthread_mutex_unlock(&lock);
	sleep(Tprep); 
	
	rc1= pthread_mutex_lock(&lock1);
	while(Noven ==0){
	rc = pthread_cond_wait(&cond1, &lock1);
	}
	Noven--;
	rc1 = pthread_mutex_unlock(&lock1);

	rc = pthread_mutex_lock(&lock);
	Ncook++;
	rc = pthread_cond_signal(&cond);
	rc = pthread_mutex_unlock(&lock);

	sleep(Tbake);
	clock_gettime(CLOCK_REALTIME, &start1);
	
	
	rc1 = pthread_mutex_lock(&lock1);
	rc2 = pthread_mutex_lock(&lock2);
	while(Ndeliverer == 0){
	rc = pthread_cond_wait(&cond2, &lock2);
	}
	Ndeliverer--;
	rc2 = pthread_mutex_unlock(&lock2);

	Noven++;
	rc1 = pthread_cond_signal(&cond1);
	rc1 = pthread_mutex_unlock(&lock1);

	sleep(Tdel);
	
	clock_gettime(CLOCK_REALTIME, &finish1);
	sub_timespec(start1, finish1, &delta1);
	if ( (int)delta1.tv_sec > max1){
		max1 =  (int)delta1.tv_sec;
	}

	sum1 = sum1 + (int)delta1.tv_sec;
	clock_gettime(CLOCK_REALTIME, &finish);
	sub_timespec(start, finish, &delta);
	if ( (int)delta.tv_sec > max){
		max =  (int)delta.tv_sec;
	}

	sum = sum + (int)delta.tv_sec;
 	printf("Η παραγγελία με αριθμό %d παραδόθηκε σε %d λεπτά και κρύωνε για %d λεπτά.\n", id, (int)delta.tv_sec, (int)delta1.tv_sec);

	sleep(Tdel);

	rc2 = pthread_mutex_lock(&lock2);
	Ndeliverer++;
	rc2 = pthread_cond_signal(&cond2);
	rc2 = pthread_mutex_unlock(&lock2);


	pthread_exit(NULL);
}

int main(int argc, char *argv[])   {
	int rc;
	int Ncust;
	int pizzas;
	int seed;
	int Time;
	Ncust = atoi(argv[1]);
	seed = atoi(argv[1]);
	struct thread_args Struct_thread[Ncust];
	int Torder[Ncust];
	pthread_t  threads[Ncust];
	pthread_mutex_init(&lock ,NULL);
	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&lock1 ,NULL);
	pthread_cond_init(&cond1, NULL);
	pthread_mutex_init(&lock2 ,NULL);
	pthread_cond_init(&cond2, NULL);

	
	for (int i=0;  i < Ncust;  i++)   {
		Torder[i]=0;
		if (i == 0){
			clock_gettime(CLOCK_REALTIME, &Struct_thread[i].start);
			Torder[i] =0;
		} 

		if (i != 0){
			while (Torder[i] < Torderlow){
			Torder[i] = rand_r(&seed) % (Torderhigh+1);
			}
			clock_gettime(CLOCK_REALTIME, &Struct_thread[i].start);
				sleep(Torder[i]);
		}
		Struct_thread[i].id = i+1;
		pizzas = 0;
		while (pizzas < Norderlow){
			pizzas = rand_r(&seed) % (Norderhigh+1);
			Tprep = pizzas;
		}
		Time = 0;
		while ((Struct_thread[i].Tdel < Tlow) || (Struct_thread[i].Tdel >Thigh)){
			Struct_thread[i].Tdel = rand_r(&seed) % (Thigh+1);
		}
		rc  = pthread_create(&threads[i] , NULL , order , (void *) &Struct_thread[i]);

	}

	
	for (int i =0; i < Ncust; i++) {
		pthread_join(threads[i] , NULL);
	}
	
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&lock1);
	pthread_cond_destroy(&cond1);
	pthread_mutex_destroy(&lock2);
	pthread_cond_destroy(&cond2);

	printf("Μέγιστος χρόνος ολοκλήρωσης των παραγγελιών: %d λεπτά \n" ,  max);
	printf("Μέγιστος χρόνος κρυώματος των παραγγελιών: %d λεπτά \n" ,  max1);
	
	avtime = sum / Ncust;
	
	avtime1 = sum1 / Ncust;
		
	printf("Μέσος χρόνος ολοκλήρωσης των παραγγελιών: %d λεπτά \n" , avtime);
	printf("Μέσος χρόνος κρυώματος των παραγγελιών: %d λεπτά \n" , avtime1);

	return 0;

}
