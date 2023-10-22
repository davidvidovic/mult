#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

const int MAX_STRING = 100;

int main(void) {
	char gret [MAX_STRING];
	int csize;
	int prank;
	int random;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &csize);
	MPI_Comm_rank(MPI_COMM_WORLD, &prank);
	
	// Set random seed for each core
	srand(time(NULL) + prank);	
	
	char mess_buffer[csize][csize-1][2];
	int index_buff[csize];
	for(int i = 0; i < csize; i++) 
	{
		index_buff[i] = 0;
		for(int j = 0; j < csize - 1; j++) 
		{
			mess_buffer[i][j][0] = 0;
			mess_buffer[i][j][1] = 0;
		}
	}
	int source;

	// Each core sends a message to each other core
	// In a format: XY
	// Where X is a process that sends the message (prank)
	// Y is a random number that represents message
	
	for(int i = 0; i < csize; i++)
	{
		if(i == prank) continue; // Do not send message to itself
					 
		random = rand() % 10;
		sprintf(gret, "%d%d", prank, random);
		MPI_Send(gret, strlen(gret)+1, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		//printf("[PROC %d] Process %d sending message %d to %d process\n", prank, prank, random, i);
	}

	for(int i = 0; i < csize; i++)
	{
		if(i == prank) continue;
		
		if(prank == 0)
		{
			MPI_Recv(gret, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			source = index_buff[i];
			mess_buffer[0][source][0] = gret[0];
			mess_buffer[0][source][1] = gret[1];
			//printf("[PROC %d] Process %d recieved message from %d process, message: %s\n", prank, prank, i, gret);
		}
		else
		{
			MPI_Recv(gret, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			gret[2] = prank + 48;
			gret[3] = '\0';
			MPI_Send(gret, strlen(gret)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			//printf("[PROC %d] Process %d recieved message from %d process, message: %s\n", prank, prank, i, gret);
		}
	}

	if(prank == 0)
	{
		// Core 0 should recieve (cize-1)*(csize-1) messages
		// From other cores
		// Example: 3 total cores: 0 already recieved 2 messages from c1 and c2
		// c1 sends 2 messages (0Y and 2Y)
		// c2 sends 2 messages (0Y and 1Y)

		for(int i = 0; i < (csize-1) * csize; i++)
		{
			if(i % csize == 0) continue;
			
			MPI_Recv(gret, MAX_STRING, MPI_CHAR, i % csize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			source = gret[2] - 48;
			gret[2] = '\0';
			mess_buffer[source][index_buff[source]][0] = gret[0];
			mess_buffer[source][index_buff[source]++][1] = gret[1];
			//printf("[OFFICIAL PROC %d] Transaction happened: process %d recieved message: %s\n", prank, source, gret);
		}


		for(int i = 0; i < csize; i++)
		{
			printf("Process %d recieved: ", i);
			for(int j = 0; j < csize - 1; j++)
			{
				printf("%c%c", mess_buffer[i][j][0], mess_buffer[i][j][1]);
			}
			printf("\n");
		}
	}	

	MPI_Finalize();
return 0;
}
