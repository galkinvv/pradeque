#pragma once
#include "pradeque.h"

template <class T>
class PraDeque
{
public:
    class iterator
    {
       
    };
	template <class... Args>
    void emplace_front(Args&&... args)
    {
        //emplace_* methods are only ones having complex exception safety semantics
        //there are two points that can throw exceptions: memory allocation
        //and contained object construction.
        //Problem is solved by ensuring that the state is correct before any operation that can throw.
        //No any rollbacks are done, so presence of unused but allocated block treated as correct state.
        //First: we allocate memory and save a pointer in a table, now we own it.
        //We never need to allocate more than one memory block.
        //Second: we construct an object in allocated memory.
        //Third: we update bounds of container

    }
    
    template <class... Args>
    void emplace_back(Args&&... args){}
};
