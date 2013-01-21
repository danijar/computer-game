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
//#include "text.h"


class ComponentText : public Component
{
	void Init()
	{
		bool result = font.loadFromFile("other/verdana.ttf");
		Debug::PassFail("Text font loading", result);

		/*
		// testing
		unsigned int id = Entity->New();
		auto txt = Entity->Add<StorageText>(id);
		txt->Text = []{ return "Hello World!"; };
		*/
	}

	void Update()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;

		/*
		auto txs = Entity->Get<StorageText>();
		
		uint offset = 0;
		const int margin = 10;
		const int textsize = 15;

		for(auto i : txs)
		{
			auto func = i.second->Text;
			string chars = func();
			Text text(chars, font, textsize);
			text.move((float)margin, (float)offset);
			wnd->draw(text);
			offset += margin + textsize;
		}
		*/

		wnd->pushGLStates();

		Text text("Hello World", font, 15);
		text.setPosition(0, 0);
		wnd->draw(text);

		Debug::Pass("Text drawn.");

		wnd->popGLStates();
	
	}

	Font font;
};