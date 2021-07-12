#ifndef __WRAPPER_H__
#define __WRAPPER_H__
#include <napi.h>
#include <vector>
#include <string>
#include "matchset.h"

class MatchSetImpl : public Napi::ObjectWrap<MatchSetImpl>
{
public:
    explicit MatchSetImpl(const Napi::CallbackInfo &info);
    ~MatchSetImpl()
    {
        delete matchSet_;
        delete placeholder_;
        delete pcmp_;
    }
    static Napi::Object Initialize(Napi::Env env, Napi::Object exports);
    Napi::Value Match(const Napi::CallbackInfo &info);
    Napi::Value Cancel(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    MatchSet *matchSet_;
    Player *placeholder_;
    PlayerComparator *pcmp_;
};

MatchSetImpl::MatchSetImpl(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<MatchSetImpl>(info)
{
    std::vector<std::string> attrs;
    std::vector<std::string> bans;
    placeholder_ = new Player(0, 0, std::move(attrs), std::move(bans));
    pcmp_ = new PlayerComparator();
    matchSet_ = new MatchSet(*pcmp_, *placeholder_);
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
    if (info.Length() != 1 || !info[0].IsString())
    {
        NAPI_THROW(Napi::TypeError::New(env, "string param expected"));
    }
    std::uint64_t uid = static_cast<uint64_t>(
        std::stoull(info[0].As<Napi::String>().Utf8Value()));
    return Napi::Boolean::New(env, matchSet_->Cancel(uid));
}

Napi::Value MatchSetImpl::Match(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 5 || !info[0].IsString() || !info[1].IsString() ||
        !info[2].IsString() || !info[3].IsArray() || !info[4].IsArray())
    {
        NAPI_THROW(Napi::TypeError::New(env,
                                        "MatchSet::Match Parameters Invalid"));
    }
    std::uint64_t uid = static_cast<uint64_t>(
        std::stoull(info[0].As<Napi::String>().Utf8Value()));
    std::uint64_t score = static_cast<uint64_t>(
        std::stoull(info[1].As<Napi::String>().Utf8Value()));
    std::uint64_t limit = static_cast<uint64_t>(
        std::stoull(info[2].As<Napi::String>().Utf8Value()));
    Napi::Array attrsNapi = info[3].As<Napi::Array>();
    std::vector<std::string> attrs;
    for (std::uint32_t i = 0; i < attrsNapi.Length(); i++)
    {
        attrs.push_back(attrsNapi.Get(i).As<Napi::String>().Utf8Value());
    }
    Napi::Array bansNapi = info[4].As<Napi::Array>();
    std::vector<std::string> bans;
    for (std::uint32_t i = 0; i < bansNapi.Length(); i++)
    {
        bans.push_back(bansNapi.Get(i).As<Napi::String>().Utf8Value());
    }
    auto matched = matchSet_->Match(uid, score, limit,
                                    std::move(attrs), std::move(bans));

    auto ret = Napi::Object::New(env);
    ret.Set<Napi::String>(std::string("self"),
                          Napi::String::New(env, std::to_string(matched[0])));
    ret.Set<Napi::String>(std::string("peer"),
                          Napi::String::New(env, std::to_string(matched[1])));
    return ret;
}

#endif