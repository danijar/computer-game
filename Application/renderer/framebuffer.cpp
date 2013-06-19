#include "module.h"

#include <GLEW/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
using namespace std;
using namespace sf;


GLuint ModuleRenderer::CreateFramebuffer(unordered_map<GLenum, GLuint> Targets)
{
	GLuint framebuffer;
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	vector<GLenum> buffers;
	for(auto i : Targets)
	{
		// bind target to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, i.first, GL_TEXTURE_2D, i.second, 0);

		// keep track of draw buffers
		if(GL_COLOR_ATTACHMENT0 <= i.first && i.first <= GL_COLOR_ATTACHMENT0 + GL_MAX_COLOR_ATTACHMENTS - 1)
			buffers.push_back(i.first);

		// check if it is really bound
		GLint type;
		glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, i.first, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type);
		if(type != GL_TEXTURE) Debug->Fail("framebuffer target could not be attached");
	}
	glDrawBuffers(buffers.size(), &buffers[0]);

	Debug->PassFail("framebuffer setup", glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return framebuffer;
}

void ModuleRenderer::TextureCreate(string Name, GLenum Type, float Size)
{
	GLuint id;
	glGenTextures(1, &id);
	TextureResize(id, Type, Size);
	textures.insert(make_pair(Name, make_tuple(id, Type, Size)));
}

tuple<GLuint, GLenum, float> ModuleRenderer::TextureGet(string Name)
{
	auto i = textures.find(Name);
	if(i != textures.end())
		return i->second;
	else
	{
		Debug->Fail("texture (" + Name + ") was not found");
		return make_tuple(0, 0, 0);
	}
}

void ModuleRenderer::TextureResize(GLuint Id, GLenum Type, float Size)
{
	auto size = Global->Get<RenderWindow>("window")->getSize();

	auto format = TextureFormat(Type);
	glBindTexture(GL_TEXTURE_2D, Id);
	glTexImage2D(GL_TEXTURE_2D, 0, Type, int(size.x * Size), int(size.y * Size), 0, format.first, format.second, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

pair<GLenum, GLenum> ModuleRenderer::TextureFormat(GLenum InternalType)
{
	GLenum type = 0;
	switch (InternalType)
	{
	case GL_RGB16:
	case GL_RGB16F:
	case GL_RGB32F:
		type = GL_RGB;
		break;
	case GL_RGBA16:
	case GL_RGBA16F:
	case GL_RGBA32F:
		type = GL_RGBA;
		break;
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
	case GL_DEPTH_COMPONENT32F:
		type = GL_DEPTH_COMPONENT;
		break;
	case GL_DEPTH24_STENCIL8:
	case GL_DEPTH32F_STENCIL8:
		type = GL_DEPTH_STENCIL;
		break;
	}

	GLenum format = 0;
	switch (InternalType)
	{
	case GL_RGB16:
	case GL_RGBA16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32:
		format = GL_UNSIGNED_BYTE;
		break;
	case GL_DEPTH24_STENCIL8:
		format = GL_UNSIGNED_INT_24_8;
		break;
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_DEPTH_COMPONENT32F:
		format = GL_FLOAT;
		break;
	case GL_DEPTH32F_STENCIL8:
		format = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
		break;
	}

	if(!type || !format)
		Debug->Fail("havn't found type of texture");

	return make_pair(type, format);
}
