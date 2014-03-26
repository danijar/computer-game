#pragma once

#include <string>

#include <glew/glew.h>

#include "manager/log/manager.h"


class HelperOpengl
{
public:
	HelperOpengl(std::string Name) : name(Name) {}

	bool Init()
	{
		return Init(name);
	}
	int Test()
	{
		return Test(name);
	}

	static bool Init(std::string Name)
	{
		bool result = (glewInit() == GLEW_OK);
		ManagerLog::PassFail(Name, "glew initialization", result);
		return result;
	}
	static int Test(std::string Name)
	{
		GLenum result;
		int count;
		std::string message;

		for (count = 0; (result = glGetError()) != GL_NO_ERROR; ++count) {
			     if(result == GL_INVALID_ENUM)                      message = "GL_INVALID_ENUM";
			else if(result == GL_INVALID_VALUE)                     message = "GL_INVALID_VALUE";
			else if(result == GL_INVALID_OPERATION)                 message = "GL_INVALID_OPERATION";
			else if(result == GL_STACK_OVERFLOW)                    message = "GL_STACK_OVERFLOW";
			else if(result == GL_STACK_UNDERFLOW)                   message = "GL_STACK_UNDERFLOW";
			else if(result == GL_OUT_OF_MEMORY)                     message = "GL_OUT_OF_MEMORY";
			else if(result == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) message = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";

			ManagerLog::Fail("", message);
		}
		if (count > 0) ManagerLog::Fail(Name, std::to_string(count) + " OpenGL error" + (count > 1 ? "s" : ""));

		return count;
	}
private:
	std::string name;
};
