//��������� ��������
#include <process.h>
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <time.h>
#include <omp.h>
 
 
#define N 5             	//����� ���������
#define LEFT 	(i-1)%N    	//����� ����� �������� � ������� i
#define RIGHT 	(i+1)%N   	//������ ����� �������� �������� i


enum States {
	THINKING = 0, HUNGRY = 1, EATING = 2
};
                        
int state[N];           //��������� ������� ��������
  
struct Philosopher {     //�������� ��������: �����
    int number;
};
 
struct Forks {           //��������� ���������� ����� � ��������
    int left;           //0-��� ����� 
    int right;          //1-���� �����
} 	forks[N];
 
 
CRITICAL_SECTION cs;        //��� ����������� ������: �������. ���������    
CRITICAL_SECTION cs_forks;  //� �����. �����
 
HANDLE philMutex[N];    	//������� �������� �� ��������
HANDLE forkMutex[N];    	//� ������ �����
 
void think (int i) {      	//���������� ����������� ��������
    EnterCriticalSection( &cs );    //���� � ����������� ������
    std::cout << "Philosopher number " << i << " thinking" << std::endl;
    LeaveCriticalSection( &cs );    //����� �� ����������� ������
}
 
void eat (int i) {
    EnterCriticalSection( &cs );    
    std::cout << "Philosopher number " << i << " eating" << std::endl;
    LeaveCriticalSection( &cs );    
}
 
void test (int i) {   //�������� ����������� ������ ��������� ����
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[i] = EATING;
        ReleaseMutex(philMutex[i]); //����������� ������
    }
}
 
void take_forks (int i) { //������ �����
    EnterCriticalSection( &cs_forks );              //���� � ����������� ������
    state[i] = HUNGRY;                              //�������� ������� ��������� ��������
    test(i);                                        //������� �������� ��� �����
    LeaveCriticalSection( &cs_forks );              //����� �� ����������� ������
    WaitForSingleObject( philMutex[i], INFINITE );  //���������� ���� ����� �� ���������
}
 
void put_forks(int i) {  				//������� ������ ����� �������
    EnterCriticalSection( &cs_forks );  //���� � ����������� ������ 
    state[i] = THINKING;                //������� �������� ����
    test(LEFT);                         //��������� ����� �� ���� ����� �����
    test(RIGHT);                        //��������� ����� �� ���� ����� ������
    LeaveCriticalSection( &cs_forks );  //����� �� ����������� ������
}
 
void take_left_fork(int i) { 							//������� ����� ����� �����
    EnterCriticalSection( &cs );
    WaitForSingleObject( forkMutex[LEFT], INFINITE );   //���� ����� ��������   
    forks[i].left = 1;                                  //����� ��
    LeaveCriticalSection( &cs );
}
 
void take_right_fork (int i) { //������� ����� ������ �����
    EnterCriticalSection( &cs );
    WaitForSingleObject( forkMutex[RIGHT], INFINITE );  //���� ����� ��������   
    forks[i].right = 1;                                 //����� ��
    LeaveCriticalSection( &cs );
}

void put_left_fork (int i) {  //������ ����� �����
    WaitForSingleObject( forkMutex[LEFT], INFINITE );   
    forks[i].left=0;            //������ �����
    ReleaseMutex( forkMutex[LEFT] );
}
  
void put_right_fork (int i) { 			//������ ������ �����
    WaitForSingleObject( forkMutex[RIGHT], INFINITE );  
    forks[i].right = 0;           		//������ �����
    ReleaseMutex( forkMutex[RIGHT] );
}

int test_fork (int i) {   //��������� ������� � �������� ����� �����
    if (forks[i].left == 1 && forks[i].right == 1) { //���� ���
        ReleaseMutex( forkMutex[LEFT] );    //��������� �������� �����
        ReleaseMutex( forkMutex[RIGHT] );   //
        return 1;                                   //����
    }
    else
        return 0;
}

void philosopher(Philosopher phil) { //���������� ������ ��������
    
    while (1) {   //���������� ����
       	//����� �����
        int n = rand()%2+1;
        think(phil.number); //������
        
		if (n == 1) {
            take_left_fork(phil.number);        //����� ����� ��������
            take_right_fork(phil.number);
        } else {
            take_right_fork(phil.number);
            take_left_fork(phil.number);
        }
                
        if (test_fork(phil.number) == 1)
            eat(phil.number);
        
        
        //������ ����� ��������
        if (forks[phil.number].left == 1 && forks[phil.number].right == 1) {
            int n = rand()%2;
            if (n == 1) {
                put_left_fork(phil.number);
                put_right_fork(phil.number);
            } else {  //��������� �������
                put_right_fork(phil.number);
                put_left_fork(phil.number);
            }
        }
        
        Sleep(10);  //���� ����� �� ������������ ���������
    }
	
}
   
int main() {
    srand(time(NULL));  //��� �� ����� ���� ������������� ����������
    Philosopher phil[N];
    
    for (int i = 0; i < N; i++) { //������������� � ��������� ��� �����
        forks[i].left=0;
        forks[i].right=0;
    }
    
    for(int i = 0; i < N; i++) {
        phil[i].number = i;
	}
	
	for (int i = 0; i < N; i++) { //������� ��������
        philMutex[i] = CreateMutex( NULL, FALSE, NULL );
        forkMutex[i] = CreateMutex(NULL, FALSE, NULL);
    }
    
	InitializeCriticalSection( &cs );       //��������������
    InitializeCriticalSection( &cs_forks ); //����������� ������
    
    #pragma omp parallel for num_threads (5)
    for (int i = 0; i < 5; i++) {
		philosopher(phil[i]);
	}
    
    Sleep(INFINITE);    //����� ������ ������ ���������� � ����������� ���������� 
    
}
