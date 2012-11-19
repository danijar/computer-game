#pragma once

struct Two
{
	int X, Y;
	Two() : X(0), Y(0) {}
	Two(int X, int Y) : X(X), Y(Y) {}
};

typedef Two Size;
typedef Two Position;