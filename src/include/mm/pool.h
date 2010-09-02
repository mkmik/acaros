#ifndef _MM_POOL_H_
#define _MM_POOL_H_

#include <types.h>
#include <list.h>
#include <algo/avl.h>

#define POOL_SLOT_UNALLOCATED 0
#define POOL_SLOT_ALLOCATED 1
#define POOL_SLOT_FREE (2 | POOL_SLOT_ALLOCATED)
#define POOL_SLOT_USED (4 | POOL_SLOT_ALLOCATED)

/** defines an element where pool allocator metadata resides
 * slots themselves are allocated inside chunks using a free list algorithm.
 *  Slots define the size and position of a range inside a pool. 
 * Slots can be "used" or "free". used slots contains data for quick locating 
 * a slot given an address (AVL tree, similar to AddressSpace and Ranges)
 * while free slots are linked together in a free list, describing free memory ranges 
 * in the pool. 
 *
 * Slots are allocated from page sized metadata space called chunks using 
 * a simple free list algorithm. Free slots inside a chunk are linked together in a 
 * unallocatedSlots list. When a slot is allocated it is removed from this list and it can
 * become a "used" or "free" slot. Note that "used" and "free" slots are both
 * "allocated" slots and contain valid data.
 */
struct pool_slot {
  /** slots can be either unallocated or allocated 
   * unallocated slots use the "unallocatedSlots element
   * of this union while allocated slots use the structure below
   */
  union {
    /** node of list linking together unallocated slots.
     * used when the slot is in its unallocated state */
    list_head_t unallocatedSlots;
    /** used when the slot is in its allocated state.
     * depending on the type it describes an used or a free range */
    struct { 
      /** start address or the range */
      pointer_t start; 
      /** length of the range in bytes */
      size_t length;   
      union {
	/** used slots are stored in a AVL tree with this member */
	avl_node_t range;
	/** free slots are stored in a free list using this member */
	list_head_t freeList;
      };
    };
  };
  /** type of the slot. bitmask.
   * reflect the ALLOCATED/UNALLOCATED USED/FREE state */
  u_int8_t type;
  /** just for debug */
  int debugID;
} PACKED;

typedef struct pool_slot pool_slot_t;

/** tree insertion: order by start address */
avl_make_compare(pool_slot_t, range, start);
/** tree search: match a range by containing address */
static inline int pool_slot_t_avlmatch(pointer_t k, pool_slot_t *b) {
  if(k >= b->start && k < (b->start+b->length))
    return 0;
  else
    return k < b->start ? -1 : 1;
}

/** Chunks are page sized metadata units that contains metadata slots.
 * When the metadata grows chunks are allocated like every other range
 * and added to the chunkList. Every chunk maintains its unallocatedSlot list
 * (TODO: subject to change!!) and a unallocated slot count for fast detection
 * of near saturation condition, because chunks must have free slots in order 
 * to allocate a new chunk for growing the chunk list. 
 */
struct pool_chunk {
  /** node in the pool's chunk list */
  list_head_t chunkList;
  /** head of the list of unallocated slots */
  list_head_t unallocatedSlots;
  /** caches the length of the unallocatedSlots list */
  u_int16_t numUnallocatedSlots;
  /** start of the array of slots contained in this chunk */
  pool_slot_t slots[0];
} PACKED;

typedef struct pool_chunk pool_chunk_t;


/** size of the virtual region containing 
 * the non paged pool. pages are allocated as needed
 * but never paged out. */
#define NON_PAGED_POOL_SIZE (0x4000000) // 64MB

/** init various system pools */
void mm_pool_init();


#endif
