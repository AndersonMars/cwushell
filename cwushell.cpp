//Marshall Anderson
//CS 470: Operating Systems
//Lab 1: CWUSHELL

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <vector>
#include <regex>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <ctime>
#include <sys/utsname.h>

using namespace std;

//methods
vector<string> parseCommand(string &input);
int commandCheck(string command);
int exitCommand(vector<string> command);
int promptCommand(vector<string> command);
int fileInfoCommand(vector<string> command);
int osInfoCommand(vector<string> command);
int systemCommand(vector<string> command);
int manualCommand();


//variables to use
string commands[5] = { "exit", "prompt", "fileinfo", "osinfo", "manual" };
string input;
string defaultPrompt = "cwushell";
string prompt = defaultPrompt;
int lastCommand = 0;

int main()
{

	int exitVal = -1;
	bool exit = false;
	while(!exit)
	{
		//output whatever the prompt currently is, will be cwushell on launch
		cout << prompt << "> ";
		
		//Get the command from the user, and tokenize it into the arrays
		getline(cin, input);
		
		vector<string> parsedCommand = parseCommand(input);
		
		//pass the first part of the command to check if it is a system call or local
		int commandVal = commandCheck(parsedCommand[0]);
		
		//switch to decide what to do with each command
		switch(commandVal)
		{
			//system command: fork to execute
			case -1:
			lastCommand = systemCommand(parsedCommand);
			break;
			//exit command: check for an exit val, if not get the last exit value, if none exist, exit 0
			case 0:
			exitVal = exitCommand(parsedCommand);
			if(exitVal != -1) exit = true;
			break;
			//prompt command: check if restore to default or new prompt
			case 1:
			lastCommand = promptCommand(parsedCommand);
			break;
			//fileinfo command: check file and get info based on switches
			case 2:
			lastCommand = fileInfoCommand(parsedCommand);
			break;
			//osinfo command: get info about os depending on switches
			case 3:
			lastCommand = osInfoCommand(parsedCommand);
			break;
			//manual command, print a help list to the screen
			case 4:
			manualCommand();
			break;
		}
	
	}
	cout << "Exited with value " << exitVal << endl;
	return exitVal;
}

int manualCommand()
{

	cout << "Welcome to the cwushell manual. Here you can see the various commands available to\nthis shell.\nThese commands include: exit, prompt, fileinfo, osinfo,\nand other system commands normally available on the operating system.\nHere is more info for each command:\n\nUsage: exit [option]\nExits the shell with an exit value.\nThis exit value can be manually given with an option.\nIf no option is given, it will default to the return value of the last command\ngiven, or 0.\n\nUsage: prompt [option]\nChanges the prompt for the shell. By default, this prompt is cwushell.\nUsing an option will change the prompt to that new option, while no option will\nreset the prompt.\n\nUsage: fileinfo [option]\nPrints information related to filename given in option.\n-i \t prints the inode number of the file.\n-t \t print the type of file.\n-m \t print the date of last modification.\n\nUsage: osinfo [option]\nPrints information related to operating system given in option.\n-S \t prints the operating system running on this system.\n-v \t prints the version of the OS.\n-a \t prints the computer architecture.\n";
	
	return 0;

}

int systemCommand(vector<string> command)
{
	//convert the vector of strings to a singular string
	string commandString;
	for(int i = 0; i < command.size(); i++)
	{
		
		commandString += command[i] + " ";
	
	}
	return system(commandString.c_str());

}

int osInfoCommand(vector<string> command)
{
	if(command.size() == 1 || command[1] == "-h" || command[1] == "--help")
	{
		cout << "Usage: osinfo [option]" << endl;
		cout << "Prints information related to operating system given in option." << endl;
		cout << "-S \t prints the operating system running on this system." << endl;
		cout << "-v \t prints the version of the OS." << endl;
		cout << "-a \t prints the computer architecture." << endl;
		return 0;
	}
	
	struct utsname osDetails;
	
	int ret = uname(&osDetails);
	
	bool getName = false;
	bool getVer = false;
	bool getArc = false;
	
	//check for invalid characters
	for(int i = 1; i < command.size(); i++)
	{
		if(command[i][0] != '-') 
		{
			cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
		for(int j = 1; j < command[i].size(); j++)
		{
		
			if(command[i][j] != 'S' && command[i][j] != 'v' && command[i][j] != 'a')
			{
			
				cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
				return 1;
			
			}
		
		}
	
	}
	
	//otherwise, check which switches are being printed	
	if(command[1].find("S") != string::npos) getName = true; 
	if(command[1].find("v") != string::npos) getVer = true; 
	if(command[1].find("a") != string::npos) getArc = true;
		
		
	if(command.size() >= 4)
	{
		//if there are multiple calls to the same switch
		if(command[2].find("S") != string::npos && getName == true)
		{
		 	cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
		 	return 1;
		}
			
		if(command[2].find("S") != string::npos) getName = true;
			
		if(command[2].find("v") != string::npos && getVer == true)
		{
		 	cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
		 	return 1;
		}
			
		if(command[2].find("v") != string::npos) getVer = true;
			
		if(command[2].find("a") != string::npos && getArc == true)
		{
			cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
			
		if(command[2].find("a") != string::npos) getArc = true;
		
	}
	if(command.size() == 5)
	{
		if(command[3].find("S") != string::npos && getName == true)
		{
			cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
			
		if(command[3].find("S") != string::npos) getName = true;
			
		if(command[3].find("v") != string::npos && getVer == true)
		{
			cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
			
		if(command[3].find("v") != string::npos) getVer = true;
			
		if(command[3].find("a") != string::npos && getArc == true)
		{
			cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
			 
		if(command[3].find("a") != string::npos) getArc = true;
		
	}
	
	if(getName == true) cout << "Operating System: " << osDetails.sysname << endl; 
	if(getVer == true) cout << "OS Version: " << osDetails.release << endl;
	if(getArc == true) cout << "Architecture: " << osDetails.machine << endl;
	

	return 0;
}

int fileInfoCommand(vector<string> command)
{

	if(command.size() == 1 || command[1] == "-h" || command[1] == "--help")
	{
		cout << "Usage: fileinfo [option]" << endl;
		cout << "Prints information related to filename given in option." << endl;
		cout << "-i \t prints the inode number of the file." << endl;
		cout << "-t \t print the type of file." << endl;
		cout << "-m \t print the date of last modification." << endl;
	}
	
	//check for invalid characters
	for(int i = 1; i < command.size() - 1; i++)
	{
		if(command[i][0] != '-') 
		{
			cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
			return 1;
		}
		for(int j = 1; j < command[i].size(); j++)
		{
		
			if(command[i][j] != 'i' && command[i][j] != 't' && command[i][j] != 'm')
			{
			
				cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
				return 1;
			
			}
		
		}
	
	}
	
	struct stat fileStat;
	if(stat(command[command.size() - 1].c_str(), &fileStat) < 0)
	{
	
		cerr << "Error: Unable to get file status." << endl;
		return 1;
	
	}
	
	
	//get file type
	string fileType;
	if(S_ISREG(fileStat.st_mode)) fileType = "Regular file";
	else if(S_ISDIR(fileStat.st_mode)) fileType = "Directory";
	else if(S_ISLNK(fileStat.st_mode)) fileType = "Symbolic link";
	else fileType = "Other";
	
	//get Inode
	ino_t inode = fileStat.st_ino;
	
	//get last modified time
	time_t modifiedTime = fileStat.st_mtime;
	
	bool getType = false;
	bool getInode = false;
	bool getTime = false;
	
	//if command is 2, print every switch
	if(command.size() == 2)
	{
		getType = true;
		getInode = true;
		getTime = true;
	}
	
	//otherwise, check which switches are being printed
	else
	{
		
		if(command[1].find("i") != string::npos) getInode = true;
		if(command[1].find("t") != string::npos) getType = true;
		if(command[1].find("m") != string::npos) getTime = true;
		
		
		if(command.size() >= 4)
		{
			//if there are multiple calls to the same switch
			if(command[2].find("i") != string::npos && getInode == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			
			if(command[2].find("i") != string::npos) getInode = true;
			
			if(command[2].find("t") != string::npos && getType == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			
			if(command[2].find("t") != string::npos) getType = true;
			
			if(command[2].find("m") != string::npos && getTime == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			
			if(command[2].find("m") != string::npos) getTime = true;
		
		}
		if(command.size() == 5)
		{
			if(command[3].find("i") != string::npos && getInode == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			
			if(command[3].find("i") != string::npos) getInode = true;
			
			if(command[3].find("t") != string::npos && getType == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			
			if(command[3].find("t") != string::npos) getType = true;
			
			if(command[3].find("m") != string::npos && getTime == true)
			{
			 cout << "You have entered too many parameters. Enter -h or --help after a command for more information." << endl;
			 return 1;
			}
			 
			if(command[3].find("m") != string::npos) getTime = true;
		
		}
		//check if all switches are grouped
		if(command[1] == "-itm") getInode, getType, getTime = true;
	}
	
	if(getInode == true) cout << "Inode: " << inode << endl; 
	if(getType == true) cout << "File Type: " << fileType << endl;
	if(getTime == true) cout << "Last modified time: " << modifiedTime << endl;
	
	
	return 0;
}

int promptCommand(vector<string> command)
{
	//if we reach this point and size == 1, then it is just a default reset
	if(command.size() == 1) prompt = defaultPrompt;
	else if(command.size() > 2) cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
	else prompt = command[1];
	lastCommand = 0;
	return 0;

}

int exitCommand(vector<string> command)
{
	//lastCommand == 0 when no commands have been called, otherwise it is updated
	if(command.size() == 1)
	{
		return lastCommand;
	}
	
	//check to see if the only characters in the command are the exit command and number
	if(command.size() > 2) cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
	//check if the second part of the command is only an integer
	int exitVal;
	try 
	{
		exitVal = stoi(command[1]);
		return exitVal;
	}
	catch(...)
	{
	
		cout << "You have entered an unknown parameter. Enter -h or --help after a command for more information." << endl;
		return -1;
	
	}

}

int commandCheck(string command)
{
	int returnVal = -1;

	//default value of -1, if still -1 it will run system commands
	int select = -1;
	for(int i = 0; i < 5; i++)
	{
		//if the command input matches one of the cwushell commands
		if(command.compare(commands[i]) == 0)
		{
			select = i;
			break;
		}
	
	}
	
	switch(select)
	{
	//If any of thes numbers return we know its a local command, if -1 returns it's system
		//exit command
		case 0:
			returnVal = 0;
			break;
		//prompt command
		case 1:
			returnVal = 1;
			break;
		//fileinfo command
		case 2:
			returnVal = 2;
			break;
		//osinfo command
		case 3:
			returnVal = 3;
			break;
		//manual command
		case 4: 
			returnVal = 4;	
	}
	return returnVal;


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
