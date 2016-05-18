//
// Created by ih3 on 2/10/16.
//

#ifndef MADTRACKER3_GEN_MINMAX_H
#define MADTRACKER3_GEN_MINMAX_H

namespace util
{

template< typename T >
constexpr T max(T a, T b)
{
    return (a < b) ? b : a;
}

template< typename T >
constexpr T min(T a, T b)
{
    return (a < b) ? a : b;
}

}
#endif //MADTRACKER3_GEN_MINMAX_H
