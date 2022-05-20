#include "Engone/ScriptingModule.h"

#include "Engone/Logger.h"
#include "Engone/AssetModule.h"

//#define ENGONE_V8

#ifdef ENGONE_V8
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"v8_monolith.lib")
#pragma comment(lib,"Dbghelp.lib")

#include <libplatform/libplatform.h>
#include <v8-initialization.h>
#include <v8.h>
#endif

namespace engone {

	static bool isInitialized = false;
#ifdef ENGONE_V8
	static v8::Isolate* isolate = nullptr;
	static v8::ArrayBuffer::Allocator* array_buffer_allocator = nullptr;
	static v8::Global<v8::Context> global_context;
	static std::unique_ptr<v8::Platform> platform;

	void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
		if (args.Length() < 1) return;
		
		v8::HandleScope handleScope(isolate);
		v8::String::Utf8Value str(isolate, args[0]);
		log::out << *str;
	}
	void ScriptAPI(v8::Local<v8::ObjectTemplate>& global) {
		global->Set(v8::String::NewFromUtf8Literal(isolate, "ENGONE_LOG"), v8::FunctionTemplate::New(isolate, LogCallback));
	}
#endif
	void InitScripts() {
		if (isInitialized) return;
#ifdef ENGONE_V8
		platform = v8::platform::NewDefaultPlatform();
		v8::V8::InitializePlatform(platform.get());
		v8::V8::Initialize();

		v8::Isolate::CreateParams create_params;
		create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
		array_buffer_allocator = create_params.array_buffer_allocator;
		isolate = v8::Isolate::New(create_params);

		v8::Isolate::Scope isolate_scope(isolate);
		v8::HandleScope handle_scope(isolate);

		v8::Local<v8::ObjectTemplate> globalObject = v8::ObjectTemplate::New(isolate);
		ScriptAPI(globalObject);

		v8::Local<v8::Context> context = v8::Context::New(isolate,NULL,globalObject);
		global_context = v8::Global<v8::Context>(isolate, context);
#endif
		// return if something failed

		isInitialized = true;
	}
	void CleanupScripts() {
		if (!isInitialized) return;
#ifdef ENGONE_V8
		global_context.Reset();
		isolate->Dispose();
		v8::V8::Dispose();
		v8::V8::DisposePlatform();
		delete array_buffer_allocator;
		platform.release();
#endif
		isInitialized = false;
	}
#define V8CHECK(handle,path) if(handle.IsEmpty()) { ExceptionMessage(trycatch,path); return;};
#ifdef ENGONE_V8
	const char* toString(v8::Local<v8::Value> val) {
		if (val.IsEmpty()) return "";
		return *v8::String::Utf8Value(v8::Isolate::GetCurrent(),val);
	}
	std::string toString(v8::Maybe<int32_t> val) {
		return std::to_string(val.ToChecked());
	}
	void ExceptionMessage(v8::TryCatch& trycatch,const std::string& path) {
		v8::Local<v8::Value> exception = trycatch.Exception();
		v8::Local<v8::Message> msg = trycatch.Message();
		//v8::String::Utf8Value str(isolate, exception);
		//log::out << "Exception " << toString(exception) << "\n";
		//log::out << "Name: " << path << "\n";
		//log::out << "Line: " << toString(msg->GetLineNumber(isolate->GetCurrentContext())) << "\n";
		//log::out << "trace: "<<toString(msg->GetStackTrace()->CurrentScriptNameOrSourceURL(isolate)) << "\n";

		v8::MaybeLocal<v8::Value> hm = trycatch.StackTrace(isolate->GetCurrentContext());
		auto loc = hm.ToLocalChecked();

		log::out << toString(loc) << "\n";
	}
#endif
	/*
		SCRIPT
	*/
	Script::Script(const std::string& path) : m_path(path) {
		ReadScript(path, this);
	}
	void Script::reload(const std::string& path) {
		if (!path.empty()) {
			m_path = path;
		}
		m_source.clear();
		ReadScript(m_path, this);
	}
	void Script::run() {
		if (m_source.empty()) return;
		if (!isInitialized){
			InitScripts();
			if (!isInitialized) { // double check that the initialization worked
				log::out << log::RED << "Scripts failed initialization\n";
				return;
			}
		}
#ifdef ENGONE_V8
		//// Run some code
		v8::Isolate::Scope isolateScope(isolate);
		v8::HandleScope handleScope(isolate);
		v8::Local<v8::Context> context = v8::Local<v8::Context>::New(isolate,global_context);
		v8::Context::Scope context_scope(context);

		v8::Local<v8::String> source = v8::String::NewFromUtf8(isolate, m_source.c_str()).ToLocalChecked();
		v8::TryCatch trycatch(isolate);

		v8::MaybeLocal<v8::Script> script = v8::Script::Compile(context, source);
		V8CHECK(script,m_path);
		v8::MaybeLocal<v8::Value> result = script.ToLocalChecked()->Run(context);
		V8CHECK(result,m_path)

		v8::String::Utf8Value utf8(isolate, result.ToLocalChecked());
		log::out << *utf8 << "\n";
#endif
	}
	void Script::ReadScript(const std::string& path, Script* script) {
		FileReader file(path, false);

		// file not found
		if (!file) {
			log::out << log::RED << path << " not found\n";
			return;
		}

		if (file.fileSize > 0u)
			file.readAll(&script->m_source);
	}
	// operator overloading stuff
	void Comp() {
		using namespace engone;
		FileReader file("script.js", false);
		std::string source;
		file.readAll(&source);
		file.close();

		// simple replace
		std::string result;

		// find blueprints, classes, functions objects with ["+"]

		int depth = 0;
		int numSwaps = -1;
		std::vector<int> swaps;
		for (size_t i = 0; i < source.length(); ++i) {
			char chr = source[i];
			if (chr == '(') {
				++depth;
			}
			//bool skip=false;
			//if (swaps.size() > 0) {
			//	skip = swaps.back() == depth;
			//}
			//if (chr == '+' && !skip) {
			//	result += "['+'](";
			//	swaps.push_back(depth);
			//	continue;
			//}
			if (swaps.size() > 0) {
				if (chr == ')' || chr == ';' || chr == '+') {
					if (swaps.back() == depth) {
						result += ")";
						swaps.pop_back();
					}
				}
			}
			if (chr == '+') {
				result += "['+'](";
				swaps.push_back(depth);
				continue;
			}
			if (chr == ')') {
				--depth;
			}
			result += chr;
		}
		log::out << result;
	}
}