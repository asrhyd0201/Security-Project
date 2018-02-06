/*SendMail Third Party Program to archieve sender's address using Milter API*/


#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "bloom.h"
#include "log.h"

#include "libmilter/mfapi.h"

#define BLOCK_SIZE 5

#define NO_OF_BLOCKS 20
/*  bool */
#ifndef bool

#define bool   int

#define TRUE   1

#define FALSE  0

#endif 

struct priv_struct
{
	char *mailfrom;
        char *mailto;
        char *body;
};

BLOOM *bloom;

int nblocks;   //To count number of blocks inserted in bloom filter 

void insert(char *address)
{
	FILE *fp;
	fp=fopen("SAddr.txt","a");
	fprintf(fp,"%s\n",address);
	fclose(fp);
}
int countBlocks(int length)
{
	int count=0,i,size=BLOCK_SIZE;
	for(i=0; ;i++,size=pow(2,i)*BLOCK_SIZE)
	{
		count+=ceil(length/size);
		if(size > length)
			break;	
	}	
        return count;	
}
void HBF_insert(char *s)
{
	int i,j,size;
	FILE *fp;
	char block[strlen(s)+1],offset[strlen(s)+30],tmp[20];
        printf("nblcks = %d + countblocks = %d > NO_OF_BLOCKS = %d",nblocks,countBlocks(strlen(s)),NO_OF_BLOCKS);
	size=BLOCK_SIZE;
	for(i=0;size<strlen(s);i++,size=pow(2,i)*BLOCK_SIZE)
	{
		for(j=0;j<strlen(s);j+=size)
		{
			
			strncpy(block,s+j,size);
			block[size]='\0';
			//printf("%s\n",block);
			sprintf(offset,"%s%d",block,j/size);
			//printf("%s\n",offset);
			bloom_add(bloom,offset);
			nblocks++;
			//printf("No of Blocks: %d\n",nblocks);
		}
	}
        /*for(i=0;i<65536/8;i++)
          printf("%c",bloom->a[i]);
        sprintf(offset,"%s%d","tmp",nblocks);*/
        fp=fopen("tmp","w");
	for(i=0;i<bloom->asize/8;i++)
	{ 
              fprintf(fp,"%c",bloom->a[i]);
	}
	fclose(fp);


        if(nblocks+countBlocks(strlen(s))>NO_OF_BLOCKS)//Saturation Point
        {
                //printf("LOG IN");
		logCreate(bloom);
		bloom_destroy(bloom);
		if(!(bloom=bloom_create(65536, 4, sax_hash, sdbm_hash,DJBHash,BKDRHash))) 
		{
			fprintf(stderr, "ERROR: Could not create bloom filter\n");
			return EXIT_FAILURE;
		}
                nblocks=0;
                //printf("Nblocks=%d\n",nblocks);
                //printf("LOG OUT");
	}
}
	
sfsistat xxfi_envfrom(SMFICTX *ctx, char **argv)
{
	struct priv_struct *priv;
	priv = malloc(sizeof *priv);
	smfi_setpriv(ctx,priv);
	char *mailaddr = smfi_getsymval(ctx, "{mail_addr}");
     	priv = smfi_getpriv(ctx);
     	if (priv == NULL)
       		return SMFIS_TEMPFAIL;
    	if (argv[0] != NULL)
    	{
      		priv->mailfrom = strdup(argv[0]);
      		if (priv->mailfrom == NULL)
        		return SMFIS_TEMPFAIL;
		else
			insert(priv->mailfrom);
			bloom_add(bloom, priv->mailfrom);
                        nblocks++;
			//bloom_display(bloom);
    	}
        /* continue processing */
    	return SMFIS_CONTINUE;
}

sfsistat xxfi_envrcpt(SMFICTX *ctx, char **argv)
{
        char *mailto = smfi_getsymval(ctx, "{mail_addr}");
        struct priv_struct *priv;
     	priv = smfi_getpriv(ctx);
     	if (priv == NULL)
       		return SMFIS_TEMPFAIL;
    	if (argv[0] != NULL)
    	{
      		priv->mailto = strdup(argv[0]);
      		if (priv->mailto == NULL)
        		return SMFIS_TEMPFAIL;
		else
			insert(priv->mailto);
			bloom_add(bloom, priv->mailto);
                        nblocks++;
			//bloom_display(bloom);
    	}
        /* continue processing */
    	return SMFIS_CONTINUE;
}

sfsistat xxfi_body(SMFICTX *ctx,unsigned char *bodyp, size_t bodylen)
{

        struct priv_struct *priv;
     	priv = smfi_getpriv(ctx);
     	if (priv == NULL)
       		return SMFIS_TEMPFAIL;
        
	if(bodyp !=NULL)
	{
                bodyp[bodylen]='\0';
		insert(bodyp);
		//bloom_add(bloom, bodyp);
		HBF_insert(bodyp);
		//bloom_display(bloom);
	}
	
        /* continue processing */
        return SMFIS_CONTINUE;
}

struct smfiDesc smfilter =
{

        "SFilter", 	//<name>		/* filter name */

        SMFI_VERSION,   //version		/* version code -- do not change */

	SMFIF_NONE, 	//flags			/*flag specifying to dont modified the mail*/
                        
        NULL, 		//xxfi_connect,   	/* connection info filter */

        NULL, 		//xxfi_helo, 		/* SMTP HELO command filter */

        xxfi_envfrom,   //xxfi_envfrom,		/* envelope sender filter */

        xxfi_envrcpt, 	//xxfi_envrcpt,   	/* envelope recipient filter */

        NULL,		//xxfi_header,     	/* header filter */

        NULL, 		//xxfi_eoh,   		/* end of header */

        xxfi_body, 	//xxfi_body,		/* body block filter */

        NULL, 		//xxfi_eom,   		/* end of message */

        NULL, 		//xxfi_abort,       	/* message aborted */

        NULL, 		//xxfi_close,       	/* connection cleanup */

        NULL, 		//xxfi_unknown,   	/* unknown SMTP commands */

        NULL, 		//xxfi_data, 		/* DATA command */

        NULL, 		//xxfi_negotiate        /* Once, at the start of each SMTP connection */

};

int main()
{

	if(!(bloom=bloom_create(65536, 4, sax_hash, sdbm_hash,DJBHash,BKDRHash))) 
	{
		fprintf(stderr, "ERROR: Could not create bloom filter\n");
		return EXIT_FAILURE;
	}

	if (smfi_register(smfilter) == MI_FAILURE)
    	{
		fprintf(stderr, "smfi_register failed\n");
		exit(EX_UNAVAILABLE);
	}

	if (smfi_setconn("inet:9999@localhost") == MI_FAILURE)
	{
		fprintf(stderr, "smfi_setconn() %s\n",strerror(errno));
		exit(EX_SOFTWARE);
	}

	if (smfi_opensocket(0) == MI_FAILURE)
	{
		fprintf(stderr, "smfi_opensocket() %s\n",strerror(errno));
		exit(EX_SOFTWARE);
	}

	(void) smfi_settimeout(600);

	return smfi_main();
}
