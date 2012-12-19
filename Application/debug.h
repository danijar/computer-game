#pragma once

#include <iostream>
#include <string>
using namespace std;

namespace Debug
{
	void Pass(string Message);
	void Fail(string Message);
	void PassFail(string Message, bool Result, string MessagePass = "success", string MessageFail = "fail");
	void Crash(string Message);
};