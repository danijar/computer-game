#include "module.h"

#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "print.h"


void ModuleInterface::Init()
{
	bool result = font.loadFromFile(Name() + "/font/" + "source.ttf");
	Debug->PassFail("font loading", result);

	Listeners();
}

void ModuleInterface::Update()
{
	auto wnd = Global->Get<RenderWindow>("window");

	wnd->pushGLStates();
	DrawPrint();
	wnd->popGLStates();
}

void ModuleInterface::Listeners()
{
	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		Global->Get<RenderWindow>("window")->setView(View(FloatRect(0, 0, (float)Size.x, (float)Size.y)));
	});
}
