#pragma once

#include "system/system.h"

#include <sfml/Graphics.hpp>


class ModuleInterface : public Module
{
	// general
	bool show;
	void Init();
	void Update();
	void Listeners();
	void Callbacks();
	void Resize();
	void Resize(sf::Vector2u Size);

	// print
	sf::Font font;
	void DrawPrint();
	
	// crosshair
	std::pair<sf::Texture*, sf::Sprite*> crosshair;
	std::pair<sf::Texture*, sf::Sprite*> LoadImage(std::string Path);
	void DrawCrosshair();

public:
	// callbacks
	static void jsInterface(const v8::FunctionCallbackInfo<v8::Value> &args);
	static void jsDebug    (const v8::FunctionCallbackInfo<v8::Value> &args);
};
