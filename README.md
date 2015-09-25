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
 * data allocated for first block contains: 2 buffers of size 2 ^ N for implementing circular behaviour for case of a few values and techical buffer of size 2 ^ (N-1).
  * this ensures that for size of techical buffer is 4 times smaller than data buffers, so the memory overhead is't so awful
  * this is achieved by allocating (2 ^ (N-1)) * 5 sized buffer allocated on 2 ^ (N-1) boundary, selecting 2 ^ N aligned part that is used for data buffer and the rest is used for technical data.
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
* operation modes
 * no-table-pointer mode
  * used until the table is firstly allocated
  * quite effective for small sizes, alllows even small queues
  * is memory-effective for small sizes because no table is allocated.
  * allows initially to use memory optimally for both "puth to one side" and "push to both sides" strategy
   * facts of bidirectional usage should be collected in a bit during this phase - they would be needed
  * after table is once allocated this mode is never used, because usage strategy shows that table was required and data about bidirectinal usage is collected.
  * after-end iteartor for no-table-pointer mode must have element address that is different from all present elements. So if it points immediately after block - it must have a special blockid and element pointer pointing immediately after two allocatde blocks
 * plain table adressing
  * never used for iterators pointing to max blocks, because tey need consult table to check for roundtripping.
  * used unntil first roundtrip-after queue usage occured
  * to be able switch from plain adressing to another mode we need to mark entire table as "initially ubused". To do it we pass special "need to zero memory" parameter to allocator. Allocator can implement zeroing without cache polluting.
 * no plain table addressing
  * many bits are required to store info. So they must be placed into block where adress has not-so much significant bits, but better to place it in the same chache line as first table pointers. 8-th pointer seems a good candidate. Cache way-ness is't got worse here because first table pointers are going to be read anyway.
* design of distance and advance operations
 * each iterator contains info about block size, so for both diff and advance operations we can say if they involves more than one block
  * if only one block is involved - use simplest arithmetic without any table access.
  * diff case
   * if both iteartors are in "no-table-pointer" mode that is used for first block before table allocation calculate diff using block relative position info encoded in two info bits of iterator
    * each of two blocks has 2-bit integer corresponding to it. Integers differ by one mod 4. The smaller by one precceds the bigger. When block is empt it can be placed on the other side of other by adding 2 mod 4.
	 * otherwise both iterators are upgraded by reaching table using it reference in ther iterator.
	* if both iterators has "plain table adressing" bit set difference is calculated with plain table adressing mode
	* otherwise table is consulted to get current adressing mode and difference is calculated with it.
  * advance case
   * if iteartor is in "no-table-pointer" mode, a field-near-first block is consulted.
    * if table exists iterator is upgraded to current mode.
	 * otherwise advance is calculated with assumption that requested block exists.
	* if iterator "plain table adressing" bit set calculate target table position and read it.
	 * if table position is not filled (initially zeroed, or marked as unused/freed) upgrade to "no plain adressing" mode
	* for non-plain-addressing table is consulted first time to get current adressing mode and second time to load pointer to allocated block.
  * so for iterators with table pointer two techincal bits are used:
   * bit describing if plain adressing mode should be tried
   * (there was idea to have a bit that can be used to determine if distance should be calculated over zero or ovre max but this can be solved by size limiting and using non-plain for max-size blocks)
* memory free strategy
 * it's a bad idea to free block immediately after it become unused because there is high chance it would be used again
 * it's a bad idea to check about freeing blocks too often
  * idea 1: free blocks when next block become unused. Simple but very memoey effective. Neither std::vector is.
  * idea 2: free blocks when end iterator adjusted over next-minimal-block aside of unfreed block
* analyze fbvector optimizations and try to apply them
 * fbvector: allocation sizes that can be reused durin grow.
  * pradeque: in queue scenario of usage sum of all smaller blocks sizes are a bit smaller than the bigger block
 * fbvector: opt-in to allow copying via memcpy
  * pradeque: doesn't require copy constructor for operations not involving elements in the middle.
 * fbvector: autodetect and use jemalloc
  * pradeque: optimize performance for jemalloc case; split out allocator api alllowing using non-standard allocator with extra options to free
* tradeoffs: for first version tradeoffs may be made assuming system with large virtual address space and for which address computations are a lot faster than memory access.
* tradeoffs: for first version tradeoffs may be made assuming compiler that can optimize out any calculation that has all data available in compile time (inlining with specific parameters).
* tradeoffs: extra memory vs iteration speed
  * tge bigger block allows faster iteration, the smaller block allows more efficient memory usage
  * require block size to be minimal for iteration speed be nearly equal to speed for very bif block
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
* C++11 header with high stdlib compatibility
* Ensure headears are warning-free for hhigh warning levels
* common code
 * so C++ version have to be wrapper around C passing const parameters generated from template arguments
auto-generating wrapping extra parameter
* align is implicitly calculated from object size. This follows from efficient iteration imlementation and allows to keep API simpler.
