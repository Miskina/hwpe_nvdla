#ifndef __DLA_FIRMWARE_COMMON_H__
#define __DLA_FIRMWARE_COMMON_H__

#define DLA_MACRO_START do {
#define DLA_MACRO_END } while(0)

#ifndef DLA_STAT_ENABLE
    #define DLA_STAT_ENABLE 0
#endif

#ifndef DLA_CONV_STAT_ENABLE
    #define DLA_CONV_STAT_ENABLE 0
#endif



// ##############################################################################
// ##############################################################################
// ######################## STRUCTURE DEFINITITIONS #############################
// ##############################################################################
// ##############################################################################

/** \brief A common struct used as a vector of two values.
 *
 * Structure defining a 2D vector. Used in convolution
 * operations for strides, padding, etc.
 * Has x,y coordinates, but can be accessed as an array through
 * the values field.
 * */
struct dla_vec_2d
{
    union
    {
        struct
        {
            uint32_t x;
            uint32_t y;
        };
        uint32_t values[2];
    };
};



#endif // __DLA_FIRMWARE_UTIL_H__
