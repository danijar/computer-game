#pragma once

#include <string>
#include <unordered_map>
#include <functional>

#include <v8/v8.h>

class Module; // better use void* pointer directly

class HelperScript
{
public:
	HelperScript(std::string Name, Module *Module, v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> Context);
	~HelperScript();
	void Bind(std::string Name, std::function<void(v8::FunctionCallbackInfo<v8::Value> const &)> Function);
	bool Load(std::string Path, bool Module = true);
	v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> Run(std::string Path, bool Module = true);
	v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> Inline(std::string Source);
	static Module *Unwrap(v8::Local<v8::Value> Data);

private:
	std::string name;
	v8::Persistent<v8::External, v8::CopyablePersistentTraits<v8::External>> module; // this is only for binding scripts
	v8::Persistent<v8::Context, v8::CopyablePersistentTraits<v8::Context>> context;
	std::unordered_map<std::string, v8::Persistent<v8::Script, v8::CopyablePersistentTraits<v8::Script>>> scripts;

	v8::Persistent<v8::Script, v8::CopyablePersistentTraits<v8::Script>> Compile(std::string Source);
	v8::Persistent<v8::Value, v8::CopyablePersistentTraits<v8::Value>> Execute(v8::Persistent<v8::Script, v8::CopyablePersistentTraits<v8::Script>> Script, std::string Name = "");
};
