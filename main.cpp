//Обедающие философы
#include <semaphore.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <windows.h>
#include <omp.h>

#define log(x) std::cout << x << std::endl


#define LEFT(index) (index + length - 1) % length 
#define RIGHT(index) (index + 1) % length 
 
sem_t *mutex;
sem_t *array; 	// 0 1 2 3 4 5
int *state; 	// 3 3 3 3 3 3
int length;
 
enum states {
	HUNGRY = 0, THINKING = 1, EATING = 2, SITTING = 3
};
 
bool isSpoonsFree(int index) {
	if (state[index] == HUNGRY && state[LEFT(index)] != EATING && state[RIGHT(index)] != EATING) {
		state[index] = EATING;
		sem_post(&array[index]);
	}
}

void takeSpoons(int index) {
	log("1");
	sem_wait(mutex); 
  	log("2");
	state[index] = HUNGRY;
	log("3"); 
  	isSpoonsFree(index);
	log("4");   
  	sem_post(mutex);
	log("5"); 
  	sem_wait(&array[index]);
  	log("6");
}

void putSpoons(int i) {
    sem_wait(mutex); 
    state[i] = THINKING;
    isSpoonsFree(LEFT(i)); 
    isSpoonsFree(RIGHT(i)); 
  	sem_post(mutex);
}	

void eat(int index) {
	int time = rand() % length;
	printf("Philosopher %d is eating!\n", index);
	Sleep(time);
	printf("Philosopher %d stop eating!\n", index);
}

void think(int index) {
	int time = rand() + length;
	printf("Philosopher %d is thinking!\n", index);
	Sleep(time);
	printf("Philosopher %d stop thinking!\n", index);
} 

void process(int index) {
	think(index);
	takeSpoons(index);
	eat(index);
	putSpoons(index);
}
 
int main() {
    
    std::cout << "Write number of philosophers: "; std::cin >> length;
    
 	array = (sem_t*)calloc(length, sizeof(sem_t));
 	state = (int*)calloc(length, sizeof(int));
 	
 	sem_init(mutex, 0, 0);
 	sem_init(array, 0, 0);
 	
	for (int i = 0; i < length; i++) {
  		state[i] = SITTING;
	}

	//food = length + 1;
	
	#pragma omp parallel for num_threads(1)
	for (int i = 0; i < length; i++) {
		process(i);
	} 	
	
 	 
    
    std::cin.get();    
    return 0;
}
