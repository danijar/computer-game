#include "module.h"

#include <string>
using namespace std;

#include "settings.h"


void ModuleRenderer::Callbacks()
{
	Script->Bind("pass",      jsPass     );
	Script->Bind("wireframe", jsWireframe);
}

v8::Handle<v8::Value> ModuleRenderer::jsPass(const v8::Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsString())
		return v8::Undefined();
	string name = *v8::String::Utf8Value(args[0]);

	// helpers
	auto v8str = [](string String){ return v8::String::New(String.c_str()); };
	auto stdstr = [](v8::Handle<v8::Value> Value){ return string(*v8::String::Utf8Value(Value)); };

	// create new pass
	if(1 < args.Length() && args[1]->IsObject())
	{

		v8::Handle<v8::Object> object = args[1]->ToObject();

		// vertex path
		string vertex;
		if(object->Has(v8str("vertex")))
			vertex = stdstr(object->Get(v8str("vertex")));
		else vertex = "quad.vert";

		// fragment path
		string fragment;
		if(object->Has(v8str("fragment")))
			fragment = stdstr(object->Get(v8str("fragment")));
		else { HelperDebug::Fail("script", "fragment shader path needed"); return v8::Undefined(); }

		// tagets as attachment to texture name
		unordered_map<GLenum, string> targets;

		if(object->Has(v8str("targets")))
		{
			v8::Handle<v8::Object> obj = object->Get(v8str("targets"))->ToObject();

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
		else
		{
			HelperDebug::Fail("script", "at least one target needed");
			return v8::Undefined();
		}

		// samplers as shader location to texture name
		unordered_map<string, string> samplers;
		if(object->Has(v8str("samplers")))
		{
			v8::Handle<v8::Object> obj = object->Get(v8str("samplers"))->ToObject();

			v8::Handle<v8::Array> locations = obj->GetPropertyNames();
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
			v8::Handle<v8::Object> obj = object->Get(v8str("samplers"))->ToObject();

			v8::Handle<v8::Array> targets = obj->GetPropertyNames();
			for(unsigned int i = 0; i < targets->Length(); ++i)
			{
				string target = stdstr(targets->Get(i));
				v8::Handle<v8::Value> fallback = obj->Get(v8str(target));
				if(fallback->IsString())
					fallbacks.insert(make_pair(target, stdstr(fallback)));
				else if(fallback->IsNumber())
					; // how to store clear value?
			}
		}

		// choose draw function
		Function function = QUAD;
		if(object->Has(v8str("function")))
		{
			string func = stdstr(object->Get(v8str("function")));
			if     (func == "FORMS" ) function = FORMS;
			else if(func == "LIGHTS") function = LIGHTS;
			else if(func == "QUAD"  ) function = QUAD;
			else if(func == "SCREEN") function = SCREEN;
			else HelperDebug::Warning("script", "function is invalid");
		}

		// resolution relative to window size
		float size = 1.0f;
		if(object->Has(v8str("size")) && object->Get(v8str("stencil"))->IsNumber())
			size = (float)object->Get(v8str("stencil"))->NumberValue();

		// stencil parameters
		GLenum stencilfunc = GL_ALWAYS;
		GLint  stencilref  = 0;
		GLenum stencilop   = GL_KEEP;
		if(object->Has(v8str("stencil")))
		{
			v8::Handle<v8::Object> obj = object->Get(v8str("stencil"))->ToObject();

			if(0 < obj->GetPropertyNames()->Length() && obj->Get(0)->IsString())
			{
				string func = stdstr(obj->Get(0));
				if     (func == "ALWAYS" ) stencilfunc = GL_ALWAYS;
				else if(func == "EQUAL"  ) stencilfunc = GL_EQUAL;
				else if(func == "GREATER") stencilfunc = GL_GREATER;
				else if(func == "LESS"   ) stencilfunc = GL_LESS;
				else if(func == "GEQUAL" ) stencilfunc = GL_GEQUAL;
				else if(func == "LEQUAL" ) stencilfunc = GL_LEQUAL;
			}
			if(1 < obj->GetPropertyNames()->Length() && obj->Get(1)->IsInt32())
			{
				stencilref = obj->Get(1)->Int32Value();
			}
			if(2 < obj->GetPropertyNames()->Length() && obj->Get(2)->IsString())
			{
				string op = stdstr(obj->Get(2));
				if     (op == "KEEP"   ) stencilop = GL_KEEP;
				else if(op == "REPLACE") stencilop = GL_REPLACE;
				else if(op == "INCR"   ) stencilop = GL_INCR;
			}
		}

		/*
		// dump collected values
		string output = "pass details:";
		output += "vertex path is (" + vertex + ")\n";
		output += "fragment path is (" + fragment + ")\n";
		for(auto i : targets) output += "target " + i.second + " at " + to_string(i.first) + "\n";
		for(auto i : samplers) output += "sampler " + i.second + " at " + i.first + "\n";
		for(auto i : fallbacks) output += "fallback " + i.second + " for " + i.first + "\n";
		output += "stencil function " + string(stencilfunc == GL_ALWAYS ? "always" : (stencilfunc == GL_EQUAL ? "equal" : "other")) + "\n";
		output += "stencil reference " + to_string(stencilref) + "\n";
		output += "stencil operation " + string(stencilfunc == GL_KEEP ? "keep" : (stencilfunc == GL_REPLACE ? "replace" : "other")) + "\n";
		HelperDebug::Print("script", output);
		*/

		// create pass
		module->CreatePass(name, vertex, fragment, targets, samplers, fallbacks, function, size, stencilfunc, stencilref, stencilop);
	}
	// toggle existing pass
	else
	{
		Pass *pass = module->GetPass(name);
		pass->Enabled = !pass->Enabled;
		HelperDebug::Print("script", string(pass->Enabled ? "enabled" : "disabled") + " (" + name + ") pass");
	}
	
	return v8::Undefined();
}

v8::Handle<v8::Value> ModuleRenderer::jsWireframe(const v8::Arguments& args)
{
	ModuleRenderer *module = (ModuleRenderer*)HelperScript::Unwrap(args.Data());
	auto stg = module->Global->Get<Settings>("settings");

	stg->Wireframe = !stg->Wireframe;

	HelperDebug::Print("script", string(stg->Wireframe ? "enabled" : "disabled") + " wireframe mode");
	return v8::Undefined();
}
