#pragma once

#include "system.h"

#include <functional>
using namespace std;
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
using namespace glm;

#include "text.h"


class ModuleText : public Module
{
	Font font;

	void Init()
	{
		bool result = font.loadFromFile("interface/" + Name() + "/source.ttf");
		Debug->PassFail("font loading", result);

		Listeners();
	}

	void Update()
	{
		auto wnd = Global->Get<RenderWindow>("window");
		auto txs = Entity->Get<StorageText>();

		wnd->pushGLStates();
		
		const int margin = 4;
		const int textsize = 15;

		uint offset = margin / 2;
		for(auto i : txs)
		{
			auto func = i.second->Text;
			string chars = func();
			Text text(chars, font, textsize);
			text.setPosition((float)margin, (float)offset);
			wnd->draw(text);
			offset += margin + textsize;
		}

		wnd->popGLStates();
	}

	void Listeners()
	{
		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Global->Get<RenderWindow>("window")->setView(View(FloatRect(0, 0, (float)Size.x, (float)Size.y)));
		});
	}
};
