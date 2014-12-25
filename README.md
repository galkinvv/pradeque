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
 * after expanding to table reuse this pointers for pointer to table (other null or some very useful information).
* analyze fbvector optimizations and try to aply them
