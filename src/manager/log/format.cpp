#include "manager.h"

using namespace std;

string ManagerLog::Format(float Value, unsigned int Head, unsigned int Tail)
{
	string text = to_string((int)(Value * pow(10, Tail)));
	while (text.length() < Tail + 1)    text = "0" + text;
	while (text.length() < Tail + Head) text = " " + text;
	text.insert(text.length() - Tail, 1, '.');
	return text;
}
