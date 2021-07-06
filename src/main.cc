#define NAPI_CPP_EXCEPTIONS
#include "matchset_impl.h"

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    MatchSetImpl::Initialize(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Initialize);
