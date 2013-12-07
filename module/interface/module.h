#pragma once

#include "system/system.h"

#include "dependency/sfml/Graphics.hpp"


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
	static v8::Handle<v8::Value> jsInterface(const v8::Arguments& args);
	static v8::Handle<v8::Value> jsDebug    (const v8::Arguments& args);
};
