/*Query Processing*/

#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<dirent.h>
#include<math.h>
#include"bloom.h"



/*Marcos defintions*/
#define loffset 50
#define DIR_NAME "log"
#define BLOCK_SIZE 5

/*Function prototypes*/
int findPayload(char *payload);
int findMail(char *mailid);
void bloomRetrieve(BLOOM *bloom,char *fileName);


int findPayload(char *payload)
{
	//FILE *fp;
	struct dirent *dp;
	BLOOM *bloom;
	DIR *dfd;
	/*bloom creation*/
	if(!(bloom=bloom_create(65536, 4, sax_hash, sdbm_hash,DJBHash,BKDRHash))) 
	{
		fprintf(stderr, "ERROR: Could not create bloom filter\n");
	}

	
	if((dfd=opendir(DIR_NAME))==NULL)
	{
	 	fprintf(stderr,"Cannot open directory",DIR_NAME);
	 	return;
	}
	while((dp=readdir(dfd)))
	{
	  	if(strcmp(dp->d_name,".")==0||strcmp(dp->d_name,"..")==0)
	      		continue;
	  	//fp=fopen(dp->d_name,"r");
		printf("%s\n",dp->d_name);
		bloomRetrieve(bloom,dp->d_name);
		if(checkPayload(bloom,payload))
                {
			closedir(dfd);
			printf("1\n");
			return 1;
		}
	}
	closedir(dfd);
	printf("0\n");
	return 0;
}
int findMail(char *mailid)
{
	struct dirent *dp;
	BLOOM *bloom;
	DIR *dfd;
	/*bloom creation*/
	if(!(bloom=bloom_create(65536, 4, sax_hash, sdbm_hash,DJBHash,BKDRHash))) 
	{
		fprintf(stderr, "ERROR: Could not create bloom filter\n");
	}
	
	if((dfd=opendir(DIR_NAME))==NULL)
	{
	 	fprintf(stderr,"Cannot open directory",DIR_NAME);
	 	return;
	}
	while((dp=readdir(dfd)))
	{
	  	if(strcmp(dp->d_name,".")==0||strcmp(dp->d_name,"..")==0)
	      		continue;
	  	//fp=fopen(dp->d_name,"r");
		printf("%s\n",dp->d_name);
		bloomRetrieve(bloom,dp->d_name);
		if(bloom_check(bloom,mailid))
		{
			closedir(dfd);
                	return 1;
		}
	}
	closedir(dfd);
	return 0;
		
}
void bloomRetrieve(BLOOM *bloom,char *fileName)
{
	FILE *fp;
	int i;
	char tmp[50]="log/";
	strcat(tmp,fileName);
	fp=fopen(tmp,"r");
	for(i=0;i<bloom->asize/8;i++)
	{
		bloom->a[i]=fgetc(fp);
	//	printf("%c",bloom->a[i]);
	}
	fclose(fp);
	//bloom_display(bloom);
}
int checkPayload(BLOOM *bloom,char *payload)
{
	char block[strlen(payload)+1];
	char offset[strlen(payload)+20],tmp[strlen(payload)+20];
	int i=0,j,k,l,m;
	int flag,blcksize,level=0;
        blcksize=BLOCK_SIZE;
	for(m=0;m<=no_Of_Levels(strlen(payload));m++,blcksize*=2)
	{
		flag=0;
		for(;i<BLOCK_SIZE;i++)
		{
		        strncpy(block,payload+i,blcksize);
			block[(int)pow(2,m)*blcksize]='\0';
			for(j=0;j<loffset;j++)
			{
		                sprintf(offset,"%s%d",block,j);
				if(bloom_check(bloom,offset))
				{	
					l=i+blcksize;	
		                        for(k=j+1;l<=(strlen(payload));k++)
					{	
		        			strncpy(block,payload+l,pow(2,m)*blcksize);
						block[(int)pow(2,m)*blcksize]='\0';
		        			sprintf(tmp,"%s%d",block,k);
						l=l+pow(2,m)*blcksize;
		                                if(!bloom_check(bloom,tmp))
						{
		                                	break;
						}      
	 				}

				}
                                if(l>=strlen(payload))
				{
                                        level++;
                                        printf("Level = %d\n",level);
					flag=1;
					break;
				}		
			}
			if(flag==1)
				break;
		}
		if(flag!=1)
			return 0;
		
	}
	return 1;
}
int no_Of_Levels(int length)
{
	int i,k;
        k=BLOCK_SIZE;
	for(i=0;length/k;i++)
	{
		k=k*2;
	}
        return i-1;  
}
