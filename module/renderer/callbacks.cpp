#include "module.h"

#include <string>
#include <dependency/sfml/System.hpp>
#include <dependency/glm/gtc/type_ptr.hpp>
using namespace std;
using namespace v8;
using namespace glm;

#include "type/settings/type.h"
#include "type/camera/type.h"


void ModuleRenderer::Callbacks()
{
	Script->Bind("rendertarget",     jsRendertarget    );
	Script->Bind("renderpass",       jsRenderpass      );
	Script->Bind("rendertargetload", jsRendertargetload);
	Script->Bind("uniform",          jsUniform         );
	Script->Bind("wireframe",        jsWireframe       );
	Script->Bind("projection",       jsProjection      );
}

Handle<Value> ModuleRenderer::jsRendertarget(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();
	string name = *String::Utf8Value(args[0]);

	GLenum type = GL_RGB16F;
	if(1 < args.Length() && args[1]->IsString())
	{
		string ty = *String::Utf8Value(args[1]);

		if     (ty == "RGB16")              type = GL_RGB16;
		else if(ty == "RGB16F")             type = GL_RGB16F;
		else if(ty == "RGB32F")             type = GL_RGB32F;

		else if(ty == "RGBA16")             type = GL_RGBA16;
		else if(ty == "RGBA16F")            type = GL_RGBA16F;
		else if(ty == "RGBA32F")            type = GL_RGBA32F;

		else if(ty == "DEPTH_COMPONENT24")  type = GL_DEPTH_COMPONENT24;
		else if(ty == "DEPTH_COMPONENT32")  type = GL_DEPTH_COMPONENT32;
		else if(ty == "DEPTH_COMPONENT32F") type = GL_DEPTH_COMPONENT32F;

		else if(ty == "DEPTH24_STENCIL8")   type = GL_DEPTH24_STENCIL8;

		else ManagerLog::Warning("script", "unknown texture type");
	}

	float size = 1.0f;
	if(2 < args.Length() && args[2]->IsNumber())
		size = (float)args[2]->NumberValue();

	module->TextureCreate(name, type, size);

	return Undefined();
}

Handle<Value> ModuleRenderer::jsRenderpass(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();
	string name = *String::Utf8Value(args[0]);

	// helpers
	auto v8str = [](string String){ return String::New(String.c_str()); };
	auto stdstr = [](Handle<Value> Value){ return string(*String::Utf8Value(Value)); };

	// create new pass
	if(1 < args.Length() && args[1]->IsObject())
	{

		Handle<Object> object = args[1]->ToObject();

		// vertex path
		string vertex;
		if(object->Has(v8str("vertex")))
			vertex = stdstr(object->Get(v8str("vertex")));
		else vertex = "quad.vert";

		// fragment path
		string fragment;
		if(object->Has(v8str("fragment")))
			fragment = stdstr(object->Get(v8str("fragment")));
		else { ManagerLog::Fail("script", "fragment shader path needed"); return Undefined(); }

		// tagets as attachment to texture name
		unordered_map<GLenum, string> targets;
		if(object->Has(v8str("targets")))
		{
			Handle<Object> obj = object->Get(v8str("targets"))->ToObject();

			if(obj->Has(v8str("COLOR_ATTACHMENT0")))
				targets.insert(make_pair(GL_COLOR_ATTACHMENT0, stdstr(obj->Get(v8str("COLOR_ATTACHMENT0")))));
			if(obj->Has(v8str("COLOR_ATTACHMENT1")))
				targets.insert(make_pair(GL_COLOR_ATTACHMENT1, stdstr(obj->Get(v8str("COLOR_ATTACHMENT1")))));
			if(obj->Has(v8str("COLOR_ATTACHMENT2")))
				targets.insert(make_pair(GL_COLOR_ATTACHMENT2, stdstr(obj->Get(v8str("COLOR_ATTACHMENT2")))));
			if(obj->Has(v8str("COLOR_ATTACHMENT3")))
				targets.insert(make_pair(GL_COLOR_ATTACHMENT3, stdstr(obj->Get(v8str("COLOR_ATTACHMENT3")))));
			if(obj->Has(v8str("DEPTH_STENCIL_ATTACHMENT")))
				targets.insert(make_pair(GL_DEPTH_STENCIL_ATTACHMENT, stdstr(obj->Get(v8str("DEPTH_STENCIL_ATTACHMENT")))));
		}

		// samplers as shader location to texture name
		unordered_map<string, string> samplers;
		if(object->Has(v8str("samplers")))
		{
			Handle<Object> obj = object->Get(v8str("samplers"))->ToObject();

			Handle<Array> locations = obj->GetPropertyNames();
			for(unsigned int i = 0; i < locations->Length(); ++i)
			{
				string location = stdstr(locations->Get(i));
				string texture = stdstr(obj->Get(v8str(location)));
				samplers.insert(make_pair(location, texture));
			}
		}

		// fallbacks as target to texture name or clear value
		unordered_map<string, string> fallbacks;
		if(object->Has(v8str("fallbacks")))
		{
			Handle<Object> obj = object->Get(v8str("fallbacks"))->ToObject();

			Handle<Array> targets = obj->GetPropertyNames();
			for(unsigned int i = 0; i < targets->Length(); ++i)
			{
				string target = stdstr(targets->Get(i));
				Handle<Value> fallback = obj->Get(v8str(target));
				if(fallback->IsString())
					fallbacks.insert(make_pair(target, stdstr(fallback)));
				else if(fallback->IsNumber())
					{ /* how to store clean value? */ }
			}
		}

		// choose draw function
		Function function = QUAD;
		if(object->Has(v8str("draw")))
		{
			string func = stdstr(object->Get(v8str("draw")));
			if     (func == "FORMS" ) function = FORMS;
			else if(func == "SKY"   ) function = SKY;
			else if(func == "LIGHTS") function = LIGHTS;
			else if(func == "QUAD"  ) function = QUAD;
			else if(func == "SCREEN") function = SCREEN;
			else ManagerLog::Warning("script", "function is invalid");
		}

		// resolution relative to window size
		float size = 1.0f;
		if(object->Has(v8str("size")) && object->Get(v8str("size"))->IsNumber())
			size = (float)object->Get(v8str("size"))->NumberValue();

		// stencil parameters
		GLenum stencilfunc = GL_ALWAYS;
		GLint  stencilref  = 0;
		GLenum stencilop   = GL_KEEP;
		if(object->Has(v8str("stencil")))
		{
			Handle<Array> array = Handle<Array>::Cast(object->Get(v8str("stencil")));

			if(0 < array->Length() && array->Get(0)->IsString())
			{
				string func = stdstr(array->Get(0));
				if     (func == "ALWAYS" ) stencilfunc = GL_ALWAYS;
				else if(func == "EQUAL"  ) stencilfunc = GL_EQUAL;
				else if(func == "GREATER") stencilfunc = GL_GREATER;
				else if(func == "LESS"   ) stencilfunc = GL_LESS;
				else if(func == "GEQUAL" ) stencilfunc = GL_GEQUAL;
				else if(func == "LEQUAL" ) stencilfunc = GL_LEQUAL;
			}
			if(1 < array->Length() && array->Get(1)->IsInt32())
			{
				stencilref = array->Get(1)->Int32Value();
			}
			if(2 < array->Length() && array->Get(2)->IsString())
			{
				string op = stdstr(array->Get(2));
				if     (op == "KEEP"   ) stencilop = GL_KEEP;
				else if(op == "REPLACE") stencilop = GL_REPLACE;
				else if(op == "INCR"   ) stencilop = GL_INCR;
			}
		}

		// create pass
		module->PassCreate(name, vertex, fragment, targets, samplers, fallbacks, function, size, stencilfunc, stencilref, stencilop);
	}
	// toggle existing pass
	else
	{
		Pass *pass = module->PassGet(name);
		pass->Enabled = !pass->Enabled;
		ManagerLog::Print("script", string(pass->Enabled ? "enabled" : "disabled") + " (" + name + ") pass");
	}
	
	return Undefined();
}

/*
 * rendertargetload(name, path, repeat?, filtering?, mipmapping?)
 * Loads a texture into a render target.
 */
Handle<Value> ModuleRenderer::jsRendertargetload(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();
	string name = *String::Utf8Value(args[0]);

	if(args.Length() < 2 || !args[1]->IsString())
		return Undefined();
	string path = *String::Utf8Value(args[1]);

	bool repeat = true;
	if(2 < args.Length() && args[2]->IsBoolean())
		repeat = args[2]->BooleanValue();

	bool filtering = true;
	if(3 < args.Length() && args[3]->IsBoolean())
		filtering = args[3]->BooleanValue();

	bool mipmapping = true;
	if(4 < args.Length() && args[4]->IsBoolean())
		mipmapping = args[4]->BooleanValue();

	module->TextureLoad(name, path, repeat, filtering, mipmapping);

	return Undefined();
}

/*
 * uniform(pass, name, value)
 * Sets a shader uniform.
 * Currently, only the OpenGL types 2fv and Matrix4fv are supported.
 */
Handle<Value> ModuleRenderer::jsUniform(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return Undefined();
	string passname = *String::Utf8Value(args[0]);

	if(args.Length() < 2 || !args[1]->IsString())
		return Undefined();
	string name = *String::Utf8Value(args[1]);

	Pass *pass = module->PassGet(passname);
	if(!pass) { module->Log->Warning("script", "pass (" + passname + ") not found"); return Undefined(); }

	glUseProgram(pass->Program);
	GLuint location = glGetUniformLocation(pass->Program, name.c_str());

	if(args.Length() < 3)
		return Undefined();

	if(args[2]->IsArray())
	{
		Handle<Array> handle = Handle<Array>::Cast(args[2]);
		if(!handle->Length()) { module->Log->Warning("script", "uniform value is empty"); return Undefined(); }

		if(handle->Get(0)->IsNumber())
		{
			vector<GLfloat> values(handle->Length());
			for(unsigned int i = 0; i < handle->Length(); ++i)
				if(handle->Get(i)->IsNumber())
					values[i] = ((GLfloat)handle->Get(i)->NumberValue());

			if(values.size() == 2)
				glUniform2fv(location, 1, &values[0]);
			else if(values.size() == 16)
				glUniformMatrix4fv(location, 1, GL_FALSE, &values[0]);
			else { module->Log->Warning("script", "uniform type not supported"); return Undefined(); }
		}
		// else if(handle->Get(0)->Is...())
		// {
		//     ...
		// }
		else { module->Log->Warning("script", "uniform type not supported"); return Undefined(); }
	}
	else { module->Log->Warning("script", "uniform type not supported"); return Undefined(); }

	return Undefined();
}

/*
 * wireframe()
 * Toggles wireframe rendering.
 */
Handle<Value> ModuleRenderer::jsWireframe(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	stg->Set<bool>("Wireframe", !*stg->Get<bool>("Wireframe"));

	ManagerLog::Print("script", string(*stg->Get<bool>("Wireframe") ? "enabled" : "disabled") + " wireframe mode");
	return Undefined();
}

/*
 * projection()
 * Get projection matrix of current camera. Return value is an array of length 16.
 */
Handle<Value> ModuleRenderer::jsProjection(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	uint64_t id = *module->Global->Get<uint64_t>("camera");
	auto cam = module->Entity->Get<Camera>(id);
	mat4 projection = cam->Projection;
	float *value = value_ptr(projection);

	Handle<Array> result = Array::New(2);
	for(int i = 0; i < 16; ++i)
		result->Set(i, Number::New(value[i]));
	return result;
}
