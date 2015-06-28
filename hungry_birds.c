#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#define FOOD         20000
#define BABY_BIRDS   8
#define FATHER_BIRD  8
#define TIMES        5

typedef struct bird {
	int number;
	int consumed;
} Bird;

sem_t wake_father_bird, food_bowl;
int food, times, count;
pthread_mutex_t lock;

void *feed(void*);
void initiate_birds(Bird*);

int main(int argc, char **argv)
{
	int i;
	pthread_t *bird_threads;
	Bird *args;

	bird_threads = malloc((BABY_BIRDS + 1) * sizeof(*args));
	args = malloc((BABY_BIRDS + 1) * sizeof(*args));

	initiate_birds(args);
	food = FOOD, times = 0, count = 0;

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\nLock initialization failed.\n");
		exit(1);
	}

	if(sem_init(&wake_father_bird, 0, 1))
	{
		printf("Error creating semaphore.\n");
		return -1;
	}

	if(sem_init(&food_bowl, 0, 1))
	{
		printf("Error creating semaphore.\n");
		return -1;
	}

	for(i = 0; i <= BABY_BIRDS; i++)
	{
		if (pthread_create(&bird_threads[i], NULL, feed, &args[i]))
		{
			printf("Error creating thread.");
			abort();
		}
	}
	for(i = 0; i <= BABY_BIRDS; i++)
	{
		if (pthread_join(bird_threads[i], NULL))
		{
			printf("Error joining thread.");
			abort();
		}
	}

	free(args); args = NULL;
	free(bird_threads); bird_threads = NULL;
	sem_destroy(&wake_father_bird);
	sem_destroy(&food_bowl);
	return 0;
}


void *feed(void *args)
{
	Bird *bird = ((Bird*) args);

	if(bird->number == FATHER_BIRD)
	{
		sem_wait(&wake_father_bird);
		while(1)
		{
			sem_wait(&wake_father_bird);
			times++;			
			food = FOOD;
			if(times != TIMES) {
				printf("The father bird woke up. Time to fill the bowl of food one more time.\n"); 
				printf("The father bird is tired. Time to take a nap... ZZZzzzZZZzzzZZZ...\n");
			}
			else {
				printf("The father bird woke up. He realized he is out of food. Simulation is over.\n");
				break;
			}
		}
	}
	else
	{
		while(1)
		{
			sem_wait(&food_bowl);
			if(times != TIMES)
			{
				printf("Baby bird %d is feeding itself... Delicious... :3\n", bird->number);
				food--;
				bird->consumed++;
				if(food == 0) {
					printf("Oh, the food is over. Time to wake papa bird!\n");
					sem_post(&wake_father_bird); 
					while(food == 0) continue; 
				}
			}
			sem_post(&food_bowl);
			printf("Baby bird %d is tired... He is too full. Time to take a nap... \n", bird->number);
			if(times == TIMES) break;
		}
	}

	pthread_mutex_lock(&lock);
		count++;
	pthread_mutex_unlock(&lock);
	while(count != BABY_BIRDS + 1) continue;

	if(bird->number != FATHER_BIRD) printf("Baby bird %d consumed: %d\n", bird->number, bird->consumed);
	return NULL;
}

void initiate_birds(Bird *args)
{
	int i;

	for(i = 0; i < BABY_BIRDS; i++)
	{
		args[i].number = i;
		args[i].consumed = 0;
	}

	args[FATHER_BIRD].number = FATHER_BIRD;
}