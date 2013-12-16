#include "module.h"

#include <string>
#include <dependency/sfml/System.hpp>
using namespace std;
using namespace v8;

#include "type/settings/type.h"


void ModuleSettings::Callbacks()
{
	Script->Bind("settings", jsSettings);
}

Handle<Value> ModuleSettings::jsSettings(const Arguments& args)
{
	ModuleSettings *module = (ModuleSettings*)HelperScript::Unwrap(args.Data());

	if(args.Length() < 1 || !args[0]->IsObject())
		return Undefined();
	Handle<Object> object = args[0]->ToObject();

	auto stg = module->Global->Get<Settings>("settings");

	Handle<Array> keys = object->GetPropertyNames();
	for(unsigned int i = 0; i < keys->Length(); ++i)
	{
		string key = *String::Utf8Value(keys->Get(i));
		if(!object->Get(String::New(key.c_str()))->IsArray())
		{
			ManagerLog::Fail("script", "could not parse (" + key + ") setting");
			continue;
		}
		Handle<Array> values = Handle<Array>::Cast(object->Get(String::New(key.c_str())));
	
		if(!values->Has(0) || !values->Get(0)->IsString())
		{
			ManagerLog::Fail("script", "could not parse (" + key + ") setting");
			continue;
		}
		string type = *String::Utf8Value(values->Get(0));

		if(type == "bool")
		{
			if(!values->Has(1) || !values->Get(1)->IsBoolean())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<bool>(key, values->Get(1)->BooleanValue());
		}
		else if(type == "int")
		{
			if(!values->Has(1) || !values->Get(1)->IsInt32())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<int>(key, values->Get(1)->Int32Value());
		}
		else if(type == "float")
		{
			if(!values->Has(1) || !values->Get(1)->IsNumber())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<float>(key, (float)values->Get(1)->NumberValue());
		}
		else if(type == "string")
		{
			if(!values->Has(1) || !values->Get(1)->IsString())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<string>(key, *String::Utf8Value(values->Get(1)));
		}
		else if(type == "Vector2i")
		{
			if(!values->Has(1) || !values->Has(2) || !values->Get(1)->IsInt32(), !values->Get(2)->IsInt32())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<sf::Vector2i>(key, sf::Vector2i(values->Get(1)->Int32Value(), values->Get(2)->Int32Value()));
		}
		else if(type == "Vector2u")
		{
			if(!values->Has(1) || !values->Has(2) || !values->Get(1)->IsUint32(), !values->Get(2)->IsUint32())
			{
				ManagerLog::Fail("script", "could not parse (" + key + ") setting");
				continue;
			}
			stg->Set<sf::Vector2u>(key, sf::Vector2u(values->Get(1)->Uint32Value(), values->Get(2)->Uint32Value()));
		}
	}

	return Undefined();
}
