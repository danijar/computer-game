#include "module.h"

#include <sfml/Graphics/RenderWindow.hpp>
using namespace sf;

#include "type/settings/type.h"
#include "type/print/type.h"


void ModuleInterface::Init()
{
	bool result = font.loadFromFile("module/" + Name() + "/font/" + "source.ttf");
	Log->PassFail("font loading", result);

	crosshair = LoadImage("texture/crosshair.png");
	crosshair.second->setOrigin((Vector2f)crosshair.first->getSize() / 2.0f);
	Resize();

	show = true;

	Listeners();
	Callbacks();
}

void ModuleInterface::Update()
{
	if(!show) return;

	auto wnd = Global->Get<RenderWindow>("window");
	wnd->pushGLStates();

	// normal interface
	DrawCrosshair();

	// debug interface
	if(*Global->Get<Settings>("settings")->Get<bool>("Debug"))
		DrawPrint();

	wnd->popGLStates();
}

void ModuleInterface::Listeners()
{
	Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
		Resize(Size);
	});

	Event->Listen("WindowRecreated", [=]{
		Resize();
	});
}

void ModuleInterface::Resize()
{
	Vector2u size = Global->Get<RenderWindow>("window")->getSize();
	Resize(size);
}

void ModuleInterface::Resize(Vector2u Size)
{
	Vector2f size = Vector2f(Size);

	Global->Get<RenderWindow>("window")->setView(View(FloatRect(0, 0, size.x, size.y)));

	crosshair.second->setPosition(size / 2.0f);
}
