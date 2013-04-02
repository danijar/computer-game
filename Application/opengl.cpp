#include "opengl.h"

#include <iostream>
using namespace std;


namespace Opengl
{
	bool InitGlew()
	{
		bool result = (glewInit() == GLEW_OK);
		cout << "Glew initialization" << " " << (result ? "success" : "fail") << endl;
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

			cout << message << endl;
		}
		if(count > 0) cout << count << " OpenGL error" << (count > 1 ? "s" : "") << endl;

		return count;
	}
}