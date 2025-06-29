// Copyright (c) 2025 Ctrl Alt Delete Games. All rights reserved.

#pragma once

#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME

// Field expansion
#define LOOM_EXPAND_FIELD_IMPL(type, name) type name;
#define LOOM_EXPAND_FIELDS_1(x) LOOM_EXPAND_FIELD_IMPL x
#define LOOM_EXPAND_FIELDS_2(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_1(__VA_ARGS__)
#define LOOM_EXPAND_FIELDS_3(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_2(__VA_ARGS__)
#define LOOM_EXPAND_FIELDS_4(x, ...) LOOM_EXPAND_FIELD_IMPL x LOOM_EXPAND_FIELDS_3(__VA_ARGS__)
#define LOOM_EXPAND_FIELDS(...) GET_MACRO(__VA_ARGS__, LOOM_EXPAND_FIELDS_4, LOOM_EXPAND_FIELDS_3, LOOM_EXPAND_FIELDS_2, LOOM_EXPAND_FIELDS_1)(__VA_ARGS__)

// Field names for reflection
#define LOOM_FIELD_NAME_IMPL(type, name) #name
#define LOOM_FIELD_NAMES_1(x) LOOM_FIELD_NAME_IMPL x
#define LOOM_FIELD_NAMES_2(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_1(__VA_ARGS__)
#define LOOM_FIELD_NAMES_3(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_2(__VA_ARGS__)
#define LOOM_FIELD_NAMES_4(x, ...) LOOM_FIELD_NAME_IMPL x, LOOM_FIELD_NAMES_3(__VA_ARGS__)
#define LOOM_FIELD_NAMES(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_NAMES_4, LOOM_FIELD_NAMES_3, LOOM_FIELD_NAMES_2, LOOM_FIELD_NAMES_1)(__VA_ARGS__)

// Constructor arguments
#define LOOM_CTOR_ARG_IMPL(type, name) type name##_
#define LOOM_EVENT_CTOR_ARGS_1(x) LOOM_CTOR_ARG_IMPL x
#define LOOM_EVENT_CTOR_ARGS_2(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_1(__VA_ARGS__)
#define LOOM_EVENT_CTOR_ARGS_3(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_2(__VA_ARGS__)
#define LOOM_EVENT_CTOR_ARGS_4(x, ...) LOOM_CTOR_ARG_IMPL x, LOOM_EVENT_CTOR_ARGS_3(__VA_ARGS__)
#define LOOM_EVENT_CTOR_ARGS(...) GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_ARGS_4, LOOM_EVENT_CTOR_ARGS_3, LOOM_EVENT_CTOR_ARGS_2, LOOM_EVENT_CTOR_ARGS_1)(__VA_ARGS__)

// Constructor initializer list
#define LOOM_CTOR_INIT_IMPL(type, name) name(name##_)
#define LOOM_EVENT_CTOR_LIST_1(x) LOOM_CTOR_INIT_IMPL x
#define LOOM_EVENT_CTOR_LIST_2(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_1(__VA_ARGS__)
#define LOOM_EVENT_CTOR_LIST_3(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_2(__VA_ARGS__)
#define LOOM_EVENT_CTOR_LIST_4(x, ...) LOOM_CTOR_INIT_IMPL x, LOOM_EVENT_CTOR_LIST_3(__VA_ARGS__)
#define LOOM_EVENT_CTOR_LIST(...) GET_MACRO(__VA_ARGS__, LOOM_EVENT_CTOR_LIST_4, LOOM_EVENT_CTOR_LIST_3, LOOM_EVENT_CTOR_LIST_2, LOOM_EVENT_CTOR_LIST_1)(__VA_ARGS__)

// Field pointer lookup
#define LOOM_FIELD_POINTER_IMPL(type, name) if (field == #name) return &name;
#define LOOM_FIELD_POINTERS_1(x) LOOM_FIELD_POINTER_IMPL x
#define LOOM_FIELD_POINTERS_2(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_1(__VA_ARGS__)
#define LOOM_FIELD_POINTERS_3(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_2(__VA_ARGS__)
#define LOOM_FIELD_POINTERS_4(x, ...) LOOM_FIELD_POINTER_IMPL x LOOM_FIELD_POINTERS_3(__VA_ARGS__)
#define LOOM_FIELD_POINTERS(...) GET_MACRO(__VA_ARGS__, LOOM_FIELD_POINTERS_4, LOOM_FIELD_POINTERS_3, LOOM_FIELD_POINTERS_2, LOOM_FIELD_POINTERS_1)(__VA_ARGS__)

// Complete macro
#define LOOM_EVENT(name, ...)                                               \
    struct name : IEvent {                                                  \
        LOOM_EXPAND_FIELDS(__VA_ARGS__)                                     \
        name() = default;                                                   \
        name(LOOM_EVENT_CTOR_ARGS(__VA_ARGS__))                             \
            : LOOM_EVENT_CTOR_LIST(__VA_ARGS__) {}                          \
        EventID GetEventTypeID() const override {                           \
            return EventType<name>::ID();                                   \
        }                                                                   \
        const char* GetEventName() const override { return #name; }         \
        std::vector<std::string> GetFieldNames() const override {           \
            return { LOOM_FIELD_NAMES(__VA_ARGS__) };                       \
        }                                                                   \
        void* GetFieldPointer(const std::string& field) override {          \
            LOOM_FIELD_POINTERS(__VA_ARGS__)                                \
            return nullptr;                                                 \
        }                                                                   \
    };