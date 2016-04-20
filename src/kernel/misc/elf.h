#ifndef __ELF_H__
#define __ELF_H__

#include <stdint.h>

#define ELF_MAGIC 0x464C457FU /* "\x7FELF" in little endian */

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t  Elf32_Sword;
typedef uint32_t Elf32_Word;

struct elf32_header {
	Elf32_Word e_magic;	// elf magic, must be equal to `ELF_MAGIC'
	uint8_t e_elf[12];	// elf identification
	Elf32_Half e_type;	// object file type
	Elf32_Half e_machine;	// machine type
	Elf32_Word e_version;	// object file version
	Elf32_Addr e_entry;	// entry point address
	Elf32_Off e_phoff;	// program header offset
	Elf32_Off e_shoff;	// section header offset
	Elf32_Word e_flags;	// processor-specific flags
	Elf32_Half e_ehsize;	// elf header size
	Elf32_Half e_phentsize;	// size of program header entry
	Elf32_Half e_phnum;	// number of program header entries
	Elf32_Half e_shentsize;	// size of section header entry
	Elf32_Half e_shnum;	// number of section header entries
	Elf32_Half e_shstrndx;	// section name string table index
};

struct elf32_program_header {
	Elf32_Word p_type;	// type of segment
	Elf32_Off p_offset;	// offset in file
	Elf32_Addr p_va;	// virtual address in memory
	Elf32_Addr p_pa;	// reserved
	Elf32_Word p_filesz;	// size of segment in file
	Elf32_Word p_memsz;	// size of segment in memory
	Elf32_Word p_flags;	// segment attributes
	Elf32_Word p_align;	// aligment of segment
};

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

struct elf64_header {
	Elf64_Word e_magic;	// elf magic, must be equal to `ELF_MAGIC'
	uint8_t e_elf[12];	// elf identification
	Elf64_Half e_type;	// object file type
	Elf64_Half e_machine;	// machine type
	Elf64_Word e_version;	// object file version
	Elf64_Addr e_entry;	// entry point address
	Elf64_Off e_phoff;	// program header offset
	Elf64_Off e_shoff;	// section header offset
	Elf64_Word e_flags;	// processor-specific flags
	Elf64_Half e_ehsize;	// elf header size
	Elf64_Half e_phentsize;	// size of program header entry
	Elf64_Half e_phnum;	// number of program header entries
	Elf64_Half e_shentsize;	// size of section header entry
	Elf64_Half e_shnum;	// number of section header entries
	Elf64_Half e_shstrndx;	// section name string table index
};

struct elf64_program_header {
	Elf64_Word p_type;	// type of segment
	Elf64_Word p_flags;	// segment attributes
	Elf64_Off p_offset;	// offset in file
	Elf64_Addr p_va;	// virtual address in memory
	Elf64_Addr p_pa;	// reserved
	Elf64_Xword p_filesz;	// size of segment in file
	Elf64_Xword p_memsz;	// size of segment in memory
	Elf64_Xword p_align;	// aligment of segment
};

// p_type
#define ELF_PHEADER_TYPE_LOAD	1

// p_flags
#define ELF_PHEADER_FLAG_EXEC	0x1
#define ELF_PHEADER_FLAG_WRITE	0x2
#define ELF_PHEADER_FLAG_READ	0x4

#define ELF_HEADER(arch_)	arch_ ## _header
#define ELF_PHEADER(arch_)	arch_ ## _program_header

#define ELF_PHEADER_FIRST(eh_, arch_) ({		\
	void *ph_ = ((uint8_t *)eh_ + eh_->e_phoff);	\
	(struct ELF_PHEADER(arch_) *)ph_;		\
})

#define ELF_PHEADER_LAST(eh_, arch_) ({			\
	struct ELF_PHEADER(arch_) *fph_;		\
							\
	fph_ = ELF_PHEADER_FIRST(eh_, arch_);		\
	fph_ + eh_->e_phnum;				\
})

#define ELF64_PHEADER_FIRST(header_) ELF_PHEADER_FIRST(header_, elf64)
#define ELF64_PHEADER_LAST(header_) ELF_PHEADER_LAST(header_, elf64)

#define ELF32_PHEADER_FIRST(header_) ELF_PHEADER_FIRST(header_, elf32)
#define ELF32_PHEADER_LAST(header_) ELF_PHEADER_LAST(header_, elf32)

#endif // __ELF_H__
