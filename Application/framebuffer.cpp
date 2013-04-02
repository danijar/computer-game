#pragma once

#include "system.h"

#include <unordered_map>
using namespace std;
#include <GLEW/glew.h>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace sf;

#include "framebuffer.h"


class ModuleFramebuffer : public Module
{
	void Init()
	{
		Opengl->Init();

		Listeners();
	}

	void Update()
	{
		auto fbs = Entity->Get<StorageFramebuffer>();
		for(auto i = fbs.begin(); i != fbs.end(); ++i)
		{
			if(!i->second->Id)
			{
				glGenFramebuffers(1, &i->second->Id);
				Setup(i->first);
			}
		}
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

		Event->Listen("WindowRecreated", [=]{
			auto fbs = Entity->Get<StorageFramebuffer>();
			for(auto i = fbs.begin(); i != fbs.end(); ++i)
			{
				glDeleteFramebuffers(1, &i->second->Id);
				glGenFramebuffers(1, &i->second->Id);
				Setup(i->first);
			}
		});

		Event->Listen<Vector2u>("WindowResize", [=](Vector2u Size){
			Resize(Size);
		});
	}

	void Resize()
	{
		Resize(Global->Get<RenderWindow>("window")->getSize());
	}

	void Resize(Vector2u Size)
	{
		auto fbs = Entity->Get<StorageFramebuffer>();
		for(auto i : fbs)
			for(auto j : i.second->Targets)
				TextureResize(j.second.first, j.second.second, Vector2u(Vector2f(Size) * i.second->Size));
	}

	////////////////////////////////////////////////////////////
	// Framebuffers
	////////////////////////////////////////////////////////////

	void Setup(unsigned int Id)
	{
		auto frb = Entity->Get<StorageFramebuffer>(Id);
		Vector2u size = Global->Get<RenderWindow>("window")->getSize();

		glBindFramebuffer(GL_FRAMEBUFFER, frb->Id);

		vector<GLenum> buffers;
		for(auto i = frb->Targets.begin(); i != frb->Targets.end(); ++i)
		{
			TextureResize(i->second.first, i->second.second, Vector2u(Vector2f(size) * frb->Size));
			glFramebufferTexture2D(GL_FRAMEBUFFER, i->first, GL_TEXTURE_2D, i->second.first, 0);
			if(i->first != GL_DEPTH_ATTACHMENT) buffers.push_back(i->first); // check for color attachment instead
		}
		glDrawBuffers(buffers.size(), &buffers[0]);

		Debug->PassFail("Framebuffer setup", (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	////////////////////////////////////////////////////////////
	// Textures
	////////////////////////////////////////////////////////////

	void TextureResize(GLuint Id, GLenum Type)
	{
		TextureResize(Id, Type, Global->Get<RenderWindow>("window")->getSize());
	}

	void TextureResize(GLuint Id, GLenum Type, Vector2u Size)
	{
		auto format = TextureFormat(Type);
		glBindTexture(GL_TEXTURE_2D, Id);
		glTexImage2D(GL_TEXTURE_2D, 0, Type, Size.x, Size.y, 0, format.first, format.second, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	pair<GLenum, GLenum> TextureFormat(GLenum InternalType)
	{
		GLenum type;
		switch (InternalType)
		{
		case GL_RGBA16:
		case GL_RGBA16F:
		case GL_RGBA32F:
			type = GL_RGBA;
			break;
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32:
		case GL_DEPTH_COMPONENT32F:
			type = GL_DEPTH_COMPONENT;
		}

		GLenum format;
		switch (InternalType)
		{
		case GL_RGBA16:
		case GL_DEPTH_COMPONENT24:
		case GL_DEPTH_COMPONENT32:
			format = GL_UNSIGNED_BYTE;
			break;
		case GL_RGBA16F:
		case GL_RGBA32F:
		case GL_DEPTH_COMPONENT32F:
			format = GL_FLOAT;
			break;
		}

		return make_pair(type, format);
	}
};
