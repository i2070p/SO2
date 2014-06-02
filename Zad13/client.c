// SO2 IS1 211A LAB12
// Patryk Siemiński
// psieminski@wi.zut.edu.pl

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <signal.h>

#define SERVER 269713
#define USERS 25

key_t id;
int semaphoreId, segmentId;

struct contacts {
	char nicknames[USERS][10];
	int queueId[USERS];
};

struct msgbuf {
	long mtype;
	char msg[255];
	char nickname[10];
};

struct contacts* buff;
struct sembuf* sops;
char nickname[10];
int pid,qid,result,option,reciever;
char textMsg[255];

void showContacts(struct contacts* serverContacts)
{
	int i;
	printf("-----------Contacts list-------------\n\n");
	for (i=0; i<USERS; i++)
	{
		if (strcmp(serverContacts->nicknames[i],"")!=0)
		{
			printf("%d.%s %d \n",i,serverContacts->nicknames[i],serverContacts->queueId[i]);
		}
	}
}

int registerOnServer(struct contacts* serverContacts, char* nickname, int pid)
{
	int i;
	for (i=0; i<USERS; i++)
	{
		if (strcmp(serverContacts->nicknames[i],nickname)==0)
		{
			printf("This nickname is already taken.\n"); return(0);
		}
	}
	for (i=0; i<USERS; i++)
	{
		if (strcmp(serverContacts->nicknames[i],"")==0)
		{
			sprintf(serverContacts->nicknames[i],nickname);
			serverContacts->queueId[i]=pid;
			return i+1;
		}
	}
	return(-1);
}

int main(int argc, char** argv) {
	result = 0; option = 0;

	sops = (struct sembuf *) malloc(sizeof(struct sembuf));
	sops[0].sem_flg = 0;

	segmentId = shmget(SERVER,sizeof(struct contacts) , 0777);
	if (segmentId < 0)
	{
		printf("Server is offline1.\n");
		exit(1);
	}

	semaphoreId = semget(SERVER, 2, 0666);
	if (semaphoreId < 0)
	{
		printf("Server is offline2.\n");
		exit(2);
	}

	if ((buff = shmat(segmentId, NULL, 0)) == (struct contacts *) -1) {
		printf("Memory segment attachment error.\n");
		exit(3);
	}

	pid = getpid();
	qid = msgget(pid, IPC_CREAT|0777);

	while(result==0)
	{
		printf("Enter your nickname : ");
		scanf("%10s",nickname);

		sops[0].sem_num = 0;
		sops[0].sem_op = 0;
		sops[0].sem_num = 0;
		sops[0].sem_op = 1;
		semop(semaphoreId,sops,1);

		result = registerOnServer(buff,nickname,qid);

		sops[0].sem_num = 0;
		sops[0].sem_op = -1;
		semop(semaphoreId,sops,1);
	}

	if (result==-1) {printf("Client has been turned off. Server is full.\n"); exit(1);}

	printf("\n------------Welcome in DarkOS communicator!------------\n\n");
	printf("Options : \n");
	printf("1. Show users list. \n");
	printf("2. Send msg. \n");
	printf("3. Shut down. \n");

	if (pid=fork())
	{
		struct msgbuf * msg=malloc(sizeof(struct msgbuf));
		while(option!=3)
		{
			printf ("Select option : "); scanf("%d",&option);
			switch (option) 
			{
			case 1:
				showContacts(buff);
				break;
			case 2: 
				printf("Enter reciever id from list : "); scanf("%d",&reciever);
				if (reciever>=0 && reciever <USERS)
				{
					printf("Enter msg : "); scanf(" %[^\n]s",&textMsg);
			
					sprintf(msg->msg,textMsg);
					sprintf(msg->nickname,nickname);
					msg->mtype=1;

					sops[0].sem_num = 0;
					sops[0].sem_op = 0;
					sops[0].sem_num = 0;
					sops[0].sem_op = 1;
					if (msgsnd(buff->queueId[reciever], msg, sizeof(struct msgbuf), 0) == -1)
					{
						printf("Failed to send msg.\n\n");
					}
					else
					{
						printf("[Sent to %s] : %s\n",buff->nicknames[reciever],textMsg);  
					}
					sops[0].sem_num = 0;
					sops[0].sem_op = -1;
				}
				else
				{
					printf("\nInvalid reciever id.\n");
				}
				break;
			case 3:
				msgctl(qid,IPC_RMID,NULL);
				kill(pid, SIGKILL);
				sops[0].sem_num = 0;
				sops[0].sem_op = 0;
				sops[0].sem_num = 0;
				sops[0].sem_op = 1;
				sprintf(buff->nicknames[result-1],"");
				buff->queueId[result-1]=0;
				sops[0].sem_num = 0;
				sops[0].sem_op = -1;
				break;
			default:
				printf("Incorrect option number.\n");
			}
		}
	}
	else
	{
		struct msgbuf * msg=malloc(sizeof(struct msgbuf));
		int test;
		while(1)
		{
			test = msgrcv(qid,msg,sizeof(struct msgbuf),-5,0);
			printf("\n [From %s] : %s \n",msg->nickname, msg->msg);
		}
	}
	return (0);
}
