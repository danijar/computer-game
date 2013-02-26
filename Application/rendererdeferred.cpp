#pragma once

#include "system.h"
#include "debug.h"
#include "opengl.h"
#include "shaders.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/matrix_transform.hpp>
using namespace glm;

#include "settings.h"
#include "form.h"
#include "transform.h"
#include "camera.h"


class ComponentRendererDeferred : public Component
{
	GLuint shader;
	/*
	GLuint shd_forms, shd_light, shd_fxaa, shd_screen;
	GLuint tex_position, tex_normal, tex_albedo, tex_light, tex_fxaa;
	GLuint fbo_forms, fbo_light, fbo_fxaa;
	GLuint forms_depth;
	unordered_map<string, GLuint> forms_targets, light_uniforms, light_targets, fxaa_uniforms, fxaa_targets, screen_uniforms;
	GLuint quad_positions, quad_texcoords;
	*/

	void Init()
	{
		Opengl::InitGlew(); 

		Pipeline();

		Resize();
		
		Listeners();
	}

	void Update()
	{
		Forms();

		/*
		Draw(shd_forms, fbo_forms, forms_targets.size());
		Draw(shd_light, light_uniforms, fbo_light, light_targets.size());
		Draw(shd_fxaa, fxaa_uniforms, fbo_fxaa, fxaa_targets.size());
		Draw(shd_screen, screen_uniforms);
		*/
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
			}
		});

		Event->Listen("WindowRecreated", [=]{
			Resize();
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Resize(Size);
		});
	}

	void Pipeline()
	{
		shader = Shaders::Create("shaders/basic.vert", "shaders/basic.frag");

		/*
		// quad
		const float POSITIONS[] = {-1.f,-1.f, 1.f,-1.f,-1.f, 1.f, 1.f, 1.f };
		const float TEXCOORDS[] = { 0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f };

		glGenBuffers(1, &quad_positions);
		glBindBuffer(GL_ARRAY_BUFFER, quad_positions);
		glBufferData(GL_ARRAY_BUFFER, sizeof(POSITIONS), POSITIONS, GL_STATIC_DRAW);

		glGenBuffers(1, &quad_texcoords);
		glBindBuffer(GL_ARRAY_BUFFER, quad_texcoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(TEXCOORDS), TEXCOORDS, GL_STATIC_DRAW);

		// pipeline
		shd_forms = Shaders::Create("shaders/forms.vert", "shaders/forms.frag");
		tex_position = Target(); tex_normal = Target(); tex_albedo = Target();
		forms_targets.insert(make_pair("position", tex_position));
		forms_targets.insert(make_pair("normal",   tex_normal  ));
		forms_targets.insert(make_pair("albedo",   tex_albedo  ));
		forms_depth = Depth();
		fbo_forms = Framebuffer(shd_forms, forms_targets, forms_depth);

		shd_light = Shaders::Create("shaders/quad.vert", "shaders/light.frag");
		light_uniforms.insert(make_pair("position_tex", tex_position));
		light_uniforms.insert(make_pair("normal_tex",   tex_normal  ));
		light_uniforms.insert(make_pair("albedo_tex",   tex_albedo  ));
		tex_light = Target();
		light_targets.insert(make_pair("image", tex_light));
		fbo_light = Framebuffer(shd_light, light_targets);

		shd_fxaa = Shaders::Create("shaders/quad.vert", "shaders/fxaa.frag");
		fxaa_uniforms.insert(make_pair("image_tex", tex_light));
		tex_fxaa = Target();
		fxaa_targets.insert(make_pair("image", tex_fxaa));
		fbo_fxaa = Framebuffer(shd_fxaa, fxaa_targets);

		shd_screen = Shaders::Create("shaders/quad.vert", "shaders/screen.frag");
		screen_uniforms.insert(make_pair("image_tex", tex_fxaa));
		*/
	}

	void Forms() // parameter GLuint shader
	{
		auto stg = Global->Get<StorageSettings>("settings");
		auto fms = Entity->Get<StorageForm>();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		glUseProgram(shader);
		
		GLuint model   = glGetUniformLocation(shader, "model"),
			   view    = glGetUniformLocation(shader, "view" ),
			   texture = glGetUniformLocation(shader, "tex"  );

		glUniformMatrix4fv(view, 1, GL_FALSE, value_ptr(Global->Get<StorageCamera>("camera")->View));

		glPolygonMode(GL_FRONT_AND_BACK, Global->Get<StorageSettings>("settings")->Wireframe ? GL_LINE : GL_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		/*
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		*/

		for(auto i : fms)
		{
			auto frm = Entity->Get<StorageForm>(i.first);
			auto tsf = Entity->Get<StorageTransform>(i.first);

			glUniformMatrix4fv(model, 1, GL_FALSE, value_ptr(tsf->Matrix));

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Vertices);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Normals);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, frm->Texcoords);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glBindTexture(GL_TEXTURE_2D, frm->Texture);
			glUniform1i(texture, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, frm->Elements);
			
			GLint size = 0;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
		}

		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);

		Opengl::Test();
	}

	void Resize()
	{
		Resize(Global->Get<RenderWindow>("window")->getSize());
	}

	mat4 Projection;
	void Resize(Vector2u Size)
	{
		auto stg = Global->Get<StorageSettings>("settings");

		glClearColor(.4f,.6f,.9f,0.f);

		glViewport(0, 0, Size.x, Size.y);

		glUseProgram(shader);
		Projection = perspective(stg->Fieldofview, (float)Size.x / (float)Size.y, 1.0f, stg->Viewdistance);
		glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, value_ptr(Projection));

		/*
		// framebuffer targets
		resizeTarget(tex_position, Size);
		resizeTarget(tex_normal,   Size);
		resizeTarget(tex_albedo,   Size);
		resizeTarget(tex_light,    Size);
		resizeTarget(tex_fxaa,     Size);
		resizeDepth (forms_depth,  Size);
		// shader uniforms
		// screen width, height
		*/
	}

	/*
	GLuint Target()
	{
		Vector2u size = Global->Get<RenderWindow>("window")->getSize();

		GLuint target;
		glGenTextures(1, &target);
		glBindTexture(GL_TEXTURE_2D, target);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		return target;
	}

	GLuint Depth()
	{
		Vector2u size = Global->Get<RenderWindow>("window")->getSize();

		GLuint target;
		glGenRenderbuffers(1, &target);
		glBindRenderbuffer(GL_RENDERBUFFER, target);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		return target;
	}

	void resizeTarget(GLuint target, Vector2u size)
	{
		glBindTexture(GL_TEXTURE_2D, target);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	
	void resizeDepth(GLuint target, Vector2u size)
	{
		glBindRenderbuffer(GL_RENDERBUFFER, target);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size.x, size.y);
	}

	GLuint Framebuffer(GLuint shader, unordered_map<string, GLuint> targets)
	{
		GLuint framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

		int n = 0; for(auto i : targets)
		{
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + n, GL_TEXTURE_2D, i.second, 0);
			glBindFragDataLocation(shader, n, i.first.c_str());
			n++;
		}

		Debug::PassFail("Renderer framebuffer creation", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
		glLinkProgram(shader);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return framebuffer;
	}

	GLuint Framebuffer(GLuint shader, unordered_map<string, GLuint> targets, GLuint depth)
	{
		GLuint framebuffer;
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

		int n = 0; for(auto i : targets)
		{
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + n, GL_TEXTURE_2D, i.second, 0);
			glBindFragDataLocation(shader, n, i.first.c_str());
			n++;
		}
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);

		Debug::PassFail("Renderer framebuffer creation", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
		glLinkProgram(shader);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		return framebuffer;
	}

	void Draw(GLuint shader, GLuint framebuffer, int targets)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

		vector<GLenum> buffers;
		for(int i = 0; i < targets; ++i)
		{
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(targets, &buffers[0]);

		Forms();
	}

	void Draw(GLuint shader, unordered_map<string, GLuint> uniforms, GLuint framebuffer, int targets)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);

		vector<GLenum> buffers;
		for(int i = 0; i < targets; ++i)
		{
			buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(targets, &buffers[0]);

		Quad(shader, uniforms);
	}

	void Draw(GLuint shader, unordered_map<string, GLuint> uniforms)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		Quad(shader, uniforms);
	}

	void Quad(GLuint shader, unordered_map<string, GLuint> uniforms)
	{
		glUseProgram(shader);
		glClear(GL_COLOR_BUFFER_BIT);

		int n = 0; for(auto i : uniforms)
		{
			glActiveTexture(GL_TEXTURE0 + n);
			glBindTexture(GL_TEXTURE_2D, i.second);
			glUniform1i(glGetUniformLocation(shader, i.first.c_str()), n);
			n++;
		}

		GLuint position = glGetAttribLocation(shader, "position");
		glEnableVertexAttribArray(position);
		glBindBuffer(GL_ARRAY_BUFFER, quad_positions);
		glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);

		GLuint texcoord = glGetAttribLocation(shader, "texcoord");
		glEnableVertexAttribArray(texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, quad_texcoords);
		glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(position);
		glDisableVertexAttribArray(texcoord);
		glActiveTexture(GL_TEXTURE0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);		
		glUseProgram(0);
	}
	*/
};
