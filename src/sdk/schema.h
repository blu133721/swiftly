#ifndef _schema_h
#define _schema_h

#include <stdint.h>

#include "../sig/Signatures.h"
#include "interfaces/cschemasystem.h"

struct SchemaKey
{
    int16_t offset;
    bool networked;
};

class Z_CBaseEntity;
void SetStateChanged(Z_CBaseEntity *pEntity, int offset);

constexpr uint32_t val_32_const = 0x811c9dc5;
constexpr uint32_t prime_32_const = 0x1000193;
constexpr uint64_t val_64_const = 0xcbf29ce484222325;
constexpr uint64_t prime_64_const = 0x100000001b3;

inline constexpr uint32_t hash_32_fnv1a_const(const char *const str, const uint32_t value = val_32_const) noexcept
{
    return (str[0] == '\0') ? value : hash_32_fnv1a_const(&str[1], (value ^ uint32_t(str[0])) * prime_32_const);
}

inline constexpr uint64_t hash_64_fnv1a_const(const char *const str, const uint64_t value = val_64_const) noexcept
{
    return (str[0] == '\0') ? value : hash_64_fnv1a_const(&str[1], (value ^ uint64_t(str[0])) * prime_64_const);
}

namespace sch
{
    int16_t FindChainOffset(const char *className);
    SchemaKey GetOffset(const char *className, uint32_t classKey, const char *memberName, uint32_t memberKey);
};

#define SCHEMA_FIELD_OFFSET(type, varName, extra_offset)                                                                                                          \
    class varName##_prop                                                                                                                                          \
    {                                                                                                                                                             \
    public:                                                                                                                                                       \
        std::add_lvalue_reference_t<type> Get()                                                                                                                   \
        {                                                                                                                                                         \
            static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClassName);                                                                            \
            static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);                                                                                      \
                                                                                                                                                                  \
            static const auto m_key =                                                                                                                             \
                sch::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);                                                                               \
                                                                                                                                                                  \
            static const size_t offset = offsetof(ThisClass, varName);                                                                                            \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);                                                                                         \
                                                                                                                                                                  \
            return *reinterpret_cast<std::add_pointer_t<type>>(                                                                                                   \
                (uintptr_t)(pThisClass) + m_key.offset + extra_offset);                                                                                           \
        }                                                                                                                                                         \
        void Set(type val)                                                                                                                                        \
        {                                                                                                                                                         \
            static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClassName);                                                                            \
            static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);                                                                                      \
                                                                                                                                                                  \
            static const auto m_key =                                                                                                                             \
                sch::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);                                                                               \
                                                                                                                                                                  \
            static const auto m_chain =                                                                                                                           \
                sch::FindChainOffset(ThisClassName);                                                                                                              \
                                                                                                                                                                  \
            static const size_t offset = offsetof(ThisClass, varName);                                                                                            \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);                                                                                         \
                                                                                                                                                                  \
            if (m_chain != 0 && m_key.networked)                                                                                                                  \
                g_Signatures->FetchSignature<NetworkSTChange>("NetworkStateChanged")((uintptr_t)(pThisClass) + m_chain, m_key.offset + extra_offset, 0xFFFFFFFF); \
            else if (m_key.networked)                                                                                                                             \
            {                                                                                                                                                     \
                if (!IsStruct)                                                                                                                                    \
                    SetStateChanged((Z_CBaseEntity *)pThisClass, m_key.offset + extra_offset);                                                                    \
                else if (IsPlatformPosix())                                                                                                                       \
                    CALL_VIRTUAL(void, 1, pThisClass, m_key.offset + extra_offset, 0xFFFFFFFF, 0xFFFF);                                                           \
            }                                                                                                                                                     \
            *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)(pThisClass) + m_key.offset + extra_offset) = val;                                             \
        }                                                                                                                                                         \
        operator std::add_lvalue_reference_t<type>() { return Get(); }                                                                                            \
        std::add_lvalue_reference_t<type> operator()() { return Get(); }                                                                                          \
        std::add_lvalue_reference_t<type> operator->() { return Get(); }                                                                                          \
        void operator()(type val) { Set(val); }                                                                                                                   \
        void operator=(type val) { Set(val); }                                                                                                                    \
    } varName;

#define SCHEMA_FIELD_POINTER_OFFSET(type, varName, extra_offset)                       \
    class varName##_prop                                                               \
    {                                                                                  \
    public:                                                                            \
        type *Get()                                                                    \
        {                                                                              \
            static constexpr auto datatable_hash = hash_32_fnv1a_const(ThisClassName); \
            static constexpr auto prop_hash = hash_32_fnv1a_const(#varName);           \
                                                                                       \
            static const auto m_key =                                                  \
                sch::GetOffset(ThisClassName, datatable_hash, #varName, prop_hash);    \
                                                                                       \
            static const size_t offset = offsetof(ThisClass, varName);                 \
            ThisClass *pThisClass = (ThisClass *)((byte *)this - offset);              \
                                                                                       \
            return reinterpret_cast<std::add_pointer_t<type>>(                         \
                (uintptr_t)(pThisClass) + m_key.offset + extra_offset);                \
        }                                                                              \
        operator type *() { return Get(); }                                            \
        type *operator()() { return Get(); }                                           \
        type *operator->() { return Get(); }                                           \
    } varName;

#define DECLARE_SCHEMA_CLASS_BASE(className, isStruct)       \
    typedef className ThisClass;                             \
    static constexpr const char *ThisClassName = #className; \
    static constexpr bool IsStruct = isStruct;

#endif