/*
*	Assignment 1, server directory listing functions
*	Jared Rankin, s2896344
*/

#include "../common.h"

int compare(const void* arg1, const void* arg2)//Used for qsort
{
	return stricmp(*(char**)arg1, *(char**) arg2);
}

//Helper function to determine what flags were passed by the user
void parseCommands(int* f, int* l, char* fileDetail[2], char* input[8])
{
	int i;//Ignore index 1 of input[], it will always contain 'list'
	char* suffix = ".txt";
	int lengthStr, lengthSuff = strlen(suffix);
	
	for(i = 1; i <= sizeof(input); i++)//Iterate through the user input array
	{
		if(input[i] == NULL)
			break;
			
		lengthStr = strlen(input[i]);
	
		if(strcmp(input[i], "-f") == 0 && *f != 1)
			*f = 1;//-f flag was typed
		
		else if(strcmp(input[i], "-l") == 0 && *l != 1)
			*l = 1;//-l flag was typed
			
		else if(lengthStr > lengthSuff)//If the string contained in the index is longer than the '.txt' suffix (Could be either a filepath or file name)
		{
			if(strcmp(input[i] + lengthStr - lengthSuff, suffix) == 0)//Flag points to a file
				fileDetail[1] = input[i];
				
			else//Otherwise the flag is a file path
				fileDetail[0] = input[i];
		}
		
		else//Case to cover a file path being three or less character EG "C:\"
			fileDetail[0] = input[i];
	}
}


//Helper function to abstract creating a string of access permissions -UNIX-
#ifndef WIN32
char* buildAccPer(long per)
{
	static char accPer[16] = {'\0'};
	
	char* dir = (S_ISDIR(per)) ? "d" : "-";
	char* ur = (per & S_IRUSR) ? "r" : "-";
	char* uw = (per & S_IWUSR) ? "w" : "-";
	char* ux = (per & S_IXUSR) ? "x" : "-";
	char* gr = (per & S_IRGRP) ? "r" : "-";
	char* gw = (per & S_IWGRP) ? "w" : "-";
	char* gx = (per & S_IXGRP) ? "x" : "-";
	char* or = (per & S_IROTH) ? "r" : "-";
	char* ow = (per & S_IWOTH) ? "w" : "-";
	char* ox = (per & S_IXOTH) ? "x" : "-";
	
	sprintf(accPer, "%c%c%c%c%c%c%c%c%c%c", *dir, *ur, *uw, *ux, *gr, *gw, *gx, *or, *ow, *ox);
	
	return accPer;
}
#endif

//Abstraction of logic for getting the file attributes
#ifndef WIN32
char* getLongList(char* fileName)
{
	struct stat fileStat;
	struct tm locTime;

	static char fileData[128] = {'\0'};
	char createDate[32] = {'\0'};
	stat(fileName, &fileStat);//Get the file attributes
	
	char* accPer = buildAccPer(fileStat.st_mode);//Build the access permissions string
	
	localtime_r(&fileStat.st_birthtim.tv_sec, &locTime);//Get the creation date from the file attributes -st_birthtim.tv_sec MAY BE UNSAFE-
	strftime(createDate, sizeof(createDate), "%D %H:%M", &locTime);//Create the time string
	
	sprintf(fileData, "%-26s %s %s %d %d", fileName, accPer, createDate, fileStat.st_uid, fileStat.st_size);//Create the string containing all of the file data
	return fileData;
}
#endif

//Function that is called from the 'list' command
void listFiles(int client, char* input[8])
{
	char str[256] = {'\0'};//String for sending back an error or the writing mode w/ filepath
	char cwdName[256] = {'\0'};//Array for current working directory path
		
	#ifdef WIN32
	
	#else //UNIX CODE
	int i = 0, count = 0;//Iteration counters: i used in for loops and fileNames array, count records how many files there are in the directory
	int fFlag = 0, lFlag = 0;//Test if -l and/or -f are in the command array
	int* f = &fFlag;
	int* l = &lFlag;
	
	char* fileDetail[2] = {'\0'};//Array to hold the file path/file name flags -[0] always holds file path to explore, [1] always holds file name to write to-
	char* fileNames[256] = {'\0'};//2D Array for names of files in a directory (and attributes if '-l'), assuming no more than 256 files in one directory
		
	char fileList[4096] = {'\0'};//If the list is being printed to a file it will be put into a formatted block first
	
	DIR* dir_ptr;
	struct dirent* direntp;
	
	
	send(client, "list", 8, 0);//Tell the client it will be receiving data from a 'list' request
	parseCommands(f, l, fileDetail, input);//Break up the input string
	
	
	if(fFlag == 1 && fileDetail[1] == NULL)//-f was specified but no file name was given
	{
		sprintf(str, "Invalid use of list! If [-f] flag is specified, a [filename] must be given.\n");
		send(client, str, sizeof(str), 0);
	}
	
	else if(fileDetail[0] == NULL && (dir_ptr = opendir(getcwd(cwdName, sizeof(cwdName)))) == NULL)//Check if current directory can be opened
	{
	
		sprintf(str, "Cannot open current directory: %s\n", strerror(errno));
		send(client, str, sizeof(str), 0);
		return;
	}
		
	else if(fileDetail[0] != NULL && (dir_ptr = opendir(fileDetail[0])) == NULL)//Check if specified directory can be opened
	{
		sprintf(str, "Cannot open directory: %s\n%s\n", fileDetail[0], strerror(errno));
		send(client, str, sizeof(str), 0);
		return;
	}
	
	send(client, str, sizeof(str), 0);//Send a blank string because there were no errors
	
	//Read directory files
	if(lFlag == 1)//Long list
	{
		while((direntp = readdir(dir_ptr)) != NULL)
		{
			fileNames[i] = strdup(getLongList(direntp->d_name));//Assign a string to [i]
			i++;
			count++;//Records how many files there are in the directory
		}
	}
	
	else//Short list
	{
		while((direntp = readdir(dir_ptr)) != NULL)
		{
			fileNames[i] = strdup(direntp->d_name);//Assign the index in fileNames, i, a duplicated string of direntp->d_name, then increment i
			i++;
			count++;
		}
	}
	
	qsort((void*)fileNames, count, sizeof(char*), compare);//Use qsort on the fileNames array
	
	if(fileDetail[1] != NULL)//A filepath to write to was specified
	{
		str[0] = '@';//Shows that a file is being written [default]
		if(fFlag == 1)//If the -f flag is specified, change the first index of fileList
			str[0] = '!';//To avoid issues with f:\ drives or relative directories starting with f
			
		strcat(str, fileDetail[1]);//Append the filepath on to the 'mode string'
		
		for(i = 0; i < count; i++)//Concatenate the data into a formatted block
		{
			strcat(fileList, fileNames[i]);
			strcat(fileList, "\n");
		}
		
		send(client, str, sizeof(str), 0);//Send the path to write to if the file path and -f flag were specified
		send(client, fileList, sizeof(fileList), 0);//Send the formatted block over the socket
	}
	
	else
	{
		send(client, str, sizeof(str), 0);//Send a blank string because the listing is not going to file
		
		sprintf(str, "%d", count);//Put the number of files into the str array so it can be sent to the client
		send(client, str, sizeof(str), 0);//Send the number of files in the directory in ASCII form
		
		for(i = 0; i < count; i++)
		{
			send(client, fileNames[i], 256, 0);//Send the client a single directory entry at a time (256 characters because that is the size of the buffer in the client)
		}
	}
	
	closedir(dir_ptr);
	#endif
}
