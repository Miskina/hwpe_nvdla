#include "Interruptable.h"

void Interruptable::interrupt() noexcept
{
    got_interrupt_ = true;
}

bool Interruptable::is_interrupted() const noexcept
{
    return got_interrupt_;
}

void Interruptable::clear_interrupt() noexcept
{
    got_interrupt_ = false;
}