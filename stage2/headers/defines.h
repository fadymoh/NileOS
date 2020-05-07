#ifndef DEFINES_H_
#define DEFINES_H_
//***************************** Video **************************************
enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

// Default Video screen sizes
#define DEFAULT_VIDEO_MAX_X 80
#define DEFAULT_VIDEO_MAX_Y 25
#define DEFAULT_TERMINAL_Y_ROWS 50

// VIDEO RAM address
#define VIDEO_RAM_BUFFER_ADDRESS 0x0B8000

// VGA cursor command and data ports
#define VGA_CURSOR_CMD_PORT 0x3D4
#define VGA_CURSOR_DATA_PORT 0x3D5
// VGA cursor commands
#define VGA_CURSOR_SET_LOW_BYTE_CMD 0x0F
#define VGA_CURSOR_SET_HI_BYTE_CMD 0x0E

// Services defines
#define MAX_SERVICES 255
#define MAX_FUNCTIONS 255

#define true 1
#define false 0
#define bool _Bool //! was char now its _Bool
#define NULL 0

//***************************** Physical Memory **************************************
#define E820_MEMORY_ENTRY_TYPE_USABLE 0x1
#define E820_MEMORY_ENTRY_TYPE_RESEARVED 0x2
#define E820_MEMORY_ENTRY_TYPE_ACPI_RECLAINMED 0x3
#define E820_MEMORY_ENTRY_TYPE_ACPI_NVS 0x4
#define E820_MEMORY_ENTRY_TYPE_BAD_MEMORY 0x5
#define E820_MEMORY_ENTRY_TYPE_HOLE 0x6

#define MAX_MEMORY_PHYSICAL_LAYOUT_ENTRIES 0x40

#define MEMORY_ENTRY_NOT_FOUND 10000
// Memory indicators string stored by the memory_scanner
#define E820_MEMORY_INDICATOR "MEMORY_INDICATOR"
// Size of the memory indicator
#define E820_MEMORY_INDICATOR_LENGTH 17
// Size of the physical memory layout storage buffer
#define E820_MEMORY_ENTRY_TABLE_SIZE 1024

//***************************** ACPI **************************************
// Root System Descriptor signature
#define RSD_SIGNATURE "RSD PTR "
#define APIC_TYPE_LOCAL_APIC 0
#define APIC_TYPE_IO_APIC 1
#define APIC_TYPE_INTERRUPT_OVERRIDE 2

//***************************** MEMORY MAP **************************************

#define KERNEL_MEMORY_MAP_ADDRESS 0x80000
#define FIRST_MEMORY_1_MiB 0x100000 // 1 MiB
#define USERMODE_CODE_ADDRESS 0x200000
#define USERMODE_SPACE_SIZE 0x200000 * 0x8
#define KERNEL_ADDRESS 0x100000
#define FILE_SYSTEM_ADDRESS USERMODE_CODE_ADDRESS + USERMODE_SPACE_SIZE //0x300000
#define FILE_SYSTEM_SIZE 0x1E00000										// 30 MB

#define KERNELKMALLOC_INITIAL_MEMORY_SIZE 0x25800000 // 600 MiB
//#define KERNELKMALLOC_INITIAL_MEMORY_SIZE         0x19000000       // 400 MiB
//#define KERNELKMALLOC_INITIAL_MEMORY_SIZE         0x6400000 *2       // 200 MiB
#define KERNELKMALLOC_INITIAL_MEMORY_ADDRESS FILE_SYSTEM_ADDRESS + FILE_SYSTEM_SIZE

#define MEMORY_PAGE_TABLE_ENTRY_PRESENT 0x1 // Page Present Flag ( bit 1 )
#define MEMORY_PAGE_TABLE_ENTRY_RW 0x2		// Page RW Flag  ( bit 2 )
#define MEMORY_PAGE_TABLE_ENTRY_USER 0x4	// Page USR Flag ( bit 3 )
#define MEMORY_PAGE_TABLE_ENTRY_PWT 0x8		// Page PWT Flag ( bit 4 )
#define MEMORY_PAGE_TABLE_ENTRY_PCD 0x10	// Page PCD Flag ( bit 5 )
#define MEMORY_PAGE_TABLE_ENTRY_2MIB 0x80   // 2 MiB Flag    ( bit 7 )

#define PAGE_TABLE_UNIT_SIZE 0x200 // 512
#define MAX_PMD_COUNT 0x10		   // 16
#define MAX_PTE_COUNT 0x200		   // 512
#define MIN_PTE_COUNT 0x2		   // 16
#define PGD_ENTRIES_LIMIT 0x4

#define ONE_GIG_VALUE 0x40000000		  // 1Gig
#define TWO_MiB_MEMORY_PAGE_SIZE 0x200000 // 2 MiB
#define FOUR_KiB_MEMORY_PAGE_SIZE 0x1000  // 4 KiB

#define KERNEL_MEMORY_SIZE 0x2EE00000 // 750 MiB (375 2Mib Entries)

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

//#define APIRQ0  64
//#define APIRQ1  65
//#define SWINT0  0x80
#define SYSCALL_INT 0x80

#define CORE_IPI 96
#define NDFS_IPI 97
#define IPIOE_IPI 98
#define WAKEUP_IPI 99
#define ATA_IPI 100

#define USER_IPI0 120
#define USER_IPI1 121
#define USER_IPI2 122
#define USER_IPI3 123
#define USER_IPI4 124
#define USER_IPI5 125
#define USER_IPI6 126
#define USER_IPI7 127

#define PIC1_COMMAND 0x20 // IO base address for master PIC
#define PIC2_COMMAND 0xA0 // IO base address for slave PIC
#define PIC1_DATA (PIC1_COMMAND + 1)
#define PIC2_DATA (PIC2_COMMAND + 1)

#define KERNEL_CODESEG 0x8
#define KERNEL_DATASEG 0x10
#define USER_CODESEG 0x18
#define USER_DATASEG 0x20

#define IDT_P_KERNEL_INTERRUPT_GATE 0x8E
#define IDT_P_USER_INTERRUPT_GATE 0xEE

// A macro for enabling all interrupts.
#define enableInterrupts() __asm__ __volatile__("sti" \
												:     \
												:     \
												: "memory");

// A macro for disabling all interrupts.
#define disableInterrupts() __asm__ __volatile__("cli" \
												 :     \
												 :     \
												 : "memory");

#define setStatus(x, status) x |= 1 << status
#define clearStatus(x, status) x &= ~(1 << status)
#define isStatus(x, status) ((x >> status) & 1)
#define setBit(value, bit) value |= 1 << bit
#define clearBit(value, bit) value |= 0 << bit
#define isBitSet(value, bit) (1 & (value >> bit))

#define TRAMPOLINE 0x0A

#define USER_STACK_SIZE_IN_PAGES 0x100
#define KERNEL_STACK_SIZE_IN_PAGES 0x24

#define MEM_PAGE_SIZE 0x1000

#define setBit64(value, bit) value[bit / 64] |= (1ULL << (bit % 64))
#define isBitSet64(value, bit) (1ULL & (value[bit / 64] >> (bit % 64)))

#define htons(A) ((((uint16_t)(A)&0xff00) >> 8) | \
				  (((uint16_t)(A)&0x00ff) << 8))
#define htonl(A) ((((uint32_t)(A)&0xff000000) >> 24) | \
				  (((uint32_t)(A)&0x00ff0000) >> 8) |  \
				  (((uint32_t)(A)&0x0000ff00) << 8) |  \
				  (((uint32_t)(A)&0x000000ff) << 24))
#define ntohs htons
#define ntohl htonl

typedef unsigned __int128 uint128_t;

#define SECTOR_SIZE 512

//#define DISK_SIG_SIZE 128
#define DISK_SIG_SIZE 128

// System Calls
#define SYSCALL_SLEEP 0x1
#define SYSCALL_SUSPEND 0x2
#define SYSCALL_OPEN_FILE 0x3
#define SYSCALL_READ_FILE 0x4
#define SYSCALL_SLEEP_TIMEOUT 0x5
#define SYSCALL_READ_FILE_BYTES 0x6
#define SYSCALL_WRITE_FILE 0x7

#define SYSCALL_TEST 0x10

#define SYSCALL_IPIOE_WAIT 0x40
#define SYSCALL_IPIOE_SEND 0x41
#define SYSCALL_IPIOE_WAIT_TIMEOUT 0x43
#define SYSCALL_IPIOE_REPLY 0x44
#define SYSCALL_IPIOE_TOUCH_PACKET 0x45
#define SYSCALL_IPIOE_WAIT_FOR_PACKET_ID 0x46
#define SYSCALL_IPIOE_GET_REPLY_FOR_ID 0x47

#define SYSCALL_NDFS_SEND 0x50
#define SYSCALL_NDFS_RECV 0x51
#define SYSCALL_NDFS_SEND_TO 0x52
#define SYSCALL_NDFS_CLOSE 0x53

#define SYSCALL_IS_MY_MAC 0x80
#define SYSCALL_GET_MAC_BY_SCOPE 0x81

#define SYSCALL_MALLOC 0x90
#define SYSCALL_VALLOC 0x91
#define SYSCALL_MEM_DUMP 0x92
#define SYSCALL_MEM_TOUCH 0x93
#define SYSCALL_MEM_UNTOUCH 0x94
// ATA Constants

#define MOUNT_POINT_SIZE 256

#define ATA_COUNT 0x04
#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170

#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376

#define ATA_MASTER_DRV_SELECTOR 0xA0
#define ATA_SLAVE_DRV_SELECTOR 0xB0

#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05

#define ATA_CMD_IDENTIFY 0xEC

#define ATA_REG_ALT_STATUS 0x0
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07
#define ATA_REG_ALTSTATUS 0x0C

#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01

#define BM0_COMMAND 0x00
#define BM0_RESV0 0x01
#define BM0_STATUS 0x02
#define BM0_RESV1 0x03
#define BM0_ADDRESS 0x04

#define BM1_COMMAND 0x08
#define BM1_RESV0 0x09
#define BM1_STATUS 0x0A
#define BM1_RESV1 0x0B
#define BM1_ADDRESS 0x0C

#define WRITE_PIO_SUCCESS 0x1
#define WRITE_PIO_FAIL 0x2
#define READ_PIO_SUCCESS 0x3
#define READ_PIO_FAIL 0x4
#define WRITE_DMA_SUCCESS 0x5
#define WRITE_DMA_FAIL 0x6
#define READ_DMA_SUCCESS 0x7
#define READ_DMA_FAIL 0x8

#define ETH_ALEN 6  /* Octets in one ethernet addr	 */
#define ETH_HLEN 14 /* Total octets in header.	 */
#define ETH_ZLEN 60 /* Min. octets in frame sans FCS */

#define ETH_DATA_LEN 1500							   /* Max. octets in payload	 */
#define ETH_FRAME_LEN ETH_DATA_LEN + 14				   /* Max. octets in frame sans FCS */
#define ETH_FRAME_LEN_WITH_TCP_SUDO ETH_FRAME_LEN + 86 /* Max. octets in frame sans FCS */

#define MAX_NETWORK_PACKET_PACK_COUNT 256

#define BITMAP_SIZE 180

#define SCOPE_IPIOE 0b00000001
#define SCOPE_GIPIOE 0b00000010
#define SCOPE_NDFS 0b00000100
#define SCOPE_IP 0b00001000
#define SCOPE_KSHELL 0b00010000

#define WAKEUP_MAPPER_COMMMAND 0x1

#define MAXIMUM_TAG_LENGTH 50

#endif
