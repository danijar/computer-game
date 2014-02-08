#pragma once

#include <string>
#include <iostream>


class ManagerLog
{
public:
	ManagerLog(std::string Name) : name(Name) {}

	void Print(std::string Message)
	{
		Print(name, Message);
	}
	void Pass(std::string Message)
	{
		Pass(name, Message);
	}
	void Fail(std::string Message)
	{
		Fail(name, Message);
	}
	void PassFail(std::string Message, bool Result, std::string Pass = "success", std::string Fail = "fail")
	{
		PassFail(name, Message, Result, Pass, Fail);
	}
	void Warning(std::string Message)
	{
		Warning(name, Message);
	}
	void Wait(std::string Message = "...")
	{
		Wait(name, Message);
	}
	void Crash(std::string Message)
	{
		Crash(name, Message);
	}

	static void Print(std::string Name, std::string Message)
	{
		std::cout << Name << (Name == "" ? "" : " " ) << Message << std::endl;
	}
	static void Inline(std::string Message)
	{
		std::cout << " " << Message;
	}
	static void Pass(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void Fail(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void PassFail(std::string Name, std::string Message, bool Result, std::string Pass = "success", std::string Fail = "fail")
	{
		if(Result) ManagerLog::Pass(Name, Message + " " + Pass);
		else       ManagerLog::Fail(Name, Message + " " + Fail);
	}
	static void Warning(std::string Name, std::string Message)
	{
		Print(Name, Message);
	}
	static void Wait(std::string Name, std::string Message = "...")
	{
		Print(Name, Message);
		std::cin.clear();
		std::cin.get();
	}
	static void Crash(std::string Name, std::string Message)
	{
		Wait(Name, "crashed, " + Message);
		std::exit(EXIT_FAILURE);
	}
	static std::string Format(float Value, unsigned int Head, unsigned int Tail)
	{
		std::string text = std::to_string((int)(Value * std::pow(10, Tail)));
		while (text.length() < Head + Tail)
			text = " " + text;
		text.insert(text.length() - Tail, 1, '.');
		return text;
	}
private:
	std::string name;
};
