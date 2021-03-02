/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2016, Linaro Limited
 * Copyright (c) 2014, STMicroelectronics International N.V.
 */
#ifndef CORE_MEMPROT_H
#define CORE_MEMPROT_H

#include <assert.h>
#include <compiler.h>
#include <types_ext.h>
#include <util.h>

#ifndef __ASSEMBLER__
/*
 * Memory area type:
 * MEM_AREA_END:      Reserved, marks the end of a table of mapping areas.
 * MEM_AREA_TEE_RAM:  core RAM (read/write/executable, secure, reserved to TEE)
 * MEM_AREA_TEE_RAM_RX:  core private read-only/executable memory (secure)
 * MEM_AREA_TEE_RAM_RO:  core private read-only/non-executable memory (secure)
 * MEM_AREA_TEE_RAM_RW:  core private read/write/non-executable memory (secure)
 * MEM_AREA_NEX_RAM_RW: nexus private r/w/non-executable memory (secure)
 * MEM_AREA_TEE_COHERENT: teecore coherent RAM (secure, reserved to TEE)
 * MEM_AREA_TEE_ASAN: core address sanitizer RAM (secure, reserved to TEE)
 * MEM_AREA_IDENTITY_MAP_RX: core identity mapped r/o executable memory (secure)
 * MEM_AREA_TA_RAM:   Secure RAM where teecore loads/exec TA instances.
 * MEM_AREA_NSEC_SHM: NonSecure shared RAM between NSec and TEE.
 * MEM_AREA_RAM_NSEC: NonSecure RAM storing data
 * MEM_AREA_RAM_SEC:  Secure RAM storing some secrets
 * MEM_AREA_IO_NSEC:  NonSecure HW mapped registers
 * MEM_AREA_IO_SEC:   Secure HW mapped registers
 * MEM_AREA_EXT_DT:   Memory loads external device tree
 * MEM_AREA_RES_VASPACE: Reserved virtual memory space
 * MEM_AREA_SHM_VASPACE: Virtual memory space for dynamic shared memory buffers
 * MEM_AREA_TA_VASPACE: TA va space, only used with phys_to_virt()
 * MEM_AREA_DDR_OVERALL: Overall DDR address range, candidate to dynamic shm.
 * MEM_AREA_SEC_RAM_OVERALL: Whole secure RAM
 * MEM_AREA_MAXTYPE:  lower invalid 'type' value
 */
enum teecore_memtypes {
	MEM_AREA_END = 0,
	MEM_AREA_TEE_RAM,
	MEM_AREA_TEE_RAM_RX,
	MEM_AREA_TEE_RAM_RO,
	MEM_AREA_TEE_RAM_RW,
	MEM_AREA_NEX_RAM_RW,
	MEM_AREA_TEE_COHERENT,
	MEM_AREA_TEE_ASAN,
	MEM_AREA_IDENTITY_MAP_RX,
	MEM_AREA_TA_RAM,
	MEM_AREA_NSEC_SHM,
	MEM_AREA_RAM_NSEC,
	MEM_AREA_RAM_SEC,
	MEM_AREA_IO_NSEC,
	MEM_AREA_IO_SEC,
	MEM_AREA_EXT_DT,
	MEM_AREA_RES_VASPACE,
	MEM_AREA_SHM_VASPACE,
	MEM_AREA_TA_VASPACE,
	MEM_AREA_PAGER_VASPACE,
	MEM_AREA_SDP_MEM,
	MEM_AREA_DDR_OVERALL,
	MEM_AREA_SEC_RAM_OVERALL,
	MEM_AREA_MAXTYPE
};

static inline const char *teecore_memtype_name(enum teecore_memtypes type)
{
	static const char * const names[] = {
		[MEM_AREA_END] = "END",
		[MEM_AREA_TEE_RAM] = "TEE_RAM_RWX",
		[MEM_AREA_TEE_RAM_RX] = "TEE_RAM_RX",
		[MEM_AREA_TEE_RAM_RO] = "TEE_RAM_RO",
		[MEM_AREA_TEE_RAM_RW] = "TEE_RAM_RW",
		[MEM_AREA_NEX_RAM_RW] = "NEX_RAM_RW",
		[MEM_AREA_TEE_ASAN] = "TEE_ASAN",
		[MEM_AREA_IDENTITY_MAP_RX] = "IDENTITY_MAP_RX",
		[MEM_AREA_TEE_COHERENT] = "TEE_COHERENT",
		[MEM_AREA_TA_RAM] = "TA_RAM",
		[MEM_AREA_NSEC_SHM] = "NSEC_SHM",
		[MEM_AREA_RAM_NSEC] = "RAM_NSEC",
		[MEM_AREA_RAM_SEC] = "RAM_SEC",
		[MEM_AREA_IO_NSEC] = "IO_NSEC",
		[MEM_AREA_IO_SEC] = "IO_SEC",
		[MEM_AREA_EXT_DT] = "EXT_DT",
		[MEM_AREA_RES_VASPACE] = "RES_VASPACE",
		[MEM_AREA_SHM_VASPACE] = "SHM_VASPACE",
		[MEM_AREA_TA_VASPACE] = "TA_VASPACE",
		[MEM_AREA_PAGER_VASPACE] = "PAGER_VASPACE",
		[MEM_AREA_SDP_MEM] = "SDP_MEM",
		[MEM_AREA_DDR_OVERALL] = "DDR_OVERALL",
		[MEM_AREA_SEC_RAM_OVERALL] = "SEC_RAM_OVERALL",
	};

	COMPILE_TIME_ASSERT(ARRAY_SIZE(names) == MEM_AREA_MAXTYPE);
	return names[type];
}
#endif

/*
 * "pbuf_is" support.
 *
 * core_vbuf_is()/core_pbuf_is() can be used to check if a teecore mapped
 * virtual address or a physical address is "Secure", "Unsecure", "external
 * RAM" and some other fancy attributes.
 *
 * DO NOT use 'buf_is(Secure, buffer)==false' as a assumption that buffer is
 * UnSecured ! This is NOT a valid asumption ! A buffer is certified UnSecured
 * only if 'buf_is(UnSecure, buffer)==true'.
 */

/* memory atttributes */
enum buf_is_attr {
	CORE_MEM_CACHED,
	CORE_MEM_NSEC_SHM,
	CORE_MEM_NON_SEC,
	CORE_MEM_SEC,
	CORE_MEM_TEE_RAM,
	CORE_MEM_TA_RAM,
	CORE_MEM_SDP_MEM,
	CORE_MEM_REG_SHM,
};

/* redirect legacy tee_vbuf_is() and tee_pbuf_is() to our routines */
#define tee_pbuf_is     core_pbuf_is
#define tee_vbuf_is     core_vbuf_is

/* Convenience macros */
#define tee_pbuf_is_non_sec(buf, len) \
		core_pbuf_is(CORE_MEM_NON_SEC, (paddr_t)(buf), (len))

#define tee_pbuf_is_sec(buf, len) \
		core_pbuf_is(CORE_MEM_SEC, (paddr_t)(buf), (len))

#define tee_vbuf_is_non_sec(buf, len) \
		core_vbuf_is(CORE_MEM_NON_SEC, (void *)(buf), (len))

#define tee_vbuf_is_sec(buf, len) \
		core_vbuf_is(CORE_MEM_SEC, (void *)(buf), (len))

/*
 * This function return true if the buf complies with supplied flags.
 * If this function returns false buf doesn't comply with supplied flags
 * or something went wrong.
 *
 * Note that returning false doesn't guarantee that buf complies with
 * the complement of the supplied flags.
 */
bool core_pbuf_is(uint32_t flags, paddr_t pbuf, size_t len);

/*
 * Translates the supplied virtual address to a physical address and uses
 * tee_phys_buf_is() to check the compliance of the buffer.
 */
bool core_vbuf_is(uint32_t flags, const void *vbuf, size_t len);

/*
 * Translate physical address to virtual address using specified mapping
 * Returns NULL on failure or a valid virtual address on success.
 */
void *phys_to_virt(paddr_t pa, enum teecore_memtypes m);

/*
 * Translate physical address to virtual address trying MEM_AREA_IO_SEC
 * first then MEM_AREA_IO_NSEC if not found.
 * Returns NULL on failure or a valid virtual address on success.
 */
void *phys_to_virt_io(paddr_t pa);

/*
 * Translate virtual address to physical address
 * Returns 0 on failure or a valid physical address on success.
 */
paddr_t virt_to_phys(void *va);

/*
 * Return runtime usable address, irrespective of whether
 * the MMU is enabled or not.
 */
vaddr_t core_mmu_get_va(paddr_t pa, enum teecore_memtypes type);

/* Return true if @va relates to a unpaged section else false */
bool is_unpaged(void *va);

struct io_pa_va {
	paddr_t pa;
	vaddr_t va;
};

/*
 * Helper function to return a physical or virtual address for a device,
 * depending on whether the MMU is enabled or not
 * io_pa_or_va() uses secure mapped IO memory if found or fallback to
 * non-secure mapped IO memory.
 */
vaddr_t io_pa_or_va_secure(struct io_pa_va *p);
vaddr_t io_pa_or_va_nsec(struct io_pa_va *p);
vaddr_t io_pa_or_va(struct io_pa_va *p);

#endif /* CORE_MEMPROT_H */
