#pragma once

#include <iostream>
#include <string>


namespace Debug
{
	using namespace std;

	void Info(string Message);
	void Pass(string Message);
	void Fail(string Message);
	void PassFail(string Message, bool Result, string MessagePass = "success", string MessageFail = "fail");
	void Wait(string Message = "...");
	void Crash(string Message);
};