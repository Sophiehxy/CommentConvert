#include<stdio.h>

#define UL unsigned long

//定义状态
typedef enum
{
	NO_COMMENT_STATE,
	C_COMMENT_STATE,
	CPP_COMMENT_STATE,
	STR_COMMENT_STATE,
	END_STATE
}STATE_ENUM;

//定义状态机
typedef struct STATE_MACHINE
{
	FILE *inputfile;
	FILE *outputfile;
	STATE_ENUM ulstate;
}STATE_MACHINE;

///////////////////////////////////////////////////
STATE_MACHINE  before_state;
STATE_MACHINE  g_state = {0};
void EventPro(char ch);
void EventProAtNo(char ch);
void EventProAtC(char ch);
void EventProAtCpp(char ch);
void EventProATStr(char ch);
//////////////////////////////////////////////////

int Comment(FILE *inputfile, FILE *outputfile)
{
	if(inputfile==NULL || outputfile==NULL)
	{
		printf("Error: Argument Invalid!\n");
		return -1;
	}

	////////////////////////////////////////////////
	//
	g_state.inputfile = inputfile;
	g_state.outputfile = outputfile;
	g_state.ulstate = NO_COMMENT_STATE;
	
	char ch;
	while(g_state.ulstate != END_STATE)
	{
		ch = fgetc(g_state.inputfile);
		EventPro(ch);
	}
	return 0;
}

void EventPro(char ch)
{
	switch(g_state.ulstate)
	{
	case NO_COMMENT_STATE:
		EventProAtNo(ch);
		break;
	case C_COMMENT_STATE:
		EventProAtC(ch);
		break;
	case CPP_COMMENT_STATE:
		EventProAtCpp(ch);
		break;
	case STR_COMMENT_STATE:
		EventProATStr(ch);
	case END_STATE:
		break;
	default:
		break;
	}
}

void EventProAtNo(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
		g_state.ulstate=END_STATE;
	case '/':
		nextch = fgetc(g_state.inputfile);
		if(nextch == '*') // C
		{
			fputc(ch,g_state.outputfile);
			fputc('/',g_state.outputfile);
			g_state.ulstate = C_COMMENT_STATE;
		}
		else if(nextch == '/') //C++
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
			g_state.ulstate = CPP_COMMENT_STATE;
		}
		break;
	case '"'://str
		fputc(ch,g_state.outputfile);
		before_state = g_state;
		g_state.ulstate = STR_COMMENT_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
}
void EventProAtC(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	case '/':
		nextch = fgetc(g_state.inputfile);
		if(nextch=='/') //C中嵌套C++，即/*中嵌套//,//用两个空格代替
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else if(nextch=='*')//C中嵌套C，即/*中嵌套/*,第二个/*用两个空格代替，状态不改变，仍为C状态	
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else if(nextch=='\n')
		{
			fputc('/',g_state.outputfile);
			fputc('/',g_state.outputfile);
		}
	case '\n'://C中遇到\n，每行都改为//
		fputc(ch,g_state.outputfile);
		fputc('/',g_state.outputfile);
		fputc('/',g_state.outputfile);
		fputc(ch,g_state.outputfile);
		g_state.ulstate = NO_COMMENT_STATE;
		break;
	case '*':
		nextch = fgetc(g_state.inputfile);
		if(nextch=='/')//遇到*/，空格输出，状态变为NO状态
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
			g_state.ulstate = NO_COMMENT_STATE;
		}
		else if(nextch=='\n')
		{
			fputc('/',g_state.outputfile);
			fputc('/',g_state.outputfile);
		}
		else
		{
			fputc(ch,g_state.outputfile);
			fputc(nextch,g_state.outputfile);
		}
		break;
	case '"':
		fputc(ch,g_state.outputfile);
		before_state = g_state;
		g_state.ulstate = STR_COMMENT_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
}
void EventProAtCpp(char ch)
{
	char nextch;
	switch(ch)
	{
	case EOF:
		fputc(' ',g_state.outputfile);
		fputc(' ',g_state.outputfile);
		g_state.ulstate = END_STATE;
		break;
	case '/':
		nextch=fgetc(g_state.inputfile);
		if(nextch=='/')//C++嵌套C++，即//嵌套//,第二个//用两个空格代替,状态不改变
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else if(nextch=='*')//C++嵌套C，即//嵌套/*,/*用两个空格代替
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else if(nextch='\n')//C++中遇到\n，退出C++状态，改为无注释状态
		{
			fputc(ch,g_state.outputfile);
			fputc('*',g_state.outputfile);
			fputc('/',g_state.outputfile);
			g_state.ulstate=NO_COMMENT_STATE;
		}
		break;
	case '*':
		nextch=fgetc(g_state.inputfile);
		if(nextch=='/')//C++嵌套C，即//嵌套*/,*/用两个空格代替
		{
			fputc(' ',g_state.outputfile);
			fputc(' ',g_state.outputfile);
		}
		else if(nextch='\n')//C++中遇到\n，退出C++状态，改为无注释状态
		{
			fputc('*',g_state.outputfile);
			fputc('/',g_state.outputfile);
			g_state.ulstate=NO_COMMENT_STATE;
		}
		break;
	case '\n'://C++中遇到\n，退出C++状态，改为无注释状态
			fputc('*',g_state.outputfile);
			fputc('/',g_state.outputfile);
			fputc(ch,g_state.outputfile);
			g_state.ulstate = NO_COMMENT_STATE;
		break;
	case '"'://C++中遇到"，进入C++状态，改为无注释状态
		fputc(ch,g_state.outputfile);
		before_state = g_state;
		g_state.ulstate = STR_COMMENT_STATE;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
	
}

void EventProATStr(char ch)
{

	char nextch;
	switch(ch)
	{
	case EOF:
		g_state.ulstate = END_STATE;
		break;
	case '/':
		fputc(ch,g_state.outputfile);
		break;
	case '*':
		fputc(ch,g_state.outputfile);
		break;
	case '"':
		fputc(ch,g_state.outputfile);
		g_state = before_state;
		break;
	default:
		fputc(ch,g_state.outputfile);
		break;
	}
		


}
