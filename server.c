#include "inet.h"
#include <stdbool.h>
#define MAXSIZE 1024

main(){
	int sockfd, new_sockfd, clilen;
	char buffer[MAXSIZE+1];
	struct sockaddr_in serv_addr, cli_addr;
	int bytreceive = 0;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
	  perror("Server: socket() error\n");
	  exit(1);
	}

	printf("\n** File Repository Server **\n");

	/* Get Server's IP address */
	FILE *fp;
	char returnData[64];
	char *token;
	char a[1000];	
	fp = popen("/sbin/ifconfig eth0", "r");
	while (fgets(returnData, 64, fp) != NULL)
	{
		/* store the string in returnData into variable a */
		strcat(a, returnData);
	}

	/* get the first token and walk through other tokens 
	   until the IP address token */
	token = strtok(a, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	token = strtok(NULL, " addr:");

	/* print the IP address token */
	printf("Server IP address: %s\n", token);

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(token);
	serv_addr.sin_port = htons(SERV_TCP_PORT);

	pclose(fp);

	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	perror("Server: bind() error\n");
	printf("\nWaiting for connection... [bind]\n");

	/* Get the user name */ 
	char *buf; 
	buf=(char *)malloc(10*sizeof(char)); 
	buf=getlogin(); 

	/* set the 'server_file' path */ 
	char str[30]; 
	strcpy(str, "/home/"); 
	strcat(str, buf); 
	strcat(str, "/server_file/"); 

	/* Check the path exist or not, if not, create one */ 
	struct stat s; 
	if(stat(str, &s) == -1){ 
	mkdir(str, 0700); }

	listen(sockfd, 5);
	int p[2];
	if(pipe(p) == -1)
	{
		perror("Fail to create pipe");
		exit(1);
	} 

	for(;;){
	 clilen = sizeof(cli_addr);
	 new_sockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

	
	char *clientaddress = inet_ntoa(cli_addr.sin_addr);
	char *individual_address; 
	write(p[1], &clientaddress, sizeof(clientaddress));
	
	

	if(fork() == 0){
	  close(sockfd);
	  strcpy(buffer,"** Hello, welcome to the server. ** \n\nPress\n1.Download File\n2.Send File\n3.Create Directory\n4.Delete Directory(include sub directory)\n[type /q to quit] : ");
	  send(new_sockfd, buffer, MAXSIZE, 0);


	if(new_sockfd > 0)
		
	  read(p[0],&individual_address,sizeof(individual_address));
	  printf("\nClient %s connected now.\n", individual_address);
	  

	do{
	  
	  recv(new_sockfd, buffer, MAXSIZE, 0);
	 
	  if(!strcmp(buffer, "1"))
	{	
		bzero( buffer, sizeof(buffer));
		strcat(buffer,"[List of files in Server Directory]\nPlease select a file from the list to be download:\n\n");
		
		DIR *dir;
		struct dirent *ent;

		char directyName[30];
   		strcpy(directyName, "/home/"); 
    		strcat(directyName, buf); 
    		strcat(directyName, "/server_file/");

		if ((dir = opendir (directyName)) != NULL) {
		
			//print all the files and directories within directory 
 			while ((ent = readdir (dir)) != NULL) {

			strcat(buffer, ent->d_name);
			strcat(buffer, "\n");
 			}
 		 closedir (dir);
		send(new_sockfd, buffer, MAXSIZE, 0);
		 
		}else {

  		// could not open directory 
 		 perror ("Directory not exist.");
  		return EXIT_FAILURE;
		}

	bool exist = true; 

	do{

        
	bzero( buffer, sizeof(buffer));
	recv(new_sockfd, buffer, MAXSIZE, 0);

	char filename[30];
    	strcpy(filename, "/home/"); 
    	strcat(filename, buf); 
    	strcat(filename, "/server_file/");
    	strcat(filename, buffer);
    
	
	FILE *fp = fopen(filename, "r");
	if(fp==NULL)
        {
            strcpy(buffer," There is no such file in server.Please key in the correct filename with extension.");
	    send(new_sockfd, buffer, MAXSIZE, 0);  
	    exist = false; 
	   
        }  
	
	if(exist == true )
	{
	bzero( buffer, sizeof(buffer));
	int nread = fread(buffer,1,256,fp);
	send(new_sockfd, buffer, nread, 0);
	}

	bzero( buffer, sizeof(buffer));
	strcpy(buffer,"Sucessfully Downloaded. [/q to quit]");

	}while(exist == false);
	}
	
	
	 if(!strcmp(buffer, "2"))
	{
		
		bzero( buffer, sizeof(buffer));
		recv(new_sockfd, buffer, MAXSIZE, 0);

		char filename[30];
		strcpy(filename, "/home/"); 
		strcat(filename, buf); 
		strcat(filename, "/server_file/");
		strcat(filename, buffer);
		

		FILE *fp;
   	 	fp = fopen(filename, "ab"); 
		bzero( buffer, sizeof(buffer));
		bytreceive = recv(new_sockfd, buffer, MAXSIZE, 0);
		fwrite(buffer,1,bytreceive,fp);
		fclose(fp);

		bzero( buffer, sizeof(buffer));
		strcat(buffer,"Sucessfully Sent. [/q to quit]");
	}
	
	 
	if(!strcmp(buffer, "3"))
	{
		bzero( buffer, sizeof(buffer));
		strcat(buffer,"Sucessfully Created Directory. [/q to quit]");
	}

	if(!strcmp(buffer, "4"))
	{
		bzero( buffer, sizeof(buffer));
		strcat(buffer,"Sucessfully Deleted Directory. [/q to quit]");
	}
	 send(new_sockfd, buffer, MAXSIZE, 0);
	}while(strcmp(buffer, "/q"));

	printf("\nClient %s disconnected now.\n", individual_address);
	exit(0);
	}
	close(new_sockfd);
	}
	close(sockfd);
}
