//Marshall Anderson
//CS 470: Operating Systems
//Lab 1: CWUSHELL

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <vector>
#include <regex>
#include <time.h>
#include <sys/utsname.h>

using namespace std;

//methods
void printManual();
int cwushellCommand(vector<string> localCommand);
vector<string> parseCommand(string &input);
int exitOut(vector<string> localCommand);
int promptChange(vector<string> localCommand);
void promptChangeHelp();
int getFileInfo(vector<string> localCommand);
void getFileInfoHelp();
int getOSInfo(vector<string> localCommand);
void getOSInfoHelp();



//variables to use
string commands[5] = { "exit", "prompt", "fileinfo", "osinfo", "manual" };
string input;
string defaultPrompt = "cwushell";
string prompt = defaultPrompt;
char** sysCommand;
int main()
{
	while(true)
	{
		//output "*prompt* >" to the command line at the start
		//of every iteration
		cout << prompt << "> ";
		
		//Get the command from the user, and tokenize it into the arrays
		getline(cin, input);
		
		vector<string> parsedCommand = parseCommand(input);
		
		//if we do not detect a local command to cwushell, run it as a system command.
		if(cwushellCommand(parsedCommand) == -1)
		{
			system(input.c_str());
		}
	}
	return 0;
}

vector<string> parseCommand(string &input)
{
	//delim to parse string with
	regex deliminator(" ");
	//parse the string to remove all spaces
	sregex_token_iterator first{input.begin(), input.end(), deliminator, -1}, last;
	//return the parsed string
	return {first, last};

}

int cwushellCommand(vector<string> localCommand)
{
	//default value of -1, if still -1 it will run system commands
	int select = -1;
	for(int i = 0; i < 5; i++)
	{
		//if the command input matches one of the cwushell commands
		if(localCommand[0].compare(commands[i]) == 0)
		{
			select = i;
			break;
		}
	
	}
	
	switch(select)
	{
		//regular system command
		case -1:
			localCommand.clear();
			return -1;
		//exit command
		case 0:
			exitOut(localCommand);
		//prompt command
		case 1:
			promptChange(localCommand);
			break;
		case 2:
		//fileinfo command
			getFileInfo(localCommand);
			break;
		//osinfo command
		case 3:
			getOSInfo(localCommand);
			break;
		//manual command
		case 4: 
			printManual();
		
	
	}
	
	localCommand.clear();
	return 0;

}

int exitOut(vector<string> localCommand)
{	
	//if only exit was input, simply exit with default value
	if(localCommand.size() == 1)
	{
	
		exit(0);
	
	}
	
	//if there is an exit value, verify it is an integer, if it is not, exit with defautl value 0
	if(localCommand.size() > 1)
	{
		//make sure every value in the exitvalue is an integer, if not exit default
		for(int i = 0; i < localCommand[1].size(); i++)
		{
	
			if(!isdigit(localCommand[1][i])) exit(0);
	
		}
		//exit with value given by user if all values are an integer
		exit(stoi(localCommand[1]));
	}
	return 0;
}

int promptChange(vector<string> localCommand)
{
	//if the user input more than prompt [new_prompt]
	if(localCommand.size() > 2)
	{
	
		cout << "You have entered an unknown parameter... Enter -h or --help after a command for information." << endl;
		return 0;
	
	}
	//if the user only input prompt, set to default
	if(localCommand.size() == 1)
	{
		prompt = defaultPrompt;
	}
	//if they input help commands, cout help function
	else if(localCommand[1] == "-h" || localCommand[1] == "--help")
	{
	
		promptChangeHelp();
		return 0;
	}
	//if none of these cases, set new prompt
	else
	{
	
		prompt = localCommand[1];
	}
	
	localCommand.clear();
	return 0;

}

void promptChangeHelp()
{
	cout << "Usage: prompt [new_prompt]" << endl;
	cout << "Changes the shell prompt. Leave new_prompt empty to set to the default prompt." << endl;
}

int getFileInfo(vector<string> localCommand)
{
	//if the help switch was utilized, or if just fileinfo was input
	if (localCommand.size() == 1 || localCommand[1] == "-h" || localCommand[1] == "--help") 
	{
        	getFileInfoHelp();
        	return 0;
	}
	
	//use stat to pull relevant file info
	struct stat result;
	
	//calls stat function on the file name and stores result in 'result'
	//checks whatever the last input is in localCommand, since switches come before the file
	int status = stat(localCommand[localCommand.size()-1].c_str(), &result);
	
	//file not found
	if(status < 0)
	{
		cout << "Invalid file name or missing file name, please try again" << endl;
		return 0;
	}
	
	//check type of file
	string filetype;
	
	if(S_ISREG(result.st_mode) != 0) 
	{
		filetype = "Regular file";
	}
	if(S_ISDIR(result.st_mode) != 0) 
	{
		filetype = "Directory";
	}
	if(S_ISLNK(result.st_mode) != 0)
	{
		filetype = "Symbolic link";
	}
	if(S_ISBLK(result.st_mode) != 0) 
	{
		filetype = "Block Device";
	}
	if(S_ISSOCK(result.st_mode) != 0) 
	{
		filetype = "Socket";
	}
	if(S_ISCHR(result.st_mode) != 0) 
	{
		filetype = "Character device";
	}
	if(S_ISFIFO(result.st_mode) != 0) 
	{
		filetype = "FIFO";
	}
	
	//if just fileinfo [filename] was called, utilize all 3 switches
	if(localCommand.size() == 2)
	{
		cout << "Inode: " << result.st_ino << endl;
		cout << "File Type: " << filetype << endl;
		cout << "Last modified time: " << ctime(&result.st_mtime) << endl;
		return 0;
	
	}
	//if its the non-default case, and there is a switch
	if(localCommand.size() > 2 && localCommand[1][0] == '-')
	{
	
		//if only - was entered
		if(localCommand[2].size() == 1)
		{
		
			cout << "Invalid input, please try again." << endl;
		
		}
		
		//if there is an invalid switch in the first space after -
		if(localCommand[1].size() > 1 && localCommand[1][1] != 'i' && localCommand[1][1] != 't' && localCommand[1][1] != 'm')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there is an invalid switch in the second space after -
		if(localCommand[1].size() > 2 && localCommand[1][2] != 'i' && localCommand[1][2] != 't' && localCommand[1][2] != 'm')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there is an invalid switch in the third space after -
		if(localCommand[1].size() > 3 && localCommand[1][3] != 'i' && localCommand[1][3] != 't' && localCommand[1][3] != 'm')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there are more than the three switches
		if(localCommand[1].size() > 4)
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//check if the first switch is i t or m, and print the corresponding output
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 'i')
		{
		
			cout << "Inode: " << result.st_ino << endl;
		
		}
		
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 't')
		{
		
			cout << "File Type: " << filetype << endl;
		
		}
		
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 'm')
		{
		
			cout << "Last modified time: " << ctime(&result.st_mtime) << endl;	
		
		}
		
		//these statements check if multiple switches are input in one command, i.e: fileinfo -itm [file]
		
		if(localCommand[1].size() > 2 && localCommand[1][2] == 'i')
		{
		
			cout << "Inode: " << result.st_ino << endl;
		
		}
		
		else if(localCommand[1].size() > 2 && localCommand[1][2] == 't')
		{
		
			cout << "File Type: " << filetype << endl;
	
		}
		
		else if(localCommand[1].size() > 2 && localCommand[1][2] == 'm')
		{
		
			cout << "Last modified time: " << ctime(&result.st_mtime) << endl;	
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 'i')
		{
		
			cout << "Inode: " << result.st_ino << endl;
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 't')
		{
		
			cout << "File Type: " << filetype << endl;
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 'm')
		{
		
			cout << "Last modified time: " << ctime(&result.st_mtime) << endl;	
		
		}
	}
		
	return 0;

}

void getFileInfoHelp()
{
	cout << "Usage: fileinfo [option]" << endl;
	cout << "Prints information related to filename given in option." << endl;
	cout << "-i \t prints the inode number of the file." << endl;
	cout << "-t \t print the type of file." << endl;
	cout << "-m \t print the date of last modification." << endl;
}

int getOSInfo(vector<string> localCommand)
{
	if (localCommand.size() == 1 || localCommand[1] == "-h" || localCommand[1] == "--help") 
	{
        	getOSInfoHelp();
        	return 0;
	}
	
	
	struct utsname osDetails;
	
	int ret = uname(&osDetails);
	
	//non-default case, and switch was input
	if(localCommand.size() > 1 && localCommand[1][0] == '-')
	{
	
		//if only - was entered
		if(localCommand[1].size() == 1)
		{
		
			cout << "Invalid input, please try again." << endl;
			return 0;
		
		}
		
		//if there is an invalid switch in the first space after -
		if(localCommand[1].size() > 1 && localCommand[1][1] != 'S' && localCommand[1][1] != 'v' && localCommand[1][1] != 'a')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there is an invalid switch in the second space after -
		if(localCommand[1].size() > 2 && localCommand[1][2] != 'S' && localCommand[1][2] != 'v' && localCommand[1][2] != 'a')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there is an invalid switch in the third space after -
		if(localCommand[1].size() > 3 && localCommand[1][3] != 'S' && localCommand[1][3] != 'v' && localCommand[1][3] != 'a')
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		//if there are more than the three switches
		if(localCommand[1].size() > 4)
		{
		
			cout << "Invalid switch, please try again." << endl;
			return 0;
		
		}
		
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 'S')
		{
		
			cout << "Operating System: " << osDetails.sysname << endl;
		
		}
		
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 'v')
		{
		
			cout << "Version: " << osDetails.version << endl;
		
		}
		
		else if(localCommand[1].size() > 1 && localCommand[1][1] == 'a')
		{
		
			cout << "Architecture: " << osDetails.machine << endl;	
		
		}
		
		if(localCommand[1].size() > 2 && localCommand[1][2] == 'S')
		{
		
			cout << "Operating System: " << osDetails.sysname << endl;
		
		}
		
		else if(localCommand[1].size() > 2 && localCommand[1][2] == 'v')
		{
		
			cout << "Version: " << osDetails.version << endl;
	
		}
		
		else if(localCommand[1].size() > 2 && localCommand[1][2] == 'a')
		{
		
			cout << "Architecture: " << osDetails.machine << endl;
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 'S')
		{
		
			cout << "Operating System: " << osDetails.sysname << endl;
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 'v')
		{
		
			cout << "Version: " << osDetails.version << endl;
		
		}
		
		if(localCommand[1].size() > 3 && localCommand[1][3] == 'a')
		{
		
			cout << "Architecture: " << osDetails.machine << endl;
		
		}
	}
	
	return 0;
}

void getOSInfoHelp()
{
	cout << "Usage: osinfo [-switch]" << endl;
	cout << "Prints information related to the operating system." << endl;
	cout << "-S \t prints the operating system running on the machine." << endl;
	cout << "-v \t prints the version of the operating system." << endl;
	cout << "-a \t prints the computer architecture." << endl;
}
void printManual()
{
	
	fstream help_file;
	help_file.open("help.hlp", ios::in);
	string line;
	while(getline(help_file, line))
	{
	
		cout << line << endl;
	
	}
	
}
