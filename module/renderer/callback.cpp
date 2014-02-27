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
	Script->Bind("renderpass",       jsRenderpass      );
	Script->Bind("rendertarget",     jsRendertarget    );
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

/*
 * renderpass(name, { vertex, fragment, targets: { attachment: target, ... }, samplers: { location: target, ... }, fallbacks: { target: copy/color, ... }, draw, clear, size, stencil: [ function, reference, operation ] })
 * Creates a new render target. If only name parameter is passed, it toggles existing render pass.
 */
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
		unordered_map<string, string> copyfallbacks;
		unordered_map<string, vec3> colorfallbacks;
		if(object->Has(v8str("fallbacks")))
		{
			Handle<Object> obj = object->Get(v8str("fallbacks"))->ToObject();

			Handle<Array> targets = obj->GetPropertyNames();
			for(unsigned int i = 0; i < targets->Length(); ++i)
			{
				string target = stdstr(targets->Get(i));
				Handle<Value> fallback = obj->Get(v8str(target));
				if(fallback->IsString())
					copyfallbacks.insert(make_pair(target, stdstr(fallback)));
				else if(fallback->IsArray())
				{
					Handle<Array> array = Handle<Array>::Cast(fallback);
					if(array->Length() == 3)
						colorfallbacks.insert(make_pair(target, vec3((float)array->Get(0)->NumberValue(), (float)array->Get(1)->NumberValue(), (float)array->Get(2)->NumberValue())));
				}
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

		// enable depth test and writing
		bool depth = false;
		if(object->Has(v8str("depth")))
			depth = object->Get(v8str("depth"))->BooleanValue();

		// clear color targets before rendering
		bool clear = false;
		vec3 clearcolor = vec3(0);
		if(object->Has(v8str("clear"))) {
			clear = true;
			Handle<Value> value = object->Get(v8str("clear"));
			if(value->IsArray())
			{
				Handle<Array> array = Handle<Array>::Cast(value);
				if(array->Length() == 3)
					clearcolor = vec3((float)array->Get(0)->NumberValue(), (float)array->Get(1)->NumberValue(), (float)array->Get(2)->NumberValue());
			}
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
				else ManagerLog::Warning("script", "pass (" + name + ") unknown stencil function");
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
				else ManagerLog::Warning("script", "pass (" + name + ") unknown stencil operation");
			}
		}

		// create pass
		module->PassCreate(name, vertex, fragment, targets, samplers, copyfallbacks, colorfallbacks, function, depth, clear, clearcolor, size, stencilfunc, stencilref, stencilop);
	}
	// toggle existing pass
	else
	{
		if(Pass *pass = module->PassGet(name))
		{
			pass->Enabled = !pass->Enabled;
			ManagerLog::Print("script", string(pass->Enabled ? "enabled" : "disabled") + " (" + name + ") pass");
		}
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
 */
Handle<Value> ModuleRenderer::jsUniform(const Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString()) { module->Log->Warning("script", "wrong parameter"); return Undefined(); }
	string passname = *String::Utf8Value(args[0]);

	if(args.Length() < 2 || !args[1]->IsString()) { module->Log->Warning("script", "wrong parameter"); return Undefined(); }
	string name = *String::Utf8Value(args[1]);

	if(args.Length() < 3) { module->Log->Warning("script", "wrong parameter"); return Undefined(); }

	// get pass
	Pass *pass = module->PassGet(passname);
	if(!pass) { module->Log->Warning("script", "pass (" + passname + ") not found"); return Undefined(); }

	// get location
	glUseProgram(pass->Program);
	GLuint location = glGetUniformLocation(pass->Program, name.c_str());

	// get type
	struct Uniform {char Name[128]; GLsizei Length; GLint Size; GLenum Type; } uniform;
	bool match = false;
	int count = 0;
	glGetProgramiv(pass->Program, GL_ACTIVE_UNIFORMS, &count);
	for(int i = 0; i < count && !match; ++i)
	{
		glGetActiveUniform(pass->Program, GLuint(i), sizeof(uniform.Name), &uniform.Length, &uniform.Size, &uniform.Type, uniform.Name);
		if(uniform.Length > sizeof(uniform.Name))
			module->Log->Fail("script", "uniform name longer than " + to_string(sizeof(uniform.Name)) + " characters");

		if(uniform.Name == name) match = true;
	}
	if(!match) { module->Log->Fail("script", "uniform (" + name + ") not found in pass (" + passname + ")"); return Undefined(); }

	// get value type
	bool isint   = false;
	bool isfloat = false;
	bool isbool  = false;
	switch(uniform.Type)
	{
	case GL_FLOAT:
	case GL_FLOAT_VEC2:
	case GL_FLOAT_VEC3:
	case GL_FLOAT_VEC4:
	case GL_FLOAT_MAT2:
	case GL_FLOAT_MAT3:
	case GL_FLOAT_MAT4:
		isfloat = true;
		break;
	case GL_INT:
	case GL_INT_VEC2:
	case GL_INT_VEC3:
	case GL_INT_VEC4:
		isint = true;
		break;
	case GL_BOOL:
	case GL_BOOL_VEC2:
	case GL_BOOL_VEC3:
	case GL_BOOL_VEC4:
		isbool = true;
		break;
	}

	// read values
	vector<int>   ints;
	vector<float> floats;
	vector<int>   bools;
	if(args[2]->IsArray())
	{
		Handle<Array> array = Handle<Array>::Cast(args[2]);
		if(isint)
			for(unsigned int i = 0; i < array->Length(); ++i)
				if(array->Get(i)->IsInt32())
					ints.push_back(array->Get(i)->Int32Value());
		if(isfloat)
			for(unsigned int i = 0; i < array->Length(); ++i)
				if(array->Get(i)->IsNumber())
					floats.push_back((float)array->Get(i)->NumberValue());
		if(isbool)
			for(unsigned int i = 0; i < array->Length(); ++i)
				if(array->Get(i)->IsBoolean())
					bools.push_back(array->Get(i)->BooleanValue() ? 1 : 0);
	}
	else if(isint && args[2]->IsInt32())
		ints.push_back(args[2]->Int32Value());
	else if(isfloat && args[2]->IsNumber())
		floats.push_back((float)args[2]->NumberValue());
	else if(isbool && args[2]->IsBoolean())
		bools.push_back(args[2]->BooleanValue() ? 1 : 0);
	else
		module->Log->Warning("script", "invalid uniform value");

	// set uniform value
	switch(uniform.Type)
	{
	case GL_FLOAT:
		if(floats.size() != 1) module->Log->Warning("script", "uniform value length mismatch");
		glUniform1fv(location, 1, &floats[0]);
		break;
	case GL_FLOAT_VEC2:
		if(floats.size() != 2) module->Log->Warning("script", "uniform value length mismatch");
		glUniform2fv(location, 1, &floats[0]);
		break;
	case GL_FLOAT_VEC3:
		if(floats.size() != 3) module->Log->Warning("script", "uniform value length mismatch");
		glUniform3fv(location, 1, &floats[0]);
		break;
	case GL_FLOAT_VEC4:
		if(floats.size() != 4) module->Log->Warning("script", "uniform value length mismatch");
		glUniform4fv(location, 1, &floats[0]);
		break;
	case GL_INT:
		if(ints.size() != 1) module->Log->Warning("script", "uniform value length mismatch");
		glUniform1iv(location, 1, &ints[0]);
		break;
	case GL_INT_VEC2:
		if(ints.size() != 2) module->Log->Warning("script", "uniform value length mismatch");
		glUniform2iv(location, 1, &ints[0]);
		break;
	case GL_INT_VEC3:
		if(ints.size() != 3) module->Log->Warning("script", "uniform value length mismatch");
		glUniform3iv(location, 1, &ints[0]);
		break;
	case GL_INT_VEC4:
		if(ints.size() != 4) module->Log->Warning("script", "uniform value length mismatch");
		glUniform4iv(location, 1, &ints[0]);
		break;
	case GL_BOOL:
		if(bools.size() != 1) module->Log->Warning("script", "uniform value length mismatch");
		glUniform1iv(location, 1, &bools[0]);
		break;
	case GL_BOOL_VEC2:
		if(bools.size() != 2) module->Log->Warning("script", "uniform value length mismatch");
		glUniform2iv(location, 1, &bools[0]);
		break;
	case GL_BOOL_VEC3:
		if(bools.size() != 3) module->Log->Warning("script", "uniform value length mismatch");
		glUniform3iv(location, 1, &bools[0]);
		break;
	case GL_BOOL_VEC4:
		if(bools.size() != 4) module->Log->Warning("script", "uniform value length mismatch");
		glUniform4iv(location, 1, &bools[0]);
		break;
	case GL_FLOAT_MAT2:
		if(floats.size() != 4) module->Log->Warning("script", "uniform value length mismatch");
		glUniformMatrix2fv(location, 1, GL_FALSE, &floats[0]);
		break;
	case GL_FLOAT_MAT3:
		if(floats.size() != 9) module->Log->Warning("script", "uniform value length mismatch");
		glUniformMatrix3fv(location, 1, GL_FALSE, &floats[0]);
		break;
	case GL_FLOAT_MAT4:
		if(floats.size() != 16) module->Log->Warning("script", "uniform value length mismatch");
		glUniformMatrix4fv(location, 1, GL_FALSE, &floats[0]);
		break;
	case GL_SAMPLER_2D:
		module->Log->Warning("script", "setting sampler uniforms not supported");
		break;
	case GL_SAMPLER_CUBE:
		module->Log->Warning("script", "setting sampler uniforms not supported");
		break;
	default:
		module->Log->Warning("script", "unknown sampler type");
		break;
	}

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

	stg->Set<bool>("Wireframe", !stg->Is("Wireframe"));

	ManagerLog::Print("script", string(stg->Is("Wireframe") ? "enabled" : "disabled") + " wireframe mode");
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
