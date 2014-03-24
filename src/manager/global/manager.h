#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "manager/log/manager.h"


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
			ManagerLog::Warning("system", "cannot add global (" + Name + ") because it already exists.");
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
			ManagerLog::Crash("system", "cannot get global (" + Name + ") because it does not exists.");
			return nullptr;
		}
		return static_cast<T*>(i->second.get());
	}
	void Delete(std::string Name)
	{
		auto i = list.find(Name);
		if(i == list.end())
		{
			ManagerLog::Warning("system", "cannot delete global (" + Name + ") because it does not exists");
			return;
		}
		delete &i->second; // test this
		list.erase(i);
	}
	bool Check(std::string Name)
	{
		return list.find(Name) != list.end();
	}
private:
	std::unordered_map<std::string, std::shared_ptr<void>> list;
};
