### Goal

Provide a container that would be like a std::vector suitable as "default container for c++" with following extra features:
* emplace/push/pop_front have the same complexity as _back variants
* emplace/push/pop_back/front and random access have *constant non-amortized* complexity (in terms of arithmetical operations and memory allocations)
* emplace/push/pop_back/front invalidates neither iterators nor pointers on (to keeped elements)



### Requirements to meet goal
* include all features above
* performance for emplace/push/pop back and random access that is comparable with vectors
* performance for operations operating on multiple elements that is comparable with vectors
* memory efficiency for the case "many containers with a few objects"
* memory efficiency for the case "one container with a lot of objects"

### Ideas to implement

* basic structure: use table with arrays of exponential increasing size.
* implement infinite-queue-pattern usage by relocating same size arrays from the end of container to the beginng of container
 * relocation can be done iff elements fits in a few arrays of same size
  * more arrays of same size allows optimized memory usage - no need to allocate entire big array to relocate data that use small portion of it
  * for optimized memory usage size of next array should be not a lot greater than sum of sizes of all previous arrays
 * relocation of a few arrays can be efficiently done by storing array pointers in circular buffer and rolling it via base pointer
* optimize iteartor inc/dec operations by requiring that all ends of data blocks correspond to adresses with specific alignment, so most increment operations would just add constant to pointer and checks that such addition does't cross bound
* use aligned arrays allocation
 * for known to be small internal arrays retrieve pointer to array from pointer to specific element
 * for big arrays this achieves aligment requirement for bounds 
* no buffers except first contains associated extra internal data.
 * first buffer is allocated before global table so it does contain reference to that table to make iterators be able to find table.
 * buffer boundaries must be aligned so internal data can't be placed inside aligned object - it must be placed near it
 * iterators pointing to first buffer that are created before allocating table has null table pointer and in extra bits may contain information placement
 * data allocated for first block contains: 2 buffers of size 2**N for implementing circular behaviour for case of a few values and techical buffer of size 2**(N-1).
  * this ensures that for size of techical buffer is 4 times smaller than data buffers, so the memory overhead is't so awful
  * this is achieved by allocating (2**(N-1))*5 sized buffer allocated on 2**(N-1) boundary, selecting 2**N aligned part that is used for data buffer and the rest is used for technical data.
  * the side that technical data is placed according the buffer is encoded in iterator extra bits.
* possibly use small-size optimization: for initial elements allocate single array aligned 2 times more than typical block and just store two pointers to begin-end elements in it. This allows even efficient rotating for small queue usage pattern.
 * after expanding to table reuse first of them for pointer to table (other can be null or carry some other useful information).
* the iterator should contain all information that is required to perform iteration in same block with very rare acesses to table. Example iterator: pointer to element, pointer to table element which lower bits encodes at least lower limit of container size.
* the object itself should contain all information to perform all front/back related operations that does't require memory alloc/free without acessing the table.
 * so it need to contain begin and end iterators
  * iterators share address part corresponding to table address. This area can be used for other information.
 * it's good for objext to contain all information required to calculate size
  * size can't be stored directly because require many bits
  * we can store extra information required to internal function of distance calculation
   * so to design what information must be stored here internal function of distance calculation must be designed
    * first check if iterators belong to the same block
	  * if they are - just subtract
	  * otherwise load information about block size and from-zero-side from table
	 * **Think about including this info in main block id**
	 * if sizes and from-zero-side are the same

   * to be sure that distance between iterators pointing to the same block is simple we should limit container max size to such value that no block can contain both begin and end for container with max_size
   * one possibility of extra information required to disatnce calculation is the logical position of smallest blocks in the table.
* analyze fbvector optimizations and try to apply them
 * fbvector: allocation sizes that can be reused durin grow.
  * pradeque: in queue scenario of usage sum of all smaller blocks sizes are a bit smaller than the bigger block
 * fbvector: opt-in to allow copying via memcpy
  * pradeque: doesn't require copy constructor for operations not involving elements in the middle.
 * fbvector: autodetect and use jemalloc
  * pradeque: optimize performance for jemalloc case; split out allocator api alllowing using non-standard allocator with extra options to free
* tradeoffs: for first version tradeoffs may be made assuming system with large virtual address space and for which address computations are a lot faster than memory access.
* tradeoffs: extra memory vs iteration speed
  * tge bigger block allows faster iteration, the smaller block allows more efficient memory usage
  * require block size to be minimal for iteration speed be nearly equal to speed for very bif block
  * be traditional, look how tgis is implemented in existing containers
   * all current deques limiting the size in term of bytes
   * gcc std vector uses 1 as min size.

### Use case analysis

* vector allows random acess by index, which is very fast.
 * Given index and reference to vector it requires one memory access to vector instance and dependent access to vector data at calculated address.
 * index is the only way to stably identify element in a vector, because vector iterators are not stable.
 * pradeque can use another approach to this. It's random access by index nearly always requires one more indirect memory lookup operation, that's bad.
But (index, reference to pradeque) pair shouldn't be used in this use case of pradeque, iterator should be used.
It would allow very fast (direct address) access to element, random access to elements in same block without extra lookup (better than vector),
And random access to element in another block with single extra lookup into table (like vector).
There would be a bit more address calculations but they expected to be trivial.
* for allow using in different scenarious the object must be move-assignable(C)/move-constructible(C++). So no internal pointers to main object is allowed.

### External API, "kernel" API
* C header declaring functions that gets extra argument with structure. Gives possibilyty to integrate into core of other projects.
 * Effeciency for basic "kernel" API is more importsant than simplicity of use. More simple API can be built over efficient API but not the other way.
 * "Kernel" must allow efficient implementations of operations on multiple elements, for example by allowing iteration by sequential groups.
* C++11 header with high stdlib compatibility
* common code
 * so C++ version have to be wrapper around C passing const parameters generated from template arguments
auto-generating wrapping extra parameter
* align is implicitly calculated from object size. This follows from efficient iteration imlementation and allows to keep API simpler.
