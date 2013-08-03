#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "helper/debug.h"


class ManagerGlobal
{
public:
	template <typename T>
	T *Add(std::string Name)
	{
		return Add(Name, new T());
	}
	template <typename T>
	T *Add(std::string Name, T *Instance)
	{
		auto result = list.insert(make_pair(Name, std::shared_ptr<void>(Instance)));
		if (!result.second)
		{
			HelperDebug::Warning("system", "cannot add global (" + Name + ") because it already exists.");
			return Get<T>(Name);
		}
		return Instance;
	}
	template <typename T>
	T *Get(std::string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			HelperDebug::Crash("system", "cannot get global (" + Name + ") because it does not exists.");
			return nullptr;
		}
		return static_cast<T*>(i->second.get());
	}
	void Delete(std::string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			HelperDebug::Warning("system", "cannot delete global (" + Name + ") because it does not exists");
			return;
		}
		delete &i->second; // test this
		list.erase(i);
	}
private:
	std::unordered_map<std::string, std::shared_ptr<void>> list;
};
