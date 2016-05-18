#include<stdio.h>
//每个区由若干个内存块组成，//每个块是4096个字节

//int i = 0;*/   
/****************************************
 * Function name:  main
 * Argument     :  void
 * Destribute   :  
 * Return value :
 * when   why   what    who
 * 
 ****************************************/

extern int CommentConvert(FILE *, FILE *);
//extern int Comment(FILE *, FILE *);

int main()
{
	FILE *fpIn;
	FILE *fpOut;
	fpIn = fopen("input.c","r");
	if(NULL == fpIn)
	{
		printf("Error: Open input.c file fail!\n");
		return -1;
	}
	fpOut = fopen("output.c","w");
	if(NULL == fpOut)
	{
		printf("Error: Open output.c file fail!\n");
		return -1;
	}

	CommentConvert(fpIn,fpOut);//
//	Comment(fpIn,fpOut);

	fclose(fpIn);
	fclose(fpOut);
	return 0;
}