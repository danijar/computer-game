#include "module.h"

#include <BULLET/btBulletDynamicsCommon.h>
#include <GLM/gtc/type_ptr.hpp>
#include <SFML/OpenGL.hpp>
#include <GLEW/glew.h>
using namespace glm;

#include "settings.h"
#include "camera.h"


ModulePhysic::DebugDrawer::DebugDrawer(ManagerEntity *Entity, ManagerGlobal *Global, HelperFile *File, HelperDebug *Debug) : Entity(Entity), Global(Global), File(File), Debug(Debug), mode(btIDebugDraw::DBG_NoDebug)
{
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	{ string source = File->Read("shader/shader.vert");
	const GLchar* chars = source.c_str();
	glShaderSource(vertex, 1, &chars, NULL);
	glCompileShader(vertex); }

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	{ string source = File->Read("shader/shader.frag");
	const GLchar* chars = source.c_str();
	glShaderSource(fragment, 1, &chars, NULL);
	glCompileShader(fragment); }

	shader = glCreateProgram();
	glAttachShader(shader, vertex);
	glAttachShader(shader, fragment);
	glLinkProgram(shader);

	GLint status;
	glGetProgramiv(shader, GL_LINK_STATUS, &status);

	if(status != GL_TRUE) Debug->Fail("program creation fail");

	glDetachShader(shader, vertex);
	glDetachShader(shader, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ModulePhysic::DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	auto cam = Entity->Get<StorageCamera>(*Global->Get<unsigned int>("camera"));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, value_ptr(cam->Projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, value_ptr(cam->View));

	glBegin(GL_LINES);
		glVertex3f(from.getX(), from.getY(), from.getZ());
		glVertex3f(to.getX(), to.getY(), to.getZ());
	glEnd();

	glUseProgram(0);
}

void ModulePhysic::DebugDrawer::setDebugMode(int debugMode)
{
	mode = debugMode;
}

int	ModulePhysic::DebugDrawer::getDebugMode() const
{
	return mode;
}
