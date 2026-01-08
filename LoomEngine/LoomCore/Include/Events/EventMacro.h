// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#include "IEvent.h"
#include "EventType.h"
#include <string>
#include <vector>

namespace Loom {

#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME

    // Field expansion
#define LOOM_EXPAND_FIELD_IMPL(type, name) type name;
#define LOOM_EXPAND_FIELDS_1(x) LOOM_EXPAND_FIELD_IMPL x
#define LOOM_EXPAND_FIELDS_2(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_1(__VA_ARGS__)
#define LOOM_EXPAND_FIELDS_3(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_2(__VA_ARGS__)
#define LOOM_EXPAND_FIELDS_4(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_3(__VA_ARGS__)
//#define LOOM_EXPAND_FIELDS(...) GET_MACRO(__VA_ARGS__, LOOM_EXPAND_FIELDS_4, LOOM_EXPAND_FIELDS_3, LOOM_EXPAND_FIELDS_2, LOOM_EXPAND_FIELDS_1)(__VA_ARGS__)

    // Handle empty field case
#define LOOM_EXPAND_FIELDS_0()
#define LOOM_EXPAND_FIELDS(...) \
    LOOM_IF_HAS_ARGS(__VA_ARGS__)(GET_MACRO(__VA_ARGS__, LOOM_EXPAND_FIELDS_4, LOOM_EXPAND_FIELDS_3, LOOM_EXPAND_FIELDS_2, LOOM_EXPAND_FIELDS_1)(__VA_ARGS__), LOOM_EXPAND_FIELDS_0())

    // Field names for reflection
#define LOOM_FIELD_NAME_IMPL(type, name) #name
#define LOOM_FIELD_NAMES_1(x) LOOM_FIELD_NAME_IMPL x
#define LOOM_FIELD_NAMES_2(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_1(__VA_ARGS__)
#define LOOM_FIELD_NAMES_3(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_2(__VA_ARGS__)
#define LOOM_FIELD_NAMES_4(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_3(__VA_ARGS__)
//#define LOOM_FIELD_NAMES(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_NAMES_4, LOOM_FIELD_NAMES_3, LOOM_FIELD_NAMES_2, LOOM_FIELD_NAMES_1)(__VA_ARGS__)

#define LOOM_FIELD_NAMES_0()
#define LOOM_FIELD_NAMES(...) \
    LOOM_IF_HAS_ARGS(__VA_ARGS__)(GET_MACRO(__VA_ARGS__, LOOM_FIELD_NAMES_4, LOOM_FIELD_NAMES_3, LOOM_FIELD_NAMES_2, LOOM_FIELD_NAMES_1)(__VA_ARGS__), LOOM_FIELD_NAMES_0())

    // Constructor arguments
#define LOOM_CTOR_ARG_IMPL(type, name) type name##_
#define LOOM_EVENT_CTOR_ARGS_1(x) LOOM_CTOR_ARG_IMPL x
#define LOOM_EVENT_CTOR_ARGS_2(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_1(__VA_ARGS__)
#define LOOM_EVENT_CTOR_ARGS_3(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_2(__VA_ARGS__)
#define LOOM_EVENT_CTOR_ARGS_4(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_3(__VA_ARGS__)
//#define LOOM_EVENT_CTOR_ARGS(...) GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_ARGS_4, LOOM_EVENT_CTOR_ARGS_3, LOOM_EVENT_CTOR_ARGS_2, LOOM_EVENT_CTOR_ARGS_1)(__VA_ARGS__)

#define LOOM_EVENT_CTOR_ARGS_0()
#define LOOM_EVENT_CTOR_ARGS(...) \
    LOOM_IF_HAS_ARGS(__VA_ARGS__)(GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_ARGS_4, LOOM_EVENT_CTOR_ARGS_3, LOOM_EVENT_CTOR_ARGS_2, LOOM_EVENT_CTOR_ARGS_1)(__VA_ARGS__), LOOM_EVENT_CTOR_ARGS_0())

    // Constructor initializer list
#define LOOM_CTOR_INIT_IMPL(type, name) name(name##_)
#define LOOM_EVENT_CTOR_LIST_1(x) LOOM_CTOR_INIT_IMPL x
#define LOOM_EVENT_CTOR_LIST_2(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_1(__VA_ARGS__)
#define LOOM_EVENT_CTOR_LIST_3(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_2(__VA_ARGS__)
#define LOOM_EVENT_CTOR_LIST_4(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_3(__VA_ARGS__)
//#define LOOM_EVENT_CTOR_LIST(...) GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_LIST_4, LOOM_EVENT_CTOR_LIST_3, LOOM_EVENT_CTOR_LIST_2, LOOM_EVENT_CTOR_LIST_1)(__VA_ARGS__)

#define LOOM_EVENT_CTOR_LIST_0()
#define LOOM_EVENT_CTOR_LIST(...) \
    LOOM_IF_HAS_ARGS(__VA_ARGS__)(GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_LIST_4, LOOM_EVENT_CTOR_LIST_3, LOOM_EVENT_CTOR_LIST_2, LOOM_EVENT_CTOR_LIST_1)(__VA_ARGS__), LOOM_EVENT_CTOR_LIST_0())

    // Field pointer lookup
#define LOOM_FIELD_POINTER_IMPL(type, name) if (field == #name) return &name;
#define LOOM_FIELD_POINTERS_1(x) LOOM_FIELD_POINTER_IMPL x
#define LOOM_FIELD_POINTERS_2(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_1(__VA_ARGS__)
#define LOOM_FIELD_POINTERS_3(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_2(__VA_ARGS__)
#define LOOM_FIELD_POINTERS_4(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_3(__VA_ARGS__)
//#define LOOM_FIELD_POINTERS(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_POINTERS_4, LOOM_FIELD_POINTERS_3, LOOM_FIELD_POINTERS_2, LOOM_FIELD_POINTERS_1)(__VA_ARGS__)

#define LOOM_FIELD_POINTERS_0()
#define LOOM_FIELD_POINTERS(...) \
    LOOM_IF_HAS_ARGS(__VA_ARGS__)(GET_MACRO(__VA_ARGS__, LOOM_FIELD_POINTERS_4, LOOM_FIELD_POINTERS_3, LOOM_FIELD_POINTERS_2, LOOM_FIELD_POINTERS_1)(__VA_ARGS__), LOOM_FIELD_POINTERS_0())

    // Helper to detect if args exist
#define LOOM_ARG16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define LOOM_HAS_COMMA(...) LOOM_ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define LOOM_TRIGGER_PARENTHESIS(...) ,
#define LOOM_IS_EMPTY(...) LOOM_IS_EMPTY_( \
LOOM_HAS_COMMA(__VA_ARGS__), \
LOOM_HAS_COMMA(LOOM_TRIGGER_PARENTHESIS __VA_ARGS__), \
LOOM_HAS_COMMA(__VA_ARGS__ (/*empty*/)), \
LOOM_HAS_COMMA(LOOM_TRIGGER_PARENTHESIS __VA_ARGS__ (/*empty*/)) \
)
#define LOOM_PASTE5(_0, _1, _2, _3, _4) _0 ## _1 ## _2 ## _3 ## _4
#define LOOM_IS_EMPTY_(_0, _1, _2, _3) LOOM_HAS_COMMA(LOOM_PASTE5(LOOM_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define LOOM_IS_EMPTY_CASE_0001 ,
#define LOOM_IF_HAS_ARGS(...) LOOM_IF_HAS_ARGS_(LOOM_IS_EMPTY(__VA_ARGS__))
#define LOOM_IF_HAS_ARGS_(empty) LOOM_PASTE2(LOOM_IF_HAS_ARGS__, empty)
#define LOOM_PASTE2(a, b) a ## b
#define LOOM_IF_HAS_ARGS__1(t, f) f
#define LOOM_IF_HAS_ARGS__0(t, f) t

    // Complete macro - handles both empty and non-empty field lists
#define LOOM_EVENT(name, ...)                                               \
    struct name : IEvent {                                                  \
        LOOM_EXPAND_FIELDS(__VA_ARGS__)                                     \
        name() = default;                                                   \
        LOOM_IF_HAS_ARGS(__VA_ARGS__)(                                      \
            name(LOOM_EVENT_CTOR_ARGS(__VA_ARGS__))                         \
                : LOOM_EVENT_CTOR_LIST(__VA_ARGS__) {},                     \
        )                                                                   \
        EventID GetEventTypeID() const override {                           \
            return EventType<name>::ID();                                   \
        }                                                                   \
        const char* GetEventName() const override { return #name; }         \
        std::vector<std::string> GetFieldNames() const override {           \
            LOOM_IF_HAS_ARGS(__VA_ARGS__)(                                  \
                return { LOOM_FIELD_NAMES(__VA_ARGS__) };,                  \
                return {};                                                  \
            )                                                               \
        }                                                                   \
        void* GetFieldPointer(const std::string& field) override {          \
            LOOM_FIELD_POINTERS(__VA_ARGS__)                                \
            return nullptr;                                                 \
        }                                                                   \
    };
}