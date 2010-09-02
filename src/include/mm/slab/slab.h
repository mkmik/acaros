#ifndef _SLAB_H_
#define _SLAB_H_

#include <types.h>
#include <list.h>
#include <algo/avl.h>

#define SLAB_MAX_GFP_ORDER 6

#define SLAB_NORMAL 0
#define SLAB_DMA 1

/** A slab cache is a repository of objects of the same size and same type.
 * Objects are allocated very efficently, also because they are not destroyed
 * upon free, but are kept cached in their initialized state until memory is 
 * eventually reclaimed by the memory manager.
 *  A slab cache maintans pointers to a constructor callback that will be 
 * called when an object really needs to be constructed; it will not be called
 * on each allocation but only when fresh pages are a allocated.
 *  The user should leave the object in it's initialized (or compatible) state upon free.
 * The destructor will be called only when the memory is reclaimed.
 *
 * The slab cache maintains separate lists of full, paritial, and free slabs.
 * Objects are allocated first from the partial slab list, and if that list is empty,
 * the next free slab is used (and then moved to the partial list).
 *  When the memory manager needs memory some free slabs are released.
 *
 * When we must free an object, we must quickly locate the slab which contains
 * it, but all we have is the object's address. The current implementation
 * uses a AVL search tree to quickly assign slab memory ranges to slab pointers,
 * but such a tree can easly grow to depth > 10. Alternative implementations
 *  could map the slab pointer directly in the pfd, and use the tree if the page
 * is not mapped in phyisical memory (and searching is not the performance issue then)
 **/
struct slab_cache {
  /** full slabs. never searched for allocation */
  list_head_t slabs_full;
  /** partial slabs. searched first for allocation */
  list_head_t slabs_partial;
  /** free slabs. used for allocation only if there are no partial slabs.
   * free slabs can be easly reclaimed by the memory manager when memory is low. */
  list_head_t slabs_free;
  /** search tree that maps data area with the associated slab.
   * useful when freeing an object */
  avl_node_t *slabMapRoot;
  /** size of every object */
  size_t objsize;
  /** flags */
  unsigned int flags;
  /** number of objects in each slab */
  unsigned int num;
  /** This indicates the size of the slab in pages. Each slab consumes 
   * 2 ^^ gfporder pages as these are the allocation sizes the buddy allocator 
   * (TODO: implement the buddy allocator) */
  unsigned int gfporder;

  unsigned int dflags;
  /** name of the cache */
  char *name;

  /** pointer to the pool from which get pages */
  //  Pool_t *pool; // find a way to define Pool_t without being a "ho" file

  /** this call back with be called with the pointer to the newly allocated 
   * object as argument, and it should initialize the object */
  void *(*constructor)(void*, size_t);
  /** this function is called when the backing slab is returned to the memory manager
   * (not on every free). It should free additional resources or whatever needed.
   **/
  void *(*destructor)(void*, size_t);
};

typedef struct slab_cache slab_cache_t;

typedef u_int32_t slab_bufctl_t;

/** sentinel value to mark end of bufctl list */
#define BUFCTL_END 0xFFFFFF

/** A slab consists of a header and a set of contigous objects of the same size.
 * Slabs are maintained in a slab cache.
 * Objects are allocated after the slab header or in an external memory area.
 * Free objects in the slabs are "linked" together using bufctl's, numerical
 * indexes which "point" to the next free object.
 */
struct slab_slab {
  list_head_t list;
  /** back pointer to the cache */
  slab_cache_t *cache;
  /** maps data area with associated slab */
  avl_node_t slabMap;
  /** points to the starting address of the 
   * first object in the slab */
  void* mem;
  /** number or active objects in the slab */
  unsigned int inuse;
  slab_bufctl_t free;
  slab_bufctl_t buffs[0];
};

typedef struct slab_slab slab_slab_t;

/** tree insertion: order by start address */
avl_make_compare(slab_slab_t, slabMap, mem);
/** tree search: match a range by containing address */
static inline int slab_slab_t_avlmatch(void* k, slab_slab_t *b) {
  if(k >= b->mem && k < (b->mem+b->cache->objsize))
    return 0;
  else
    return k < b->mem ? -1 : 1;
}

/** normal malloc is implemented using segregated storage allocator
 * based upon the slab allocator. A cache is created for every power of 2 size
 * from 64 to a configurable upper bound. When a specific size is requested
 * the closes power of 2 is choosen and the object is allocated inside the
 * cache associated with that size. */
struct slab_size_cache {
  size_t size;
  avl_node_t node;
  slab_cache_t *cache;
};

typedef struct slab_size_cache slab_size_cache_t;

avl_make_compare(slab_size_cache_t, node, size);
avl_make_match(slab_size_cache_t, size);

/** initializes the slab allocator */
extern void slab_init();
/** creates a new cache
 * name: name of the cache
 **/
extern slab_cache_t* slab_createCache(char* name, size_t size, int align,
				      int flags, 
				      void *(*constructor)(void*, size_t),
				      void *(*destructor)(void*, size_t));

#define SLAB_SLEEP 0
#define SLAB_NOSLEEP 1

/** allocate an object from the cache.
 * flags can be SLAB_DONT_SLEEP etc.
 * the size is implicit by choosing that cache */
extern void* slab_alloc(slab_cache_t* cache, unsigned int flags);

/** free an object.
 * TODO: without the cache pointer!
 */
extern void slab_free(slab_cache_t* cache, void* object);

/** computes the best page order (2 ^^ pageorder = number of pages) for allocating
 * objects of this size */
extern int slab_computePageOrder(size_t size);
/** computes the number of object and memory wastage using a specific size and page order */
extern int slab_cacheEstimate(size_t size, unsigned int order, unsigned int *w);

/** interface to the size cache */
extern void* slab_malloc(size_t size, unsigned int flags);
/** free a generic pointer. TODO: merge with slab_free */
extern void slab_mfree(void* addr);

#endif
