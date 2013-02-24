#pragma once

#include <SFML/Window/Keyboard.hpp>
using namespace sf;

typedef Keyboard::Key Key;
inline bool KeyDown(Keyboard::Key key) { return Keyboard::isKeyPressed(key); }