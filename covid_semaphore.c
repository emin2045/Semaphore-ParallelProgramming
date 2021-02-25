#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_STAFF 8		   // Heathcare Staff that is Working at Hospital
#define MAX_EMPTYSEAT 3	   // Room Capacity
static int MAX_PEOPLE = 0; // It Determined by User Input
static int PRE_VALUE = 0;  // For Clock Operations
static int HOSPITAL_OPENING_CLOCK = 9;

sem_t semUnit[MAX_STAFF]; // Unit Semaphore
sem_t semPeople;		  // People Semaphore

void *people(void *id); // People Thread Function Header
void *staff(void *id);	// Heathcare Staff Thread Function Header

int choice = 0;
int randomNumGenerator(int mod, int choice); // Random Number Generator Function Header

int isAllTestFinished = 0;

void *people(void *id) // People Thread Function Implementation
{
	int ID = *(int *)id;
	printf("People %d: is going to hospital.\n", ID + 1);

	sem_wait(&semPeople); // Lock the Semaphore for People

	int anno; // Announcement of Direction

	int seats = MAX_EMPTYSEAT;
	int emptySpace;

	for (int i = 0; i < MAX_STAFF; i++)	// Checking Seats' Situation
	{
		sem_getvalue(&semUnit[i], &emptySpace);
		if (emptySpace != 0 && seats >= emptySpace)
		{
			seats = emptySpace;
			anno = i;
		}
	}

	sem_post(&semPeople); // Unlock the Semaphore for People

	int SValue;
	sem_getvalue(&semUnit[anno], &SValue);

	if (SValue == 3) // There is Nobody, Healthcare Staff is Waiting
	{
		sem_wait(&semUnit[anno]);
		printf("People %d: is entering %dth unit. Then, S/he is going to warning healthcare staff if s/he is ventilating!\n", ID + 1, anno + 1);
	}

	else
	{
		sem_wait(&semUnit[anno]);
		printf("People %d: is entering %dth unit.\n", ID + 1, anno + 1);
	}

	if (ID + 1 == MAX_PEOPLE)
	{
		isAllTestFinished = 1;
	}
	pthread_exit(NULL);
}

void *staff(void *id) // Staff Thread Function Implementation
{
	int U_ID = *(int *)id;
	int SValue;
	sem_getvalue(&semUnit[U_ID], &SValue); // Get S value

	while (isAllTestFinished != 1)
	{
		sem_getvalue(&semUnit[U_ID], &SValue); // Get S value

		switch (SValue)
		{
		case 0: // SValue = 0 ----> Here is Full
			randomNumGenerator(2, 1);

			for (int i = 0; i < 3; i++)
			{
				sem_post(&semUnit[U_ID]);
			}

			printf("%dth Unit is empty right now. It's ready to use!\n", U_ID + 1);
			break;

		case 1: // SValue = 1 ----> There are two People
			printf("Attention Please! The last people %d, let's start! Please, pay attention to your social distance and hygiene; use a mask in %dth unit!\n", SValue, U_ID + 1);
			randomNumGenerator(2, 1);
			break;
		case 2: // SValue = 2 ----> There is one People
			printf("Attention Please! The last people %d, let's start! Please, pay attention to your social distance and hygiene; use a mask in %dth unit!\n", SValue, U_ID + 1);
			randomNumGenerator(2, 1);
			break;

		case 3: // SValue = 3 ----> There is Nobody
			printf("Healtcare Staff %d: is ventilating %dth unit.\n", U_ID + 1, U_ID + 1);
			randomNumGenerator(2, 1);
			break;
		}

		randomNumGenerator(3, 1);
	}

	printf("%dth Unit will be closed shortly after.\n", U_ID + 1);

	pthread_exit(NULL);
}

int randomNumGenerator(int mod, int choice)
{
	if (choice == 0)
	{
		int sleepTime = rand() % mod; // Random int [0,mod)
		sleep(sleepTime);

		PRE_VALUE += sleepTime;

		if (PRE_VALUE >= 60)
		{
			HOSPITAL_OPENING_CLOCK++;
			PRE_VALUE = PRE_VALUE - 60;
		}

		return PRE_VALUE;
	}
	else
	{
		int sleepTime = rand() % mod; // Generate a Random Number Between 0 and Mod
		sleep(sleepTime);

		return sleepTime;
	}
}

int main(int argc, char *argv[])
{
	printf("Enter a number according to how many people you want to come to the hospital:\n--> ");
	scanf("%d", &MAX_PEOPLE);

	pthread_t p_tid[MAX_PEOPLE]; // People thread
	pthread_t staffs[MAX_STAFF]; // Staff thread

	int Number[MAX_PEOPLE];

	for (int i = 0; i < MAX_PEOPLE; i++) // Initialize People ID
	{
		Number[i] = i;
	}

	for (int i = 0; i < MAX_STAFF; i++)
	{
		sem_init(&semUnit[i], 0, 3); // SValue = 3 ---- People Semaphore Init
	}

	sem_init(&semPeople, 0, 1); // SValue = 1 ---- People Semaphore Init

	for (int i = 0; i < MAX_STAFF; i++)
	{
		pthread_create(&staffs[i], NULL, staff, (void *)&Number[i]); // Threads Creation Operation For Staffs
	}

	for (int i = 0; i < MAX_PEOPLE; i++)
	{
		/********************************************* Console Design **********************************************/
		if (HOSPITAL_OPENING_CLOCK == 9 && PRE_VALUE < 10)
		{
			printf("\nSomeone got suspicious. Hour: 0%d:0%d\n", HOSPITAL_OPENING_CLOCK, randomNumGenerator(2, 0));
		}
		else if (HOSPITAL_OPENING_CLOCK == 9 && PRE_VALUE >= 10)
		{
			printf("\nSomeone got suspicious. Hour: 0%d:%d\n", HOSPITAL_OPENING_CLOCK, randomNumGenerator(2, 0));
		}
		else if (HOSPITAL_OPENING_CLOCK != 9 && PRE_VALUE < 10)
		{
			printf("\nSomeone got suspicious. Hour: %d:0%d\n", HOSPITAL_OPENING_CLOCK, randomNumGenerator(2, 0));
		}
		else if (HOSPITAL_OPENING_CLOCK != 9 && PRE_VALUE >= 10)
		{
			printf("\nSomeone got suspicious. Hour: %d:%d\n", HOSPITAL_OPENING_CLOCK, randomNumGenerator(2, 0));
		}
		/********************************************* Console Design **********************************************/

		pthread_create(&p_tid[i], NULL, people, (void *)&Number[i]); // Threads Creation Operation For People
	}

	/****************************************************  Join  ***************************************************/
	for (int i = 0; i < MAX_PEOPLE; i++)
	{
		pthread_join(p_tid[i], NULL);
	}
	for (int i = 0; i < MAX_STAFF; i++)
	{
		pthread_join(staffs[i], NULL);
	}
	/****************************************************  Join  ***************************************************/

	return 0;
}