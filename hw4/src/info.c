#include "includes/info.h"

#define CMDLINE_IS_VALID  0x04
#define MMAP_IS_VALID     0x40
#define MAGIC_POINTER     0x40E
#define EBDA_FIND_LENGTH  1024
#define MEM_REG_BEGIN     0x000E0000
#define MEM_REG_END       0x000FFFFF

void about_cmdline(multiboot_info_t *mis)
{
    if(mis->flags & CMDLINE_IS_VALID)
    {
        printf("cmdline: %s\n", (const char*) mis->cmdline);
    }
    else
    {
        printf("cmdline not available\n");
    }
}

void about_mmap(multiboot_info_t *mis)
{
    if(mis->flags & MMAP_IS_VALID)
    {
        const uint32_t last_addr = mis->mmap_addr + mis->mmap_length;
        uint32_t* upper_bytes;
        uint32_t* down_bytes;
        uint64_t end_addr;
        mmap_info_t* mmap_info_prt = (mmap_info_t*)mis->mmap_addr;
        while((uint32_t)mmap_info_prt < last_addr)
        {
            down_bytes = (uint32_t*)&(mmap_info_prt->base_addr);
            upper_bytes = down_bytes + 1;
            printf("memory-range: 0x%x%x - ", *upper_bytes, *down_bytes);

            end_addr = mmap_info_prt->base_addr + mmap_info_prt->length - 1;
            down_bytes = (uint32_t*)&(end_addr);
            upper_bytes = down_bytes + 1;
            printf("0x%x%x, type %d\n", *upper_bytes, *down_bytes, mmap_info_prt->type);

            mmap_info_prt = (mmap_info_t*)
                ((uint32_t)mmap_info_prt + (mmap_info_prt->size + 4));
        }
    }
    else
    {
        printf("mmap not available\n");
    }
}

struct rsdp_descriptor {
 char signature[8];
 uint8_t checksum;
 char oemid[6];
 uint8_t revision;
 uint32_t rsdt_address;
} __attribute__ ((packed));
typedef struct rsdp_descriptor rsdp_descriptor_t;

void about_apic()
{
    uint8_t is_ebda_successed;
    uint8_t is_mem_reg_successed;
    uint16_t* ebda_addr = (uint16_t*)MAGIC_POINTER;
    uint32_t ebda_value = *ebda_addr;
    char* ebda_ptr = (char*)ebda_value;
    printf("%x\n", *ebda_ptr);
    char* end_addr = ebda_ptr + EBDA_FIND_LENGTH;
    while(ebda_ptr < end_addr)
    {
        if(*ebda_ptr == 'R'){
            printf("FOUND!!!\n");
        }
        ++ebda_ptr;
    }

    rsdp_descriptor_t* desc_ptr = 0;
    char* mem_ptr = (char*)MEM_REG_BEGIN;
    char* mem_end = (char*)MEM_REG_END;
    while(mem_ptr < mem_end)
    {
        if( *mem_ptr       == 'R' && 
            *(mem_ptr + 1) == 'S' && 
            *(mem_ptr + 2) == 'D' && 
            *(mem_ptr + 3) == ' ' && 
            *(mem_ptr + 4) == 'P' && 
            *(mem_ptr + 5) == 'T' && 
            *(mem_ptr + 6) == 'R' && 
            *(mem_ptr + 7) == ' ')
        {
            desc_ptr = (rsdp_descriptor_t*) mem_ptr;
            break;
        }
        ++mem_ptr;
    }

    if(desc_ptr == 0)
    {
        printf("descriptor not found :(\n");
    }
    else
    {
        printf("descriptor found :)\n");
        printf("Rsdt address = 0x%x\n", desc_ptr->rsdt_address);
    }
}
