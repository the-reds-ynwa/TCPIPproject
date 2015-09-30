#include "inet.h"
#define MAXSIZE 1024

main(int argc, char *argv[]){
int sockfd;
char buffer[MAXSIZE+1];
int bytereceive = 0;
struct sockaddr_in serv_addr;
char crtname[20];
char delname[20];
static struct sigaction act;

void catchin(int);

act.sa_handler = catchin;
sigfillset(&(act.sa_mask));

sigaction(SIGINT, &act, (void *) 0);


if(argc <= 1){
printf("How to use : %s remoteIPaddress [example: ./client 127.0.0.1]\n", argv[0]); exit(1); }

bzero( (char*) &serv_addr, sizeof(serv_addr) );
serv_addr.sin_family = AF_INET ;
serv_addr.sin_port = htons (SERV_TCP_PORT);
inet_pton (AF_INET, argv[1], &serv_addr.sin_addr);

if( (sockfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0) {
perror("Client: socket() error \n");
exit(1); }

if(connect (sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0 ) {
perror("Client: connect() error\n");
exit(1);}

/* Get the user name */ 
	char *buf; 
	buf=(char *)malloc(10*sizeof(char)); 
	buf=getlogin(); 

	/* set the 'client_file' path */ 
	char str[30]; 
	strcpy(str, "/home/"); 
	strcat(str, buf); 
	strcat(str, "/client_file/"); 

	/* Check the path exist or not, if not, create one */ 
	struct stat s; 
	if(stat(str, &s) == -1){ 
	mkdir(str, 0700); }

do{
bzero( buffer, sizeof(buffer));
recv(sockfd, buffer, MAXSIZE, 0);
printf("\n%s\n", buffer); 
gets(buffer);
send(sockfd,buffer, MAXSIZE, 0);


if(!strcmp(buffer, "1"))
{
	bzero( buffer, sizeof(buffer));
	recv(sockfd, buffer, MAXSIZE, 0);
	printf("\n%s\n", buffer); 
	gets(buffer);
	send(sockfd,buffer, MAXSIZE, 0);
	
   	char filname[30];
    	strcpy(filname, "/home/"); 
    	strcat(filname, buf); 
    	strcat(filname, "/client_file/");
    	strcat(filname, buffer);
    
    	FILE *fp;
    	fp = fopen(filname, "ab"); 

		if(NULL == fp)
    	{
        	printf("Error opening file");
        
    	}
    
    	bzero( buffer, sizeof(buffer));
    
    	bytereceive = recv(sockfd, buffer, MAXSIZE, 0);
    	fwrite(buffer,1,bytereceive,fp);
   
}


else if(!strcmp(buffer, "2"))
{

		
	DIR *dir;
	struct dirent *ent;

	char directyName[30];
   	strcpy(directyName, "/home/"); 
    	strcat(directyName, buf); 
    	strcat(directyName, "/client_file/");

	if ((dir = opendir (directyName)) != NULL) {
		
		printf("\n[List of files in Client Directory]\n");
  		// print all the files and directories within directory 
  		while ((ent = readdir (dir)) != NULL) {

		printf("%s\n", ent->d_name);    }

 		closedir (dir);
	}

	printf("\nPlease enter one of the filename from above including extension\n");
	
	bzero( buffer, sizeof(buffer));
	gets(buffer);
	send(sockfd,buffer, MAXSIZE, 0);

	char filname[30];
	strcpy(filname, "/home/"); 
	strcat(filname, buf); 
	strcat(filname, "/client_file/");
	    
	strcat(filname, buffer);

	FILE *fp;
    	fp = fopen(filname, "r"); 

	bzero( buffer, sizeof(buffer));
	int nread = fread(buffer,1,256,fp);
	send(sockfd, buffer, nread, 0);	
}


else if(!strcmp(buffer, "3"))
{
	printf("Enter directory name that you want to create: ");
	scanf("%s", crtname);

	/* set the path/name of the directory that want to create */ 
	char crtDirectory[30]; 
	strcpy(crtDirectory, "/home/"); 
	strcat(crtDirectory, buf); 
	strcat(crtDirectory, "/"); 
	strcat(crtDirectory, crtname);

	/* Check the path exist or not, if not, create one */ 
	struct stat s; 
	if(stat(crtDirectory, &s) == -1){ 
	mkdir(crtDirectory, 0700); } 
}


else if(!strcmp(buffer, "4"))
{
	printf("Enter directory name that you want to delete: ");
	scanf("%s", delname);

	/* set the path of the directory that want to delete */ 
	char delDirectory[30]; 
	strcpy(delDirectory, "/home/"); 
	strcat(delDirectory, buf); 
	strcat(delDirectory, "/"); 
	strcat(delDirectory, delname);

	/* select all the files inside the directory that want to delete */
	char seltSubDirectory[50];
	strcpy(seltSubDirectory, "exec rm -r ");
	strcat(seltSubDirectory, "/home/"); 
	strcat(seltSubDirectory, buf); 
	strcat(seltSubDirectory, "/"); 
	strcat(seltSubDirectory, delname);
	strcat(seltSubDirectory, "/*"); 

	/* Check the path exist or not, if exist, delete it */ 
	struct stat s; 
	if(stat(delDirectory, &s) != -1){
	system(seltSubDirectory);
	rmdir(delDirectory); } 
}

}while (strcmp(buffer, "/q"));
close(sockfd);
}

void catchin(int signo){
	printf("\n[ Interrupt signal has been ignored.]\n");
}
