#ifndef __WRAPPER_H__
#define __WRAPPER_H__
#include <napi.h>
#include <string>
#include "matchset.h"
using namespace Napi;

class MatchSetImpl : public Napi::ObjectWrap<MatchSetImpl>
{
public:
    explicit MatchSetImpl(const Napi::CallbackInfo &info);
    static Napi::Object Initialize(Napi::Env env, Napi::Object exports);
    Napi::Value Match(const Napi::CallbackInfo &info);
    Napi::Value Cancel(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    MatchSet *matchSet_;
};

MatchSetImpl::MatchSetImpl(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MatchSetImpl>(info)
{
    Player placeholder{0, 0};
    PlayerComparator pcmp;
    matchSet_ = new MatchSet(pcmp, placeholder);
}

Napi::Object
MatchSetImpl::Initialize(Napi::Env env, Napi::Object exports)
{
    Napi::Function constructor = DefineClass(env, "MatchSet",{
        InstanceMethod<&MatchSetImpl::Match>("Match"),
        InstanceMethod<&MatchSetImpl::Cancel>("Cancel")
    });
    exports.Set("MatchSet", constructor);

    return exports;
}

Napi::Value MatchSetImpl::Cancel(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    return Napi::Number::New(env, 0);
}

Napi::Value MatchSetImpl::Match(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 2 || !info[0].IsNumber() || !info[1].IsNumber())
    {
        NAPI_THROW(Napi::TypeError::New(env, "2 number param expected"));
    }

    auto matched = matchSet_->Match(info[0].As<Napi::Number>().Uint32Value(),
                                    info[1].As<Napi::Number>().Uint32Value());

    auto ret = Napi::Object::New(env);
    ret.Set<Napi::Number>(std::string("self"),
                          Napi::Number::New(env, matched[0]));
    ret.Set<Napi::Number>(std::string("peer"),
                          Napi::Number::New(env, matched[1]));
    return ret;
}

#endif