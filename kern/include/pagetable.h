#include <types.h>
#include <lib.h>
#include <synch.h>

/* pagetable API */

#define VALID_B 0x80
#define REF_B   0x40
#define WRITE_B 0x20
#define R_B	0x10
#define W_B	0x08
#define X_B	0x04
#define SUPER_B 0x01

#define FRAME( x ) (bframe + (x * PAGE_SIZE))
#define PTE_VALID( x ) (x.control & VALID_B)

/* The attributes of the PTE (Page Table Entry) assume 
 * an inverted pagetable */

/* the pagetable is an array of pagetable entries */
struct pte  *pagetable;
struct lock *pagetable_lock;
u_int32_t    pagetable_size;
paddr_t	     bframe;

/* boolean used to determine whether to use ram_stealmem
 * or pagetable function */
int pagetable_initialized = 0;


/* an inverted pagetable entry 
 * page    - the virtual address holding the frame
 * owner   - a process id representing an owner of the frame 
 * control - control bits
 * 	.     .     .     .     .     .     .      . 
 * 	^     ^     ^     ^	^     ^     ^      ^
 * 	|     |     |     |     |     |     |      |
 *    valid  ref  write   r     w     x  reserved supervisor
 *
 * next    - an index into the pagetable containing the next ptr,
 * 	   negative one when none exists. 
 */

struct pte
{
	vaddr_t   page;
	pid_t     owner;
	u_int8_t  control;
	int16_t   next;
};

/* bootstrap */
void
pagetable_bootstrap(void);

/* add a page belonging to the current process with the passed
 * permissions */
void
addpage(vaddr_t page, int read, int write, int execute);

/* invalidate the passed page belonging to the current process */
void
invalidatepage(vaddr_t page);

/* returns a pointer to a pte belonging to the current process. 
 * Returns NULL on failure. */
struct pte *
getpte(vaddr_t page);

/* the inverted pagetable hash function. The result of this function
 * determines which index into the pagetable we place a given page.
 * the function takes both the virtual address representing the page
 * into account as well as the process id of the process calling the 
 * function. We give process IDs a higher precedence over the page
 * to account for the high likely hood that many processes will be 
 * referencing the same virtual address */
int
hash(vaddr_t page);