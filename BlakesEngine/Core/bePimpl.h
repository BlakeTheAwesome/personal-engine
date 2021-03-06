#pragma once

#include <new>

// Usage in other files
#define PIMPL_NEW(_class) _class::Create
#define PIMPL_DELETE(x) { if ((x)) { (x)->Dispose(); } (x) = nullptr; }
#define PIMPL_GET_DATA(_this) ((Impl*)(intptr_t(_this)+sizeof(*this)))
#define PIMPL_GET_PARENT(_class, _this) ((_class*)(intptr_t(_this)-sizeof(_class)))


// Usage in header
#define PIMPL_DECLARE(_class, ...)                                                                               \
    private:                                                                                                     \
        _class() : self(*PIMPL_GET_DATA(this)){};                                                                \
        _class(const _class&) = delete;                                                                          \
        _class(_class&&) = delete;                                                                          \
        _class& operator=(const _class&) = delete;                                                               \
        _class& operator=(_class&&) = delete;                                                               \
        struct Impl; Impl& self;                                                                                 \
    public:                                                                                                      \
        static _class* Create(__VA_ARGS__);                                                                      \
        void Dispose();                                                                                          \
// PIMPL_DECLARE


// Usage in CPP
// Defines pimpl data structure
#define PIMPL_DATA(_class, ...)                                                                                  \
struct _class::Impl                                                                                              \
{                                                                                                                \
    Impl(__VA_ARGS__);                                                                                           \
    Impl(const Impl&) = delete;                                                                                  \
    Impl(Impl&&) = delete;                                                                                       \
    Impl& operator=(const Impl&) = delete;                                                                       \
    Impl& operator=(Impl&&) = delete;                                                                            \
    ~Impl();                                                                                                     \
    _class* Parent() { return PIMPL_GET_PARENT(_class, this);}                                                   \
// PIMPL_DATA
#define PIMPL_DATA_END };


// Defines a constructor with no arguments
#define PIMPL_CONSTRUCT(_class)                                                                                  \
    _class* _class::Create()                                                                                     \
    {                                                                                                            \
        _class* cls = new(malloc(sizeof(_class) + sizeof(Impl))) _class();                                       \
        new(&cls->self) Impl();                                                                                  \
        return cls;                                                                                              \
    }                                                                                                            \
    _class::Impl::Impl()                                                                                         \
// PIMPL_CONSTRUCT

// Defines a constructor with arguments, need to use all 3 with VARS as just the variable names not types
#define PIMPL_CONSTRUCT_ARGS(_class, ...) _class* _class::Create(__VA_ARGS__)
#define PIMPL_CONSTRUCT_ARGS_VARS(_class, ...)                                                                   \
    {                                                                                                            \
        _class* cls = new(malloc(sizeof(_class) + sizeof(Impl))) _class();                                       \
        new(&cls->self) Impl(__VA_ARGS__);                                                                       \
        return cls;                                                                                              \
    }                                                                                                            \
// PIMPL_CONSTRUCT_ARGS_VARS
#define PIMPL_CONSTRUCT_ARGS_BODY(_class, ...) _class::Impl::Impl(__VA_ARGS__)


// Defines the Dispose function
#define PIMPL_DESTROY(_class)                                                                                    \
    void _class::Dispose()                                                                                       \
    {                                                                                                            \
        self.~Impl();                                                                                            \
        free(this);                                                                                              \
    }                                                                                                            \
    _class::Impl::~Impl()                                                                                        \
// PIMPL_Dispose
