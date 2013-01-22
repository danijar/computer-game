#pragma once

#include "system.h"
#include "debug.h"

#include <functional>
using namespace std;
#include <SFML/Graphics.hpp>
using namespace sf;
#include <GLM/glm.hpp>
using namespace glm;

#include "window.h"
#include "text.h"


class ComponentText : public Component
{
	Font font;

	void Init()
	{
		bool result = font.loadFromFile("other/courier.ttf");
		Debug::PassFail("Text font loading", result);

		Listeners();
	}

	void Update()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		auto txs = Entity->Get<StorageText>();

		wnd->pushGLStates();
		
		const int margin = 3;
		const int textsize = 16;

		uint offset = 0;
		for(auto i : txs)
		{
			auto func = i.second->Text;
			string chars = func();
			Text text(chars, font, textsize);
			text.move((float)margin, (float)offset);
			wnd->draw(text);
			offset += margin + textsize;
		}

		wnd->popGLStates();
	}

	void Listeners()
	{
		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			// ...
		});
	}
};