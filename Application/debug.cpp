#include "debug.h"

namespace Debug
{
	void Info(string Message)
	{
		cout << Message << endl;
	}

	void Pass(string Message)
	{
		cout << Message << endl;
	}

	void Fail(string Message)
	{
		cout << Message << endl;
	}

	void PassFail(string Message, bool Result, string MessagePass, string MessageFail)
	{
		if(Result) Pass(Message + " " + MessagePass);
		else       Fail(Message + " " + MessageFail);
	}

	void Inline(string Message)
	{
		cout << Message;
	}

	void Wait(string Message)
	{
		cout << Message << endl;
		cin.clear();
		cin.get();
	}

	void Crash(string Message)
	{
		cout << "Error: " << Message << endl;
		cin.clear();
		cin.get();
		exit(EXIT_FAILURE);
	}

};