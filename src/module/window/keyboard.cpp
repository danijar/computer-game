#include "module.h"

#include <string>
#include <sfml/Window.hpp>
using namespace std;
using namespace sf;

Keyboard::Key ModuleWindow::Key(string Name)
{
	sf::Keyboard::Key key;
	if     (Name == "A") key = sf::Keyboard::A;
	else if(Name == "B") key = sf::Keyboard::B;
	else if(Name == "C") key = sf::Keyboard::C;
	else if(Name == "D") key = sf::Keyboard::D;
	else if(Name == "E") key = sf::Keyboard::E;
	else if(Name == "F") key = sf::Keyboard::F;
	else if(Name == "G") key = sf::Keyboard::G;
	else if(Name == "H") key = sf::Keyboard::H;
	else if(Name == "I") key = sf::Keyboard::I;
	else if(Name == "J") key = sf::Keyboard::J;
	else if(Name == "K") key = sf::Keyboard::K;
	else if(Name == "L") key = sf::Keyboard::L;
	else if(Name == "M") key = sf::Keyboard::M;
	else if(Name == "N") key = sf::Keyboard::N;
	else if(Name == "O") key = sf::Keyboard::O;
	else if(Name == "P") key = sf::Keyboard::P;
	else if(Name == "Q") key = sf::Keyboard::Q;
	else if(Name == "R") key = sf::Keyboard::R;
	else if(Name == "S") key = sf::Keyboard::S;
	else if(Name == "T") key = sf::Keyboard::T;
	else if(Name == "U") key = sf::Keyboard::U;
	else if(Name == "V") key = sf::Keyboard::V;
	else if(Name == "W") key = sf::Keyboard::W;
	else if(Name == "X") key = sf::Keyboard::X;
	else if(Name == "Y") key = sf::Keyboard::Y;
	else if(Name == "Z") key = sf::Keyboard::Z;
	else if(Name == "0") key = sf::Keyboard::Num0;
	else if(Name == "1") key = sf::Keyboard::Num1;
	else if(Name == "2") key = sf::Keyboard::Num2;
	else if(Name == "3") key = sf::Keyboard::Num3;
	else if(Name == "4") key = sf::Keyboard::Num4;
	else if(Name == "5") key = sf::Keyboard::Num5;
	else if(Name == "6") key = sf::Keyboard::Num6;
	else if(Name == "7") key = sf::Keyboard::Num7;
	else if(Name == "8") key = sf::Keyboard::Num8;
	else if(Name == "9") key = sf::Keyboard::Num9;
	else if(Name == "Shift") key = sf::Keyboard::LShift;
	else if(Name == "Ctrl" ) key = sf::Keyboard::LControl;
	else if(Name == "Space") key = sf::Keyboard::Space;
	else if(Name == "Enter") key = sf::Keyboard::Return;
	return key;
}

bool ModuleWindow::Pressed(vector<Keyboard::Key> Keys)
{
	for (auto i : Keys)
		if (!Keyboard::isKeyPressed(i))
			return false;
	return true;
}
