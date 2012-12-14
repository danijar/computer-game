#include "debug.h"

namespace Debug
{

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

	void Error(string Message)
	{
		cout << "Error: " << Message << endl;
	}

};