#pragma once

#include "system.h"


class ModuleFramebuffer : public Module
{
	void Init()
	{
		Listeners();
	}

	void Update()
	{

	}

	void Listeners()
	{
		/*
		 * on window resize
		 * get window size
		 *    for each framebuffer
		 *        set Changed = true  // just for expressiveness
		 *        for each target
		 *            resize to window size
		 *        do what ever necessary
		 *        set Changed = false // just for expressiveness
		 */

		/*
		 * on window recreation
		 * get window size
		 *    for each framebuffer
		 *        set Changed = true  // just for expressiveness
		 *        delete opengl object
		 *        create new opengl object
		 *        for each target
		 *            resize to window size
		 *            bind to framebuffer again
		 *        do what ever necessary
		 *        set Changed = false // just for expressiveness
		 */
	}
};
