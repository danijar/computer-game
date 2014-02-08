#pragma once

#include <string>
#include <deque>
#include <unordered_map>


class ManagerLogic
{
public:
	float Average(std::string Name, float Value, unsigned int Times) // allow average over given time later on
	{
		// find queue and insert if not exist
		auto i = averages.find(Name);
		if (i == averages.end())
			i = averages.insert(std::make_pair(Name, std::deque<float>())).first;

		// push new element
		auto deque = &i->second;
		if (deque->size() + 1 > Times)
			deque->pop_front();
		deque->push_back(Value);

		// average over all elements
		float sum = 0;
		for (auto i = deque->begin(); i != deque->end(); ++i)
			sum += *i;
		return sum / deque->size();
	}
private:
	std::unordered_map<std::string, std::deque<float>> averages;
};
