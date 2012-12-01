#include "debug.h"

namespace Debug
{
	void PassFail(string Message, bool Result)
	{
		PassFail(Message, Result, "success", "fail");
	}

	void PassFail(string Message, bool Result, string MessagePass, string MessageFail)
	{
		if(Result) Pass(Message + " " + MessagePass);
		else       Fail(Message + " " + MessageFail);
	}

	void Pass(string Message)
	{
		cout << Message << endl;
	}

	void Fail(string Message)
	{
		cout << Message << endl;
	}

	void Error(string Message)
	{
		cout << "Error: " << Message << endl;
	}
};