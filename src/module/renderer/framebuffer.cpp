#include "module.h"

#include <glew/glew.h>
#include <sfml/OpenGL.hpp>
#include <sfml/Graphics/RenderWindow.hpp>

using namespace std;
using namespace sf;

GLuint ModuleRenderer::FramebufferCreate(unordered_map<GLenum, GLuint> Targets)
{
	GLuint id;
	glGenFramebuffers(1, &id);
	FramebufferTargets(id, Targets);
	return id;
}

void ModuleRenderer::FramebufferTarget(GLuint Id, GLenum Attachment, GLuint Target, bool Output)
{
	unordered_map<GLenum, GLuint> targets;
	targets.insert(make_pair(Attachment, Target));
	FramebufferTargets(Id, targets, Output);
}

void ModuleRenderer::FramebufferTargets(GLuint Id, unordered_map<GLenum, GLuint> Targets, bool Output)
{
	glBindFramebuffer(GL_FRAMEBUFFER, Id);

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
		if(type != GL_TEXTURE && Output) Log->Fail("framebuffer target could not be attached");
	}
	if (buffers.size())
		glDrawBuffers(buffers.size(), &buffers[0]);

	if(Output) Log->PassFail("framebuffer setup", glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}

void ModuleRenderer::TextureCreate(string Name, GLenum Type, float Size)
{
	GLuint id;
	glGenTextures(1, &id);
	if(Size)
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
		Log->Fail("texture (" + Name + ") was not found");
		return make_tuple(0, 0, 0.0f);
	}
}

void ModuleRenderer::TextureLoad(string Name, string Path, bool Repeat, bool Filtering, bool Mipmapping)
{
	Image image;
	bool result = image.loadFromFile("asset/" + this->Name() + "/texture/" + Path);
	if(!result)
	{
		Log->Fail("texture (" + Path + ") cannot be loaded.");
		return;
	}
	image.flipVertically();

	auto texture = TextureGet(Name);
	if(get<0>(texture) == 0) return;
	auto format = TextureFormat(get<1>(texture));

	glBindTexture(GL_TEXTURE_2D, get<0>(texture));
	glTexImage2D(GL_TEXTURE_2D, 0, get<1>(texture), image.getSize().x, image.getSize().y, 0, format.first, format.second, image.getPixelsPtr());
	if(Repeat)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	if(Filtering)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		if(Mipmapping)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		if(Mipmapping)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	if(Mipmapping)
		glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
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
		Log->Fail("havn't found type of texture");

	return make_pair(type, format);
}
