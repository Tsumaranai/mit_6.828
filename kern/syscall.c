/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs(const char *s, size_t len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.

	// LAB 3: Your code here.
	/*
	int i ;
	for(i = 0; i < len; i += PGSIZE){
		pte_t *ptep = pgdir_walk(curenv->env_pgdir, (void *) (s + i), false);
		if(! (ptep || *ptep & (PTE_P | PTE_U)) ){

			env_destroy(curenv);
		}

	}*/
	user_mem_assert(curenv, (void *) s, len, PTE_U);
	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
	return ;
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc(void)
{
	return cons_getc();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid(void)
{
	return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_destroy(envid_t envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	if (e == curenv)
		cprintf("[%08x] exiting gracefully\n", curenv->env_id);
	else
		cprintf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

// Dispatches to the correct kernel function, passing the arguments.
int32_t
syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.
	int sys_r = 0;
	//panic("syscall not implemented");

	switch (syscallno) {

	case SYS_cputs:
		sys_cputs((const char *)a1, (size_t) a2); break;

	case SYS_cgetc:
		sys_r = sys_cgetc(); break;

	case SYS_getenvid:
		sys_r = sys_getenvid(); break;

	case SYS_env_destroy:
		sys_r = sys_env_destroy((envid_t) a1); break;

	default:
		return -E_NO_SYS;
	}

	return sys_r;

}

