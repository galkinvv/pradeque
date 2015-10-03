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
* implement infinite-queue-pattern usage by changing order of array groups inside container while all data fits in one group
 * relocation can be done iff elements fits in a one group of arrays
  * memory usage dependency on the size of arrays must be analyzed
 * relocation can be done by logically changing the order arrays groups are accessed
* optimize iteartor add/sub operations by requiring that all ends of data blocks correspond to adresses with specific alignment, so most increment operations would just add constant to pointer and checks that such addition does't cross bound
* use aligned arrays allocation
 * for known to be small internal arrays (table with array pointers, first block, maybe second block) retrieve pointer to array from pointer to specific element
 * for big arrays this achieves aligment requirement for bounds 
* no buffers except first/second contains associated extra internal data.
 * first buffer is allocated before global table so it does contain technical buffer being deque object itself which references to the table to make iterators be able to find table.
 * buffer boundaries must be aligned so internal data can't be placed inside aligned object - it must be placed near it
 * iterators pointing to first buffer that are created before allocating table has null table pointer and in standard bits contain information about block number like any other iterator
 * data allocated for first block contains: 2 buffers of size 2 ^ N for implementing circular behaviour for case of a few values and techical buffer of same size 2 ^ N
  * if allowed to place internal buffer on both sides of data it may be made relatively smaller but makes entire structure much more complicated. So it is always before
  * placement of array object inside at the exact start ob block allocated by allocator is made a tart of an external API
  * this ensures that for size of techical buffer is 2 times smaller than data buffers, so the memory overhead isn't so awful
* the iterator should contain all information that is required to perform iteration in same block with very rare acesses to table. Example iterator: pointer to element, pointer to table element which lower bits gives current block size & alignement.
* the object itself should contain all information to perform all front/back related operations that does't require memory alloc/free without acessing the table.
 * so it need to contain begin and end iterators
  * iterators share address part corresponding to table address. This area can be used for other information.
 * it's good for objext to contain all information required to calculate size
  * size can't be stored directly because require many bits
  * we can store extra information required to internal function of distance calculation
   * so to design what information must be stored here internal function of distance calculation must be designed
* operation modes
 * no-table-pointer mode
  * used until the table is firstly allocated
  * quite effective for small sizes, allows even small queues
  * is memory-effective for small sizes because no table is allocated.
  * for empty deque start inserting "front - at the start of first buffer, back - at the end of the second buffer"
   * allow backward/forward-queue-like usage for small size
   * allow both-sides deque usage with small size
  * after table is once allocated this mode is never used, because usage strategy shows that table was required and data about bidirectinal usage is collected.
  * after-end iteartor for no-table-pointer mode must have element address that is different from all present elements.
   * it must have a blockid with normal number and zero table pointer - it is a way to identify special after end iterator
   * it's pointer points to the array itself - it is another way to identify this special after end iterator
 * forward/backward table adressing
  * begin/end always contains references to table, even for small first blocks
  * iterator low 2-bits are used as flags indicating "order when iterator was created"
   * 2 bits allow to get the newer order given to iterators with consequtive orders
   * so iterator subtraction never requires table access
   * note that after-end iterator must be taken at the account too when deciding that new generation with another order can be created.
   * iterator addition use oppurtunistic approach - calculating table value and tries to dereference it. Low bits of non-zero table enties tell about current order - so order is verified
    * to be able to make checks all table elements must be initially zeroed. Allocator can implement zeroing without cache polluting.
* design of distance and advance operations
 * each iterator contains info about block size, so for both diff and advance operations we can say if they involves more than one block
  * if only one block is involved - use simplest arithmetic without any table access.
  * diff case - table is never consulted
   * compare "order generations" of two iterators and use the order from later generation
   * distance is calculated using array position, array number and newest order
  * advance case
   * if iteartor is in "no-table-pointer" mode, an array object is consulted to get current order.
	 * current order is loaded
    * if table exists iterator is upgraded to table mode
	 * item is loaded without checks, since we know current just-loaded order
	* make opportunistic read of desired table entry to get new iterator using the order encoded inside iterator
	 * if table position is not filled (initially zeroed, or marked as unused/freed) or signals another order - look the other order.
  * so for iterators with table pointer two techincal bits are used for describing order generation
* memory free strategy
 * it's a bad idea to free block immediately after it become unused because there is high chance it would be used again immediately
 * it's a bad idea to check about freeing blocks too often
  * idea 1: free blocks when next block become unused. Simple but very memoey effective. Neither std::vector is.
  * idea 2: free blocks when end iterator adjusted over next-minimal-block aside of unfreed block
* analyze fbvector optimizations and try to apply them
 * fbvector: allocation sizes that can be reused durin grow.
  * pradeque: in queue scenario of usage sum of all smaller blocks sizes are a bit smaller than the bigger block
 * fbvector: opt-in to allow copying via memcpy
  * pradeque: doesn't require copy constructor for operations not involving elements in the middle.
 * fbvector: autodetect and use jemalloc
  * pradeque: optimize performance for jemalloc case; split out allocator api allowing using non-standard allocator with extra options to free
* tradeoffs: for first version tradeoffs may be made assuming system with large virtual address space and for which address computations are a lot faster than memory access.
* tradeoffs: for first version tradeoffs may be made assuming compiler that can optimize out any calculation that has all data available in compile time (inlining with specific parameters).
* tradeoffs: extra memory vs iteration speed
  * tge bigger block allows faster iteration, the smaller block allows more efficient memory usage
  * require block size to be minimal for iteration speed be nearly equal to speed for very big block
  * be traditional, look how tgis is implemented in existing containers
   * all current deques limiting the size in term of bytes
   * gcc std vector uses 1 as min size.
* suggested log2 block sizes - example for 32-entry table (sum of max blocks sizes is 2 ^ 28 ): 5 (min block, allocated initially two blocks in one mem region), 7,7,7, 9,9,9, 11,11,11 ..., 23, 23, 23, 25,25,25,25
* log2 fields in params_t can be copactified by :8 bit width specifiction

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
* iterating all container non-changed part while container is modified.
* iterating container while it is not modified. Does not requires stable iterators, so may be implemented a bit more efficiently, for example by using contigous blocks of memory.
* builtin place to store reference counter/deleter (for std::allocate_shared)
* support custom allocator

### External API, "kernel" API
* C header declaring functions that gets extra argument with structure. Gives possibilyty to integrate into core of other projects.
 * Effeciency for basic "kernel" API is more importsant than simplicity of use. More simple API can be built over efficient API but not the other way.
 * Header-only implementation, since the functions without inlining are going to be too slow
 * Simple functions must be inlined, linkage of not so simple functions should ensure that same code would not be duplicated, but different code versions would not be combined bya linker
  * So public API must be static inline simple wrappers
  * And inner more complex must functions declared with version embedded in their names
  * linkage of types is not bothered(while compiling C++ with type_info)
 * "Kernel" must allow efficient implementations of operations on multiple elements, for example by allowing iteration by sequential groups.
  * all provided operations should operate in terms of iterating of contigous mem blocks.
   * the exception are operations that can be more efficiently computed without pre-contigous block iteration used even internally
 * Allow associating user data with deque object by special allocator allocating a greter buffer and putting user object before deque. It would may be accessed from deque pointer
  * This may be used by C++ wrapper to store allocator and/or shd::shared_ptr refcounting buffer
* C++11 header with high stdlib compatibility
* Ensure headears are warning-free for high warning levels
* common code
 * so C++ version have to be wrapper around C passing const parameters generated from template arguments
auto-generating wrapping extra parameter
* align is implicitly calculated from object size. This follows from efficient iteration imlementation and allows to keep API simpler.
