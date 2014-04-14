// SO2 IS1 211A LAB08
// Patryk Siemiński
// psieminski@wi.zut.edu.pl

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MAX_PRIORITY 10;

struct msgbuf {
	long mtype;
	int maxRandom;
	int clientQID;
};

struct responsebuf {
	long mtype;
	int randomN;
};

int serverQID;
int result;
int myPID;
int randomN;

int main(int argc, char** argv) {
	srand(time(NULL));

	myPID = getpid();
	serverQID = msgget(myPID, IPC_CREAT|0600);

	printf("Server QID : %d\n",serverQID);

	struct msgbuf* msg = malloc(sizeof(struct msgbuf));
	while((msgrcv(serverQID,msg,sizeof(struct msgbuf),-5,0)))
	{
		randomN = rand() % msg->maxRandom + 1;
		struct responsebuf* rmsg = malloc(sizeof(struct responsebuf));
		rmsg->mtype=1;
		rmsg->randomN=randomN;
		if (msgsnd(msg->clientQID, rmsg, sizeof(struct responsebuf), 0) == -1){
			printf("Nie udalo sie wyslac wiadomosci od serwera do klienta.\n");
			exit(1);
		}
	}
	return (0);
}
