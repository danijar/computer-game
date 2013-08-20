#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>

#include "helper/debug.h"


class ManagerEntity
{
public:
	ManagerEntity() : index(0) {}
	int New()
	{
		return ++index;
	}
	template <typename T>
	T *Add(unsigned int Id)
	{
		return Add(Id, new T());
	}
	template <typename T>
	T *Add(unsigned int Id, T *Instance)
	{
		if(Id > index || 1 > Id) HelperDebug::Crash("system", "cannot add entity (" + std::to_string(Id) + ") because it is not an entity id, use 'int New()'");
		auto key = std::type_index(typeid(T));

		if (list.find(key) == list.end())
		{
			auto value = new std::unordered_map<int, std::shared_ptr<void> >();
			list.insert(std::make_pair(key, *value));
		}

		auto result = list[key].insert(std::make_pair(Id, std::shared_ptr<void>(Instance)));
		if (!result.second)
		{
			HelperDebug::Warning("system", "cannot add entity (" + std::to_string(Id) + ") to (" + std::string(key.name()) + ") because it already exists");
			return Get<T>(Id);
		}
		return Instance;
	}
	template <typename T>
	std::unordered_map<int, T*> Get()
	{
		std::unordered_map<int, T*> output;
		auto key = std::type_index(typeid(T));
		if (Check(key))
		{
			for(auto i = list[key].begin(); i != list[key].end(); ++i)
			{
				output.insert(std::make_pair(i->first, static_cast<T*>(i->second.get())));
			}
		}
		return output;
	}
	template <typename T>
	T *Get(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			HelperDebug::Crash("system", "cannot get entity because (" + std::to_string(Id) + ") in (" + std::string(key.name()) + ") does not exist");
			return nullptr;
		}
		else return static_cast<T*>(list[key][Id].get());
	}
	void Delete(unsigned int Id)
	{
		if(Id > index || 1 > Id)
		{
			HelperDebug::Warning("system", "cannot delete entity (" + std::to_string(Id) + ") because it is not an entity id");
			return;
		}
		for(auto i = list.begin(); i != list.end(); ++i)
		{
			if (Check(i->first, Id))
			{
				auto j = i->second.find(Id);
				j->second.reset();
				i->second.erase(j);
			}
		}
	}
	template <typename T>
	void Delete(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			HelperDebug::Crash("system", "cannot delete entity because (" + std::string(key.name()) + ") in (" + std::string(key.name()) + ") does not exist");
			return;
		}
		auto j = list[key].find(Id);
		j->second.reset();
		list[key].erase(j);
	}
	template <typename T>
	bool Check(unsigned int Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key)) return false;
		if (list[key].find(Id) == list[key].end()) return false;
		return true;
	}
private:
	unsigned int index;
	std::unordered_map<std::type_index, std::unordered_map<int, std::shared_ptr<void>>> list;
	bool Check(std::type_index key)
	{
		if (list.find(key) == list.end()) return false;
		return true;
	}
	bool Check(std::type_index key, int id)
	{
		if (!Check(key)) return false;
		if (list[key].find(id) == list[key].end()) return false;
		return true;
	}
};
