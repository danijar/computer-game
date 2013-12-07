#include "module.h"

#include <string>

#include "dependency/glew/glew.h"
#include "dependency/glm/gtc/type_ptr.hpp"
#include "dependency/sfml/OpenGL.hpp"
#include "dependency/bullet/btBulletDynamicsCommon.h"

#include "type/settings/type.h"
#include "type/camera/type.h"

using namespace std;
using namespace glm;


ModulePhysic::DebugDrawer::DebugDrawer(ManagerEntity *Entity, ManagerGlobal *Global, HelperFile *File, ManagerLog *Log) : Entity(Entity), Global(Global), File(File), Log(Log), mode(btIDebugDraw::DBG_NoDebug)
{
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	{ string source = File->Read("shader/shader.vert");
	const GLchar *chars = source.c_str();
	glShaderSource(vertex, 1, &chars, NULL);
	glCompileShader(vertex); }

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	{ string source = File->Read("shader/shader.frag");
	const GLchar *chars = source.c_str();
	glShaderSource(fragment, 1, &chars, NULL);
	glCompileShader(fragment); }

	shader = glCreateProgram();
	glAttachShader(shader, vertex);
	glAttachShader(shader, fragment);
	glLinkProgram(shader);

	GLint status;
	glGetProgramiv(shader, GL_LINK_STATUS, &status);

	if(status != GL_TRUE) Log->Fail("program creation fail");

	glDetachShader(shader, vertex);
	glDetachShader(shader, fragment);
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void ModulePhysic::DebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	auto cam = Entity->Get<Camera>(*Global->Get<uint64_t>("camera"));

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
