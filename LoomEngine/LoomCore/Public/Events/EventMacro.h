// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "IEvent.h"
#include "EventType.h"
#include <string>
#include <vector>

namespace Loom {

#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME

#define LOOM_FIELD(type, name) type name{};
#define LOOM_FIELD_NAME(type, name) #name
#define LOOM_CTOR_ARG(type, name) type name##_
#define LOOM_CTOR_INIT(type, name) name(name##_)
#define LOOM_FIELD_POINTER(type, name) if (field == #name) return &name;

#define LOOM_EXPAND_FIELD(x) LOOM_FIELD x
#define LOOM_FIELD_NAME_EXPAND(x) LOOM_FIELD_NAME x
#define LOOM_CTOR_ARG_EXPAND(x) LOOM_CTOR_ARG x
#define LOOM_CTOR_INIT_EXPAND(x) LOOM_CTOR_INIT x
#define LOOM_FIELD_POINTER_EXPAND(x) LOOM_FIELD_POINTER x

#define LOOM_EXPAND_FIELDS_1(a) LOOM_EXPAND_FIELD(a)
#define LOOM_EXPAND_FIELDS_2(a,b) LOOM_EXPAND_FIELD(a) LOOM_EXPAND_FIELD(b)
#define LOOM_EXPAND_FIELDS_3(a,b,c) LOOM_EXPAND_FIELD(a) LOOM_EXPAND_FIELD(b) LOOM_EXPAND_FIELD(c)
#define LOOM_EXPAND_FIELDS_4(a,b,c,d) LOOM_EXPAND_FIELD(a) LOOM_EXPAND_FIELD(b) LOOM_EXPAND_FIELD(c) LOOM_EXPAND_FIELD(d)
#define LOOM_EXPAND_FIELDS(...) GET_MACRO(__VA_ARGS__, LOOM_EXPAND_FIELDS_4, LOOM_EXPAND_FIELDS_3, LOOM_EXPAND_FIELDS_2, LOOM_EXPAND_FIELDS_1)(__VA_ARGS__)

#define LOOM_FIELD_NAMES_1(a) LOOM_FIELD_NAME_EXPAND(a)
#define LOOM_FIELD_NAMES_2(a,b) LOOM_FIELD_NAME_EXPAND(a), LOOM_FIELD_NAME_EXPAND(b)
#define LOOM_FIELD_NAMES_3(a,b,c) LOOM_FIELD_NAME_EXPAND(a), LOOM_FIELD_NAME_EXPAND(b), LOOM_FIELD_NAME_EXPAND(c)
#define LOOM_FIELD_NAMES_4(a,b,c,d) LOOM_FIELD_NAME_EXPAND(a), LOOM_FIELD_NAME_EXPAND(b), LOOM_FIELD_NAME_EXPAND(c), LOOM_FIELD_NAME_EXPAND(d)
#define LOOM_FIELD_NAMES(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_NAMES_4, LOOM_FIELD_NAMES_3, LOOM_FIELD_NAMES_2, LOOM_FIELD_NAMES_1)(__VA_ARGS__)

#define LOOM_CTOR_ARGS_1(a) LOOM_CTOR_ARG_EXPAND(a)
#define LOOM_CTOR_ARGS_2(a,b) LOOM_CTOR_ARG_EXPAND(a), LOOM_CTOR_ARG_EXPAND(b)
#define LOOM_CTOR_ARGS_3(a,b,c) LOOM_CTOR_ARG_EXPAND(a), LOOM_CTOR_ARG_EXPAND(b), LOOM_CTOR_ARG_EXPAND(c)
#define LOOM_CTOR_ARGS_4(a,b,c,d) LOOM_CTOR_ARG_EXPAND(a), LOOM_CTOR_ARG_EXPAND(b), LOOM_CTOR_ARG_EXPAND(c), LOOM_CTOR_ARG_EXPAND(d)
#define LOOM_CTOR_ARGS(...) GET_MACRO(__VA_ARGS__, LOOM_CTOR_ARGS_4, LOOM_CTOR_ARGS_3, LOOM_CTOR_ARGS_2, LOOM_CTOR_ARGS_1)(__VA_ARGS__)

#define LOOM_CTOR_INITS_1(a) LOOM_CTOR_INIT_EXPAND(a)
#define LOOM_CTOR_INITS_2(a,b) LOOM_CTOR_INIT_EXPAND(a), LOOM_CTOR_INIT_EXPAND(b)
#define LOOM_CTOR_INITS_3(a,b,c) LOOM_CTOR_INIT_EXPAND(a), LOOM_CTOR_INIT_EXPAND(b), LOOM_CTOR_INIT_EXPAND(c)
#define LOOM_CTOR_INITS_4(a,b,c,d) LOOM_CTOR_INIT_EXPAND(a), LOOM_CTOR_INIT_EXPAND(b), LOOM_CTOR_INIT_EXPAND(c), LOOM_CTOR_INIT_EXPAND(d)
#define LOOM_CTOR_INITS(...) GET_MACRO(__VA_ARGS__, LOOM_CTOR_INITS_4, LOOM_CTOR_INITS_3, LOOM_CTOR_INITS_2, LOOM_CTOR_INITS_1)(__VA_ARGS__)

#define LOOM_FIELD_POINTERS_1(a) LOOM_FIELD_POINTER_EXPAND(a)
#define LOOM_FIELD_POINTERS_2(a,b) LOOM_FIELD_POINTER_EXPAND(a) LOOM_FIELD_POINTER_EXPAND(b)
#define LOOM_FIELD_POINTERS_3(a,b,c) LOOM_FIELD_POINTER_EXPAND(a) LOOM_FIELD_POINTER_EXPAND(b) LOOM_FIELD_POINTER_EXPAND(c)
#define LOOM_FIELD_POINTERS_4(a,b,c,d) LOOM_FIELD_POINTER_EXPAND(a) LOOM_FIELD_POINTER_EXPAND(b) LOOM_FIELD_POINTER_EXPAND(c) LOOM_FIELD_POINTER_EXPAND(d)
#define LOOM_FIELD_POINTERS(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_POINTERS_4, LOOM_FIELD_POINTERS_3, LOOM_FIELD_POINTERS_2, LOOM_FIELD_POINTERS_1)(__VA_ARGS__)

#define LOOM_EVENT(name, ...)                                               \
    struct name : IEvent {                                                  \
        __VA_OPT__(LOOM_EXPAND_FIELDS(__VA_ARGS__))                         \
        name() = default;                                                   \
        __VA_OPT__(name(LOOM_CTOR_ARGS(__VA_ARGS__))                        \
            : LOOM_CTOR_INITS(__VA_ARGS__) {})                              \
        EventID GetEventTypeID() const override {                           \
            return EventType<name>::ID();                                   \
        }                                                                   \
        const char* GetEventName() const override { return #name; }         \
        std::vector<std::string> GetFieldNames() const override {           \
            return { __VA_OPT__(LOOM_FIELD_NAMES(__VA_ARGS__)) };           \
        }                                                                   \
        void* GetFieldPointer(const std::string& field) override {          \
            __VA_OPT__(LOOM_FIELD_POINTERS(__VA_ARGS__))                    \
            return nullptr;                                                 \
        }                                                                   \
    };

}