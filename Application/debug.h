#pragma once

#include <iostream>
#include <string>
using namespace std;

namespace Debug
{
	void PassFail(string Message, bool Result);
	void PassFail(string Message, bool Result, string MessagePass, string MessageFail);
	void Pass(string Message);
	void Fail(string Message);
	void Error(string Message);
};