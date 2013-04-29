#include "module.h"

#include <functional>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;

#include "text.h"


void ModuleInterface::DrawText()
{
	auto wnd = Global->Get<RenderWindow>("window");
	auto txs = Entity->Get<StorageText>();
		
	const int margin = 4;
	const int textsize = 15;

	uint offset = margin / 2;
	for(auto i : txs)
	{
		string chars = i.second->Text();
		Text text(chars, font, textsize);
		text.setPosition((float)margin, (float)offset);
		wnd->draw(text);

		int lines = 1 + count(chars.begin(), chars.end(), '\n');
		offset += lines * (margin + textsize);
	}
}
