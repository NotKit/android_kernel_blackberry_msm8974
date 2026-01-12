extern void * high_memory;
#include <linux/module.h>
#include <asm/pgtable.h>

/* Wrapper to provide is_vmalloc_addr symbol when static inline fails to inline */
int is_vmalloc_addr(const void *x)
{
	unsigned long addr = (unsigned long)x;
	return addr >= VMALLOC_START && addr < VMALLOC_END;
}
EXPORT_SYMBOL(is_vmalloc_addr);
