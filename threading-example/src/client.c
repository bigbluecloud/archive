/*
	Multithreaded program, client main file
*/

#include "client.h"

void runClient(unsigned long* data[], int* flags[], int* progArr[])
{
	struct timeval waittime;
	int i = 1, maxfd, fdIndex;//Tracks which server slot to read from, max file descriptor number, server slot 'index'
	int z, reportTimer = 0, rewriteFlag = 0;
	char buffer[33];//Buffer to store 32 digits of an unsigned long
	fd_set fds;//File descriptor set
	
	waittime.tv_sec = 0;
	waittime.tv_usec = 500000;//500 milliseconds
	
	printf("Enter unsigned long numbers to be factorised\nEnter 'q' to quit\n\n");
	
	while(1)
	{
		fdIndex = *flags[i];
		maxfd = (STDIN_FILENO >= fdIndex)? STDIN_FILENO : fdIndex;//Determine maxfd
		FD_ZERO(&fds);//Initialise the fd set
		
		FD_SET(STDIN_FILENO, &fds);
		FD_SET(fdIndex, &fds);
		
		select(maxfd+1, &fds, NULL, NULL, &waittime);
		
		if(FD_ISSET(STDIN_FILENO, &fds))//If there is keyboard input
		{
			memset(buffer, 0, sizeof(buffer));//NULL the buffer
			fgets(buffer, sizeof(buffer), stdin);//Get the user input
			
			if(buffer[0] == 'q')
			{
				CLIENTFLAG = -1;
				return;
			}
			
			if(buffer[32] != (int)NULL)
			{
				buffer[32] = (int)NULL;
				printf("Number too long, truncated to 32 digits\n");
			}
			
			
			while(CLIENTFLAG == 1)//In case the server hasn't picked up the previous number entered
				usleep(5000);
			
			NUMBER = strtoul(buffer, NULL, 10);//Convert the input to an unsigned long
			if(NUMBER == 0)
				NUMBER = 1;//Stops an infinite loop with the threads
			CLIENTFLAG = 1;//Signal server there is a number to factorise
		}
		
		if(FD_ISSET(fdIndex, &fds))
		{
			while(1)
			{
				if(*flags[i] == 1)//If the threads have not finished factorising
				{
					reportTimer = 0;//Reset the report timer
					if(rewriteFlag == 1)//If a progress report has been made
					{
						rewriteFlag = 0;
						NEWLINE
					}
					printf("%lu ", *data[i]);//Read the data and print it to screen
					fflush(stdout);//Print the result to screen immediately
					*flags[i] = 0;//Allow the threads to progress
					continue;//Wishful thinking that the scheduler interrupts immediately after this, but hey, it could happen
				}
				
				else if(*flags[i] == -1)//Threads are done
				{
					rewriteFlag = 0;
					reportTimer = 0;
					*flags[i] = 0;
					printf("\nAll done!\n");//Alert the user that the query is complete
					printf("Took %lu nanoseconds to complete the query\n\n", *data[i]);//Print the time taken for the factorisation to complete from the number currently in the data slot
					i++;
					i = (i == 11) ? 1 : i;
					break;
				}
				usleep(10000);//Reduce busy waiting somewhat and allow the server threads to get more factors ~10 milliseconds
				reportTimer++;
				if(reportTimer == 50)//The loop has slept 50 times int a row, ~500 millisecond interval of sleeping, probably slightly over
				{
					reportTimer = 0;//Reset the timer
					if(rewriteFlag == 1)//If a progress report has just been made
						printf("\r");//Overwrite the previous report
					else
					{
						rewriteFlag = 1;//Set the flag
						NEWLINE//Put the progress on a new line
					}	
					for(z = 0; z < 10; z++)
						printf("Q%d: %d%% ", z + 1, *progArr[z]);//Write the output
					fflush(stdout);
				}
			}
		}
	}
}
