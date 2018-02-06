/* Program to create log file of stored mails */
#include<stdio.h>
#include<time.h>
#include<string.h>
#include<fcntl.h>
#include<sys/types.h>
#include<dirent.h>
#include"bloom.h"

void timestamp(char *a)
{
    int i;
    time_t ltime;
    struct tm *Tm;
    ltime=time(NULL);
    Tm=localtime(&ltime);
    sprintf(a,"%d-%d-%d|%d:%d:%d",Tm->tm_mday,Tm->tm_mon,Tm->tm_year+1900,Tm->tm_hour,Tm->tm_min,Tm->tm_sec);    
}
void logCreate(BLOOM *bitArray)
{
	FILE *fp;
	char tmp[50];
	char fileName[50];
	int i;
	timestamp(tmp);
	strcpy(fileName,"log/");
	strcat(fileName,tmp);
	fp=fopen(fileName,"w");
        printf("LENGTH=%d\n",bitArray->asize);
	for(i=0;i<bitArray->asize/8;++i)
	{
		fprintf(fp,"%c",bitArray->a[i]);
	}
	fclose(fp);
}

void dirwalk(char *dir)
{
    struct dirent *dp;
    DIR *dfd;
    if((dfd=opendir(dir))==NULL)
    {
         fprintf(stderr,"Cannot open directory",dir);
         return;
    }
    while((dp=readdir(dfd)))
    {
          if(strcmp(dp->d_name,".")==0||strcmp(dp->d_name,"..")==0)
              continue;
          printf("%s\n",dp->d_name);
    }
    closedir(dfd);
}
