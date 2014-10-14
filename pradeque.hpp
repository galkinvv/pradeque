namespace pradeque_impl
{
template <class T, class TSize, int t_log2_max_size>
struct core
{
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
private:
    ArrayTablePtr table;//auto pointer should handle move construction and calling function that release memory block of all table ethat
}
}
