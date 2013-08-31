#include "module.h"

#include <string>
#include <sfml/System.hpp>
using namespace std;
using namespace sf;

#include "settings.h"


void ModuleSettings::Callbacks()
{
	Script->Bind("settings", jsSettings);
}

v8::Handle<v8::Value> ModuleSettings::jsSettings(const v8::Arguments& args)
{
	ModuleSettings *module = (ModuleSettings*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsObject())
		return v8::Undefined();
	v8::Handle<v8::Object> object = args[0]->ToObject();

	auto stg = module->Global->Get<Settings>("settings");

	v8::Handle<v8::Array> keys = object->GetPropertyNames();
	for(unsigned int i = 0; i < keys->Length(); ++i)
	{
		string key = *v8::String::Utf8Value(keys->Get(i));
		if(!object->Get(v8::String::New(key.c_str()))->IsArray())
		{
			HelperDebug::Fail("script", "could not parse (" + key + ") setting");
			continue;
		}
		v8::Handle<v8::Array> values = v8::Handle<v8::Array>::Cast(object->Get(v8::String::New(key.c_str())));
	
		if(!values->Has(0) || !values->Get(0)->IsString())
		{
			HelperDebug::Fail("script", "could not parse (" + key + ") setting");
			continue;
		}
		string type = *v8::String::Utf8Value(values->Get(0));

		if(type == "bool")
		{
			if(!values->Has(1) || !values->Get(1)->IsBoolean())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<bool>(key, values->Get(1)->BooleanValue());
		}
		else if(type == "int")
		{
			if(!values->Has(1) || !values->Get(1)->IsInt32())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<int>(key, values->Get(1)->Int32Value());
		}
		else if(type == "float")
		{
			if(!values->Has(1) || !values->Get(1)->IsNumber())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<float>(key, (float)values->Get(1)->NumberValue());
		}
		else if(type == "string")
		{
			if(!values->Has(1) || !values->Get(1)->IsString())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<string>(key, *v8::String::Utf8Value(values->Get(1)));
		}
		else if(type == "Vector2i")
		{
			if(!values->Has(1) || !values->Has(2) || !values->Get(1)->IsInt32(), !values->Get(2)->IsInt32())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<Vector2i>(key, Vector2i(values->Get(1)->Int32Value(), values->Get(2)->Int32Value()));
		}
		else if(type == "Vector2u")
		{
			if(!values->Has(1) || !values->Has(2) || !values->Get(1)->IsUint32(), !values->Get(2)->IsUint32())
			{
				HelperDebug::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<Vector2u>(key, Vector2u(values->Get(1)->Uint32Value(), values->Get(2)->Uint32Value()));
		}
	}

	return v8::Undefined();
}
