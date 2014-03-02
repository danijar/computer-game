#pragma once

#include <unordered_map>
#include <memory>
#include <typeindex>
#include <stdint.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "manager/log/manager.h"


class ManagerEntity
{
public:
	uint64_t New()
	{
		uint64_t id = 0;
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		for(int i = 0; i < 8; ++i)
		{
			id <<= 8;
			id |= uuid.data[i];
			// id |= rand() % 128;
		}
		return id;
	}
	template <typename T>
	T *Add(uint64_t Id)
	{
		return Add(Id, new T());
	}
	template <typename T>
	T *Add(uint64_t Id, T *Instance)
	{
		auto key = std::type_index(typeid(T));
		if (list.find(key) == list.end())
		{
			auto value = new std::unordered_map<uint64_t, std::shared_ptr<void> >();
			list.insert(std::make_pair(key, *value));
		}

		auto result = list[key].insert(std::make_pair(Id, std::shared_ptr<void>(Instance)));
		if (!result.second)
		{
			ManagerLog::Warning("system", "cannot add entity (" + std::to_string(Id) + ") to (" + std::string(key.name()) + ") because it already exists");
			return Get<T>(Id);
		}
		return Instance;
	}
	template <typename T>
	std::unordered_map<uint64_t, T*> Get()
	{
		std::unordered_map<uint64_t, T*> output;
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
	T *Get(uint64_t Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			ManagerLog::Crash("system", "cannot get entity because (" + std::to_string(Id) + ") in (" + std::string(key.name()) + ") does not exist");
			return nullptr;
		}
		else return static_cast<T*>(list[key][Id].get());
	}
	void Delete(uint64_t Id)
	{
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
	void Delete(uint64_t Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key, Id))
		{
			ManagerLog::Crash("system", "cannot delete entity because (" + std::string(key.name()) + ") in (" + std::string(key.name()) + ") does not exist");
			return;
		}
		auto j = list[key].find(Id);
		j->second.reset();
		list[key].erase(j);
	}
	template <typename T>
	bool Check(uint64_t Id)
	{
		auto key = std::type_index(typeid(T));
		if (!Check(key)) return false;
		if (list[key].find(Id) == list[key].end()) return false;
		return true;
	}
private:
	std::unordered_map<std::type_index, std::unordered_map<uint64_t, std::shared_ptr<void>>> list;
	bool Check(std::type_index Key)
	{
		if (list.find(Key) == list.end()) return false;
		return true;
	}
	bool Check(std::type_index Key, uint64_t Id)
	{
		if (!Check(Key)) return false;
		if (list[Key].find(Id) == list[Key].end()) return false;
		return true;
	}
};
