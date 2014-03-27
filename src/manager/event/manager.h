#pragma once

#include <string>
#include <unordered_map>
#include <functional>

class ManagerEvent
{
	typedef std::unordered_map<std::string, std::unordered_map<int, std::vector<std::pair<void*, bool>>>> Events;
public:
	void Listen(std::string Name, std::function<void()> Function)
	{
		Listen(Name, 0, Function);
	}
	void Listen(std::string Name, int State, std::function<void()> Function)
	{
		list[Name][State].push_back(std::make_pair(new std::function<void()>(Function), false));
	}
	template <typename T>
	void Listen(std::string Name, std::function<void(T)> Function)
	{
		Listen<T>(Name, 0, Function);
	}
	template <typename T>
	void Listen(std::string Name, int State, std::function<void(T)> Function)
	{
		list[Name][State].push_back(std::make_pair(new std::function<void(T)>(Function), true));
	}
	void Fire(std::string Name)
	{
		Fire(Name, 0);
	}
	void Fire(std::string Name, int State)
	{
		auto Functions = list[Name][State];

		for (auto i = Functions.begin(); i != Functions.end(); ++i) {
			if (i->second) continue;
			else (*(std::function<void()>*)(i->first))();
		}
	}
	void FireRange(std::string Name, int From, int To)
	{
		for (int i = From; i <= To; ++i) Fire(Name, i);
	}
	template <typename T>
	void Fire(std::string Name, T Data)
	{
		Fire(Name, 0, Data);
	}
	template <typename T>
	void Fire(std::string Name, int State, T Data)
	{
		auto Functions = list[Name][State];

		for (auto i = Functions.begin(); i != Functions.end(); ++i) {
			if (i->second) (*(std::function<void(T)>*)i->first)(Data);
			else           (*(std::function<void()>*)i->first)();
		}
	}
private:
	Events list;
};
