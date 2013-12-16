#include "module.h"

#include <functional>
#include <dependency/sfml/Graphics.hpp>
#include <dependency/sfml/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;

#include "type/print/type.h"


void ModuleInterface::DrawPrint()
{
	auto wnd = Global->Get<RenderWindow>("window");
	auto prs = Entity->Get<Print>();

	const int margin = 4;
	const int textsize = 15;

	unsigned int offset = margin / 2;
	for(auto i : prs)
	{
		string chars = i.second->Text();
		Text text(chars, font, textsize);
		text.setPosition((float)margin, (float)offset);
		wnd->draw(text);

		int lines = 1 + count(chars.begin(), chars.end(), '\n');
		offset += lines * (margin + textsize);
	}
}
