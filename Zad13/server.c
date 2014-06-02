// SO2 IS1 211A LAB13
// Patryk Siemiński
// psieminski@wi.zut.edu.pl

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SERVER 269713
#define USERS 25

key_t id;
int semaphoreId, segmentId;

struct contacts {
	char nicknames[25][10];
	int queueId[25];
};

struct contacts* buff;

void initiateContacts(struct contacts* serverContacts)
{
    int i;
	for (i=0; i<USERS; i++)
	{
		sprintf(serverContacts->nicknames[i],"");
		serverContacts->queueId[i]=0;
	}
}

int main(int argc, char** argv) {

	segmentId = shmget(SERVER,sizeof(struct contacts) , IPC_CREAT | 0777);
	if (segmentId < 0)
	{
		printf("SegmentId generation failure.\n");
		exit(1);
	}
	
	semaphoreId = semget(SERVER, 2, IPC_CREAT | 0666);
	if (semaphoreId < 0)
	{
		printf("Semaphore creation failure.\n");
		exit(2);
	}

	semctl(semaphoreId,0,SETVAL,0);

	printf("SegmentID : %d , SemaphoreID : %d\n",segmentId, semaphoreId);

	if ((buff = shmat(segmentId, NULL, 0)) == (struct contacts *) -1) {
			printf("Memory segment attachment error.\n");
			exit(3);
	}

	initiateContacts(buff);

	printf("Press any button and ENTER to shut down server.\n");

	while(scanf("%d",0))
	{

	}

	shmdt(buff);
	semctl(semaphoreId, 0, IPC_RMID);

	return (0);
}
