/* Fix registry tool adapted from PSP to PSVita by Major_Tom
 * Original version by skylark@mips.for.ever
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <sys/stat.h>

#include "sha1.h"

typedef unsigned char uchar;

void getcheck(uchar *block, int len, uchar *check)
{
	uchar save[4];
	uchar res[20];
	struct sha_ctx ctx;

	memcpy(save, block+14, 4);
	memset(block+14, 0, 4);

	sha_init(&ctx);
	sha_update(&ctx, block, len);
	sha_final(&ctx);
	sha_digest(&ctx, res);

	memcpy(block+14, save, 4);

	check[0] = res[4] ^ res[3] ^ res[2] ^ res[1] ^ res[0];
	check[1] = res[9] ^ res[8] ^ res[7] ^ res[6] ^ res[5];
	check[2] = res[14] ^ res[13] ^ res[12] ^ res[11] ^ res[10];
	check[3] = res[19] ^ res[18] ^ res[17] ^ res[16] ^ res[15];
}

int checkcheck(uchar *block, int len)
{
	uchar res[4];
	getcheck(block, len, res);
	if(!memcmp(res, block+14, 4))
		return 1;
	return 0;
}

void fixcheck(uchar *block, int len)
{
	uchar res[4];
	getcheck(block, len, res);
	memcpy(block+14, res, 4);
}

unsigned char d[524288];
struct {
	short _0;
	short _1;
	short parent;
	short _2;
	short _3;
	short nent;
	short nblk;
	char name[28];
	short _4;
	short fat[7];
	short _5;
} a[1024];

void walk_fatents(void)
{
	int i,j;
	for(i=0;i<1024;i++)
		if(a[i].nblk) {
			/* reassemble segments
			   it can be done better in-place
			   bleh i don't care foo */
			uchar *buf=malloc(a[i].nblk*512);
			for(j=0;j<a[i].nblk;j++)
				memcpy(buf+512*j,d+512*a[i].fat[j],512);

            char *path=malloc(sizeof(char)*100);
            sprintf(path, "seg/buf_%s.dreg", a[i].name);

			FILE *f=fopen(path,"wb+");
            fwrite(buf,512*a[i].nblk,1,f);
            fclose(f);

			if(checkcheck(buf,a[i].nblk*512))
				fprintf(stderr,"PASS 0x%02X '%s'\n",i,a[i].name);
			else {
				fprintf(stderr,"FAIL 0x%02X '%s' -> FIXED\n",i,a[i].name);
				fixcheck(buf,a[i].nblk*512);
				memcpy(d+512*a[i].fat[0],buf,512);
			}
			free(buf);
		}
}

int main(void)
{
	mkdir("seg", S_IRWXU);
	
	FILE *f=fopen("system.dreg","r");
	fread(d,1024,512,f);
	fclose(f);
	f=fopen("system.ireg","r");
	fseek(f,0xBC,SEEK_SET);
	fread(a,1024,60,f);
	fclose(f);
	walk_fatents();
	f=fopen("system_.dreg","wb+");
	fwrite(d,1024,512,f);
	fclose(f);
	return 0;
}
