/*
 * Program: prestonshell.cpp
 * A "simple" shell program
 * Author: Preston Danielson
*/

#include <cstdio>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <new>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

using namespace std;


void execcmd(vector<string>, int);
void tokenize(const string& str, vector<string>&, const string& delimiters = " ");
char* strcpy(char*, const char*);
char** strvec_to_c_strarr(vector<string>);
char* str_to_c_str(string);

#define MAX_INPUT 128
#define WAIT      00
#define REDIROUT  11
#define REDIRIN   22
#define PIPE      33
#define APPENDOUT 44

int main(int argc, char *argv[])
{
	int mode = WAIT;
	int numargs;
	string *cmd;
	vector<string> args;
	vector<string> vars; 
	vector<string> vals; 
	size_t length = MAX_INPUT;
	char curdir[100];

	cmd = new string[MAX_INPUT];

	while(1)
	{
		args.clear();
		mode = WAIT;
		getcwd(curdir, 100);
		printf("PrestonShell: %s$ ", curdir);
		getline(cin, *cmd);
		tokenize(*cmd, args);
		numargs = args.size();
		if(args[0] == "exit")
		{
			printf("Goodbye!\n");
			exit(0);
			//make error detection for extra args
		}
		if(args[0] == "cd")
		{
			const char *path = str_to_c_str(args[1]);
			chdir(path);
			continue;
			//add error message for non-existent directory
		}
		if(args[0] == "pwd")
		{
			getcwd(curdir, 100);
			printf("%s\n", curdir);
			continue;
		}
		if(args[0] == "set")
		{
			vars.push_back(args[1]);
			vals.push_back(args[2]);
			continue;
		}
		if(args[0] == "unset")
		{ //segfault???
			int i = 0;
			if(vars.size() < 1)
			{
				printf("Variable %s does not exist!\n", 								str_to_c_str(args[1]));
				continue;
			}
			else while(args[1] != vars[i])
			{
				if(i < vars.size())
					i++;
				else
				{
					printf("Variable %s does not exist!\n", 									str_to_c_str(args[1]));
					continue;
				}
			}
			vars.erase(vars.begin() + i);
			vals.erase(vals.begin() + i);
			continue;
		}
		if(args[0] == "list")
		{
			for(int i = 0; i < vars.size(); i++)
			{
				cout<<vars[i]<<" = "<<vals[i]<<endl;
			}
			continue;
		}
		else
		{
			execcmd(args, mode);
		}
	}

	return 0;
}

void tokenize(const string& str, vector<string>& tokens, 					const string& delimiters)
{
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	string::size_type pos = str.find_first_of(delimiters, lastPos);
	
	while(string::npos != pos || string::npos != lastPos)
	{
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
}

char* strcpy(char* dest, const char* from) 
{
    char *tmp = dest;
    for ( ; *from; tmp++, from++)
	{
        *tmp = *from;
    }
    *tmp = '\0';

    return dest;
}

char** strvec_to_c_strarr(vector<string> args)
{
	char **c_args = (char**)malloc(sizeof(char*)*MAX_INPUT);
	for(int i = 0; i < args.size(); i++)
	{
		strcpy(c_args[i], args[i].c_str());
	}
	return c_args;
}

char* str_to_c_str(string arg)
{
	char *c_arg = new char[arg.size() + 1];
	strcpy(c_arg, arg.c_str());
	return c_arg;
}

void execcmd(vector<string> args, int mode)
{
	pid_t pid;
	FILE *fp;
	int mode2 = WAIT;
	int status1, status2;
	char *file = str_to_c_str(args[0]);
	char **c_args = strvec_to_c_strarr(args);
	pid = fork();
	if(pid < 0)
	{
		printf("PrestonShell: Forking Error! ");
		exit(-1);
	}
	else if(pid == 0)
	{
		if(execvp(file, c_args) == -1)
			perror("PrestonShell: Execution Error! ");
		exit(-1);
	}
	else
	{
		if(mode == PIPE)
		{	
			//piping
		}
		else
		{
			do
			{
				waitpid(pid, &status1, WUNTRACED); 
			}while(!WIFEXITED(status1) && !WIFSIGNALED(status1));
		}
	}
	//execvp(file, c_args);
}
