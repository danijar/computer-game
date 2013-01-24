#pragma once

#include "system.h"
#include "debug.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
using namespace glm;

#include "settings.h"
#include "window.h"
#include "shader.h"
#include "form.h"
#include "transform.h"
#include "camera.h"


class ComponentRenderer : public Component
{
	void Init()
	{
		Window();

		Listeners();
	}

	void Update()
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto cam = Global->Get<StorageCamera>("camera");
		auto fms = Entity->Get<StorageForm>();

		// draw geometry into main framebuffer

		glUseProgram(shd->Program);
		glUniformMatrix4fv(shd->UniView, 1, GL_FALSE, value_ptr(cam->View));

		Prepare();

		for(auto i = fms.begin(); i != fms.end(); ++i) Draw(i->first);

		Cleanup();

		// apply effects using more framebuffers
		// Pass(shader_paths, list<string_shader_in, input_texture_id>, list<string_shader_out, ouput_texture_id*>) // use C++11 initializer lists

		// draw to screen (which is the default framebuffer)
	}

	void Listeners()
	{
		Event->Listen<Keyboard::Key>("InputKeyReleased", [=](Keyboard::Key Code){
			auto stg = Global->Get<StorageSettings>("settings");
			switch(Code)
			{
			case Keyboard::Key::F2:
				stg->Wireframe = !stg->Wireframe;
				break;
			case Keyboard::Key::F3:
				stg->Verticalsync = !stg->Verticalsync;
				auto wnd = &Global->Get<StorageWindow>("window")->Window;
				wnd->setVerticalSyncEnabled(stg->Verticalsync);
			}
		});

		Event->Listen("WindowRecreated", [=]{
			Window();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Perspective(Size);
		});
	}

	void Draw(unsigned int id)
	{
		auto shd = Global->Get<StorageShader>("shader");
		auto frm = Entity->Get<StorageForm>(id);
		auto tsf = Entity->Get<StorageTransform>(id);

		mat4 Scale      = scale(mat4(1), frm->Scale);
		mat4 Translate  = translate(mat4(1), tsf->Position);
		mat4 Rotate     = rotate(mat4(1), tsf->Rotation.x, vec3(1, 0 ,0))
		                * rotate(mat4(1), tsf->Rotation.y, vec3(0, 1, 0))
		                * rotate(mat4(1), tsf->Rotation.z, vec3(0, 0, 1));

		glUseProgram(frm->Program);
		mat4 Vertex = Translate * Rotate * Scale;
		glUniformMatrix4fv(shd->UniVertex, 1, GL_FALSE, value_ptr(Vertex));
		mat4 Normal = Rotate;
		glUniformMatrix4fv(shd->UniNormal, 1, GL_FALSE, value_ptr(Normal));

		glEnableVertexAttribArray(shd->AtrVertex);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
		glVertexAttribPointer(shd->AtrVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shd->AtrNormal);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
		glVertexAttribPointer(shd->AtrNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(shd->AtrTexcoord);
		glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
		glVertexAttribPointer(shd->AtrTexcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);

		glBindTexture(GL_TEXTURE_2D, frm->Texture);

		int count; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
		glDrawElements(GL_TRIANGLES, count/sizeof(GLuint), GL_UNSIGNED_INT, 0);
	}

	void Prepare()
	{
		auto stg = Global->Get<StorageSettings>("settings");

		glPolygonMode(GL_FRONT_AND_BACK, stg->Wireframe ? GL_LINE : GL_FILL);
	}

	void Cleanup()
	{
		auto shd = Global->Get<StorageShader>("shader");
		
		glUseProgram(0);
		glDisableVertexAttribArray(shd->AtrVertex);
		glDisableVertexAttribArray(shd->AtrNormal);
		glDisableVertexAttribArray(shd->AtrTexcoord);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	GLuint Pass(GLuint program)
	{
		auto stg = Global->Get<StorageSettings>("settings");

		GLuint fbo;
		GLuint fbo_depth;
		GLuint output;

		glGenRenderbuffers(1, &fbo_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, fbo_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, stg->Size.x, stg->Size.y);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glGenTextures(1, &output);
		glBindTexture(GL_TEXTURE_2D, output);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stg->Size.x, stg->Size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fbo_depth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
		Debug::PassFail("Renderer framebuffer creation", result);

		glUseProgram(program);
		glBindTexture(GL_TEXTURE_2D, output);
		glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f); // The bottom left corner
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f); // The top left corner
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f); // The top right corner
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f); // The bottom right corner

		glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);

		return output;
	}

	void Window()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		auto stg = Global->Get<StorageSettings>("settings");
	
		wnd->setVerticalSyncEnabled(stg->Verticalsync);
	
		glClearColor(.4f,.6f,.9f,0.f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);

		Perspective();
	}

	void Perspective()
	{
		auto wnd = &Global->Get<StorageWindow>("window")->Window;
		Perspective(wnd->getSize());
	}

	void Perspective(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto shd = Global->Get<StorageShader>("shader");

		glViewport(0, 0, Size.x, Size.y);

		glUseProgram(shd->Program);
		mat4 Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
		glUniformMatrix4fv(shd->UniProjection, 1, GL_FALSE, value_ptr(Projection));
	}

	bool OpenglTest()
	{
		GLenum code = glGetError();
		string message;

		if (code == GL_NO_ERROR) return true;
		else if(code == GL_INVALID_ENUM)                      message = "GL_INVALID_ENUM";
		else if(code == GL_INVALID_VALUE)                     message = "GL_INVALID_VALUE";
		else if(code == GL_INVALID_OPERATION)                 message = "GL_INVALID_OPERATION";
		else if(code == GL_STACK_OVERFLOW)                    message = "GL_STACK_OVERFLOW";
		else if(code == GL_STACK_UNDERFLOW)                   message = "GL_STACK_UNDERFLOW";
		else if(code == GL_OUT_OF_MEMORY)                     message = "GL_OUT_OF_MEMORY";
		else if(code == GL_INVALID_FRAMEBUFFER_OPERATION_EXT) message = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";

		Debug::Fail("OpenGL error " + message);
		return false;
	}
};