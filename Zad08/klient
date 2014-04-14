// SO2 IS1 211A LAB08
// Patryk Siemiński
// psieminski@wi.zut.edu.pl

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int serverQID;
int maxRandom;
int priority;
int myPID;
int myClientQID;

struct msgbuf {
	long mtype;
	int maxRandom;
	int clientQID;
};

struct responsebuf {
	long mtype;
	int randomN;
};

int main(int argc, char** argv) {

	if (argc<4)
	{
		printf("Nie podano odpowiedniej ilosci parametrow.\n");
		exit(0);
	}
	else
	{
		serverQID=atoi(argv[1]);
		maxRandom=atoi(argv[2]);
		priority=atoi(argv[3]);

		if ((maxRandom<1)||(maxRandom>30000))
		{
			printf("Argument 2, zakres losowania musi byc z przedzialu 1-30000\n");
		}
	}

	myPID = getpid();
	myClientQID = msgget(myPID, IPC_CREAT|0600);
	if (myClientQID == -1){
		printf("Nie udalo sie utworzyc kolejki klienta.\n");
		exit(1);
	}

	struct msgbuf * msg=malloc(sizeof(struct msgbuf));

	msg->mtype=priority;
	msg->maxRandom=maxRandom;
	msg->clientQID=myClientQID;

	if (msgsnd(serverQID, msg, sizeof(struct msgbuf), 0) == -1){
		printf("Nie udalo sie wyslac wiadomosci od klienta do serwera.\n");
		exit(2);
	}

	struct responsebuf* rmsg = malloc(sizeof(struct responsebuf));

	if (msgrcv(myClientQID,rmsg,sizeof(struct msgbuf),-5,0)){
		printf("Wylosowana liczba to : %d\n", rmsg->randomN);
	}
	else
	{
		printf("Nie udalo sie odebrac wiadomosci od serwera.\n");
	}

	if(msgctl(myClientQID,IPC_RMID,NULL)==0)
	{
		printf("Zamknieto kolejke klienta.\n");
	}
	else
	{
		printf("Nie udalo sie zamknac kolejki klienta.\n");
	}

	if(msgctl(serverQID,IPC_RMID,NULL)==0)
	{
		printf("Zamknieto kolejke servera.\n");
	}
	else
	{
		printf("Nie udalo sie zamknac kolejki servera.\n");
	}
	
	return (0);
}

