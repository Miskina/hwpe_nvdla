#ifndef HWPE_NVDLA_INTERRUPTABLE_H
#define HWPE_NVDLA_INTERRUPTABLE_H


template<typename T>
class has_on_interrupt
{

private:
    template<typename U> static char test( decltype(&U::on_interrupt) );
    template<typename U> static int test(...);

public:
    static inline constexpr bool value = test<T>(0) == sizeof(char);
};


// template<typename InterruptableType>
class Interruptable
{

public:

    void interrupt() noexcept
    {

        // if constexpr (has_on_interrupt<InterruptableType>::value)
        // {
        //     InterruptableType& this_ = static_cast<InterruptableType&>(*this);
        //     this_.on_interrupt();
        // }
        got_interrupt_ = true;
    }


    bool is_interrupted() const noexcept
    {
        return got_interrupt_;
    }


protected:

    void clear_interrupt() noexcept
    {
       got_interrupt_ = false;
    }

    bool got_interrupt_ = false;

};

#endif // HWPE_NVDLA_INTERRUPTABLE_H