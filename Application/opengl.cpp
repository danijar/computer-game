#include "opengl.h"


namespace Opengl
{
	bool InitGlew()
	{
		bool result = (glewInit() == GLEW_OK);
		Debug::PassFail("Glew initialization",  result);
		return result;
	}

	int Test()
	{
		GLenum result;
		int count;
		string message;

		for(count = 0; (result = glGetError()) != GL_NO_ERROR; ++count)
		{
				 if(result == GL_INVALID_ENUM)                      message = "GL_INVALID_ENUM";
			else if(result == GL_INVALID_VALUE)                     message = "GL_INVALID_VALUE";
			else if(result == GL_INVALID_OPERATION)                 message = "GL_INVALID_OPERATION";
			else if(result == GL_STACK_OVERFLOW)                    message = "GL_STACK_OVERFLOW";
			else if(result == GL_STACK_UNDERFLOW)                   message = "GL_STACK_UNDERFLOW";
			else if(result == GL_OUT_OF_MEMORY)                     message = "GL_OUT_OF_MEMORY";
			else if(result == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) message = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";

			Debug::Fail(message);
		}
		if(count > 0) Debug::Info(to_string(count) + " OpenGL error" + (count > 1 ? "s" : "") + ".");

		return count;
	}
}