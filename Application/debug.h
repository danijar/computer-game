#pragma once

#include <iostream>
#include <string>


namespace Debug
{
	using namespace std;

	void Pass(string Message);
	void Fail(string Message);
	void PassFail(string Message, bool Result, string MessagePass = "success", string MessageFail = "fail");
	void Crash(string Message);
};