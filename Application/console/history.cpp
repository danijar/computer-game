#include "module.h"


void ModuleConsole::History()
{
	if(history.size() < 1) return;
	
	if(history_index < history.size())
		text = history[history_index];
	else
		text = "";
}
