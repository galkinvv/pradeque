### Goal

Provide a container that would be like a std::vector suitable as "default container for c++" with following extra features:
* emplace/push/pop_front have the same complexity as _back variants
* emplace/push/pop_back/front and random access have *constant non-amortized* complexity (in terms of arithmetical operations and memory allocations)
* emplace/push/pop_back/front invalidates neither iterators nor pointers on (to keeped elements)



### Requirements to meet goal
* include all features above
* performance for emplace/push/pop_back and random access that is comparable with vectors
* memory efficiency for the case "many containers with a few objects"
* memory efficiency for the case "one container with a lot of objects"

### Ideas to implement

* basic structure: use table with arrays of exponential increasing size.
* implement infinite-queue-pattern usage by relocating same size arrays from the end of container to the beginng of container
 * relocation can be done iff elements fits in a few arrays of same size
  * more arrays of same size allows optimized memory usage - no need to fill entire big array to relocate
  * for optized memory usage size of next array should be not a lot greater than sum of sizes of all previous arrays
 * relocation of a few arrays can be efficiently done by storing array pointers in circular buffer and rolling it via base pointer
* optimize iteartor inc/dec operations by requiring that all ends of data blocks correspond to adresses with specific alignment, so most increment operations would just add constant to pointer and checks that such addition does't cross bound
* use aligned arrays allocation
 * for known to be small internal arrays retrieve pointer to array from pointer to specific element
 * for big arrays this achieves aligment requirement for bounds 
* possibly use small-size optimization: for initial elements allocate single array aligned 2 times more than typical block and just store two pointers to begin-end elements in it. This allows even efficient rotating for small queue usage pattern.
 * after expanding to table reuse first of them for pointer to table (other can be null or carry some other useful information).
* the iterator should contain all information that is required to perform iteration in same block with very rare acesses to table. Example iterator: pointer to element, pointer to table element which lower bits encodes at least lower limit of container size.
* the object itself should contain all information to perform all front/back related operations that does't require memory alloc/free without acessing the table.
* analyze fbvector optimizations and try to apply them
 * fbvector: allocation sizes that can be reused durin grow.
  * pradeque: in queue scenario of usage sum of all smaller blocks sizes are a bit smaller than the bigger block
 * fbvector: opt-in to allow copying via memcpy
  * pradeque: doesn't require copy constructor for operations not involving elements in the middle.
 * fbvector: autodetect and use jemalloc
  * pradeque: optimize performance for jemalloc case; split out allocator api alllowing using non-standard allocator with extra options to free
* tradeoffs: for first version tradeoffs may be made assuming system with large virtual address space and for which address computations are a lot faster than memory access.

### Use case analysis

* vector allows random acess by index, which is very fast.
 * Given index and reference to vector it requires one memory access to vector instance and dependent access to vector data at calculated address.
 * index is the only way to stably identify element in a vector, because vector iterators are not stable.
 * pradeque can use another approach to this. It's random access by index nearly always requires one more indirect memory lookup operation, that's bad.
But (index, reference to pradeque) pair shouldn't be used in this use case of pradeque, iterator should be used.
It would allow very fast (direct address) access to element, random access to elements in same block without extra lookup (better than vector),
And random access to element in another block with single extra lookup into table (like vector).
There would be a bit more address calculations but they expected to be trivial.

