#ifndef CHESSUCIENGINE_UTILS_H
#define CHESSUCIENGINE_UTILS_H

#include <iostream>

#define EnableAssertions false

#define HandleError(block) if (!(block)) {\
    std::cerr << "Execution fail at " << __FILE__ << ":" << __LINE__ << " (in " << __func__ << ")" << std::endl;\
    std::cerr << "Error code is " << GetLastError() << std::endl;\
    exit(GetLastError());\
}

#if EnableAssertions
#define Assert(block) if (!(block)) {\
    std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__ << " (in " << __func__ << ")" << std::endl;\
    exit(1);\
}
#else
#define Assert(block) block;
#endif

inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}

#define MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }

#endif //CHESSUCIENGINE_UTILS_H
