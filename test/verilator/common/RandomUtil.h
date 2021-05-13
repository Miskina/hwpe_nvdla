#ifndef RANDOM_UTIL_H
#define RANDOM_UIIL_H

#include <random>

#define MAX_INT 


namespace rnd
{

    static inline std::random_device device{};
    static inline std::mt19937 mt_engine{device()};
    
    template<typename Distribution>
    typename Distribution::result_type get_random(Distribution& distribution) noexcept
    {
        return distribution(mt_engine);
    }

} // namespace rnd




#endif