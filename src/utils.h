#ifndef _utils_h
#define _utils_h

#include <memory>
#include <string>
#include <stdexcept>

#define PREFIX "[Swiftly]"

#define GCC_COMPILER (defined(__GNUC__) && !defined(__clang__))

#define PRINT(CATEGORY, FORMAT_STR) g_SMAPI->ConPrint(std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str())
#define PRINTF(CATEGORY, FORMAT_STR, ...) g_SMAPI->ConPrintf(std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str(), __VA_ARGS__)
#define PRINTRET(CATEGORY, FORMAT_STR, RET) \
    {                                       \
        PRINT(CATEGORY, FORMAT_STR);        \
        return RET;                         \
    }
#define PRINTFRET(CATEGORY, FORMAT_STR, RET, ...)  \
    {                                              \
        PRINTF(CATEGORY, FORMAT_STR, __VA_ARGS__); \
        return RET;                                \
    }
#define CLIENT_PRINT(SLOT, CATEGORY, FORMAT_STR) g_SMAPI->ClientConPrint(SLOT, std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str())
#define CLIENT_PRINTF(SLOT, CATEGORY, FORMAT_STR, ...) g_SMAPI->ClientConPrintf(SLOT, std::string(PREFIX).append(" [").append(CATEGORY).append("] ").append(FORMAT_STR).c_str(), __VA_ARGS__)

template <typename T, int index, typename... Args>
constexpr T CallVFunc(void *pThis, Args... args) noexcept
{
    return reinterpret_cast<T (*)(void *, Args...)>(reinterpret_cast<void ***>(pThis)[0][index])(pThis, args...);
}

#ifdef _WIN32
#define WIN_LINUX(win, linux) win
#else
#define WIN_LINUX(win, linux) linux
#endif

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0)
        throw std::runtime_error("Error during formatting.");

    size_t size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

#define CALL_VIRTUAL(retType, idx, ...) \
    vmt::CallVirtual<retType>(idx, __VA_ARGS__)

namespace vmt
{
    template <typename T = void *>
    inline T GetVMethod(unsigned int uIndex, void *pClass)
    {
        if (!pClass)
        {
            return T();
        }

        void **pVTable = *static_cast<void ***>(pClass);
        if (!pVTable)
        {
            return T();
        }

        return reinterpret_cast<T>(pVTable[uIndex]);
    }

    template <typename T, typename... Args>
    inline T CallVirtual(unsigned int uIndex, void *pClass, Args... args)
    {
#ifdef _WIN32
        auto pFunc = GetVMethod<T(__thiscall *)(void *, Args...)>(uIndex, pClass);
#else
        auto pFunc = GetVMethod<T(__cdecl *)(void *, Args...)>(uIndex, pClass);
#endif
        if (!pFunc)
        {
            return T();
        }

        return pFunc(pClass, args...);
    }
}

#endif