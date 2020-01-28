// Copyright 2017 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <string>
#include "include/v8.h"
#include "src/heap/factory.h"
#include "src/objects-inl.h"
#include "src/regexp/jsregexp.h"
#include "test/fuzzer/fuzzer-support.h"

namespace v8 {
namespace internal {
namespace {

constexpr bool kVerbose = false;  // For debugging, verbose error messages.

int count = 0;

struct FuzzerArgs {
  FuzzerArgs(const uint8_t* input_data, size_t input_length,
             v8::Local<v8::Context> context, Isolate* isolate)
      : input_cursor(0),
        input_data(input_data),
        input_length(input_length),
        context(context),
        isolate(isolate) {}

  size_t input_cursor;
  const uint8_t* const input_data;
  const size_t input_length;
  v8::Local<v8::Context> context;
  Isolate* const isolate;
};


v8::Local<v8::String> v8_str(const char* s) {
  return v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), s,
                                 v8::NewStringType::kNormal)
      .ToLocalChecked();
}

v8::MaybeLocal<v8::Value> CompileRun(v8::Local<v8::Context> context,
                                     const char* source) {
  v8::Local<v8::Script> script;
  v8::MaybeLocal<v8::Script> maybe_script =
      v8::Script::Compile(context, v8_str(source));



  if (!maybe_script.ToLocal(&script)) return v8::MaybeLocal<v8::Value>();
  return script->Run(context);
}

void PrintExceptionMessage(v8::Isolate* isolate, v8::TryCatch* try_catch) {
  CHECK(try_catch->HasCaught());
  static const int kBufferLength = 256;
  char buffer[kBufferLength + 1];
  try_catch->Message()->Get()->WriteOneByte(
      isolate, reinterpret_cast<uint8_t*>(&buffer[0]), 0, kBufferLength);
  fprintf(stderr, "%s\n", buffer);
}

void CompileRunAndVerify(FuzzerArgs* args, const std::string& source) {
  v8::Local<v8::Value> result;
  v8::Isolate* isolate = reinterpret_cast<v8::Isolate*>(args->isolate);
  v8::TryCatch try_catch(isolate);
  if (!CompileRun(args->context, source.c_str()).ToLocal(&result)) {
    args->isolate->clear_pending_exception();
    // No need to verify result if an exception was thrown here, since that
    // implies a syntax error somewhere in the pattern or string. We simply
    // ignore those.
    if (kVerbose) {
      PrintExceptionMessage(isolate, &try_catch);
      fprintf(stderr, "Failed to run script:\n```\n%s\n```\n", source.c_str());
    }
    return;
  }

  // if (!ResultsAreIdentical(args)) {
  //   uint32_t hash = StringHasher::HashSequentialString(
  //       args->input_data, static_cast<int>(args->input_length),
  //       kRegExpBuiltinsFuzzerHashSeed);
  //   V8_Fatal(__FILE__, __LINE__,
  //            "!ResultAreIdentical(args); RegExpBuiltinsFuzzerHash=%x", hash);
  // }
}




extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  // Flag definitions.
  FLAG_allow_natives_syntax = true;
  // V8 setup.
  v8_fuzzer::FuzzerSupport* support = v8_fuzzer::FuzzerSupport::Get();

  v8::Isolate* isolate = support->GetIsolate();

  Isolate* i_isolate = reinterpret_cast<i::Isolate*>(isolate);
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = support->GetContext();
  v8::Context::Scope context_scope(context);
  v8::TryCatch try_catch(isolate);

 CHECK(!i_isolate->has_pending_exception());
 
  std::string source_string =
      std::string(reinterpret_cast<const char*>(data), size);
 //printf("Payload: %s\n",source_string.c_str());
  FuzzerArgs args(data, size, context, i_isolate);
 CompileRunAndVerify(&args, source_string);
  CHECK(!i_isolate->has_pending_exception());



    v8::Isolate* iso = v8::Isolate::GetCurrent();
    if (count > 1000) {
      printf("CLEANING\n");
  iso->LowMemoryNotification();
  count = 0;
  }
  count = count +1;

  return 0;
}

#ifdef CUSTOM_MUTATOR

#include "python_bridge.cc"

#endif // CUSTOM_MUTATOR

}// namespace internal
}  // namespace v8
}
