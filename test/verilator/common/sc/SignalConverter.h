#ifndef SIGNAL_CONVERTER_H
#define SIGNAL_CONVERTER_H

#include <systemc.h>

#include <type_traits>

template<typename InputType, typename OutputType>
struct SignalConverter : sc_module
{

    static_assert(std::is_integral<InputType>::value &&
                  std::is_integral<OutputType>::value,
                  "Both the input and output must be an integral type (i.e 'int', 'lont', etc.)");

    static_assert(std::is_convertible<InputType, OutputType>::value,
                  "The InputType must be convertible to OutputType");

    using input_type = InputType;
    using output_type = OutputType;

    sc_in<InputType>   input;
    sc_out<OutputType> output;

    void convert()
    {
        InputType in = input.read();
        if constexpr(std::is_convertible<InputType, OutputType>::value)
        {
            output.write(static_cast<OutputType>(in));
        }
        
    }

    SC_CTOR(SignalConverter)
    {
        SC_METHOD(convert);
        sensitive << input;
    }

};


#endif

