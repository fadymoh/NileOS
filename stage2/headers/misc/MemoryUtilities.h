#ifndef MEMORYUTILITIES_H_
#define MEMORYUTILITIES_H_
#include "defines.h"
#include "includes.h"

/********************** Memory copy functions set *****************************
 * 
 * Author: Karim Mohamed Sobh
 * Date: 27 June, 2016
 * 
 * A set of function for memory operations with different memory 
 * item types/sizes.
 * 
 * The functions implementation is done in x86_64 ASM for maximum performance.
 * 
 ******************************************************************************/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory copy operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


/******************************************************************************
 * 
 * Name: memcpy
 * Arguments:
 *          void * p_dest: memory location that data should be copied to
 *          void * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function does not
 *          handle overlapped memory locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          void * which makes the function portable with any memory pointer
 *          type through casting. The number of memory locations to be copied
 *          is defined in bytes, so for larger data types that are known ahead
 *          e.g. uint64_t, use the corresponding memcpy function from the 
 *          defined below with the size of the item as a suffix e.g. memcpy64
 * 
 ******************************************************************************/

extern void memcpy(void * p_dest,const void * p_src, uint64_t p_len);


/******************************************************************************
 * 
 * Name: memcpyc
 * Arguments:
 *          char * p_dest: memory location that data should be copied to
 *          char * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function does not
 *          handle overlapped memory locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          char * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in bytes, 
 *          and this function is a synonym of memcpy and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/


extern void memcpyc(char * p_dest,const char * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcpy8
 * Arguments:
 *          uint8_t * p_dest: memory location that data should be copied to.
 *          uint8_t * p_src: memory locations that data should be copied from.
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function does not
 *          handle overlapped memory locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint8_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in bytes, 
 *          and this function is a synonym of memcpy and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/


extern void memcpy8(uint8_t * p_dest,const uint8_t * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcpy16
 * Arguments:
 *          uint16_t * p_dest: memory location that data should be copied to.
 *          uint16_t * p_src: memory locations that data should be copied from.
 *          uint64_t p_len: number of two bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len two bytes from p_src memory  
 *          address location for p_dest memory address location. Two bytes are 
 *          copied at a time. The function does not handle overlapped memory 
 *          locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in two 
 *          bytes, and this function is a synonym of memcpy and it points to 
 *          the same assembly implementation.
 * 
 ******************************************************************************/

extern void memcpy16(uint16_t * p_dest,const uint16_t * p_src, uint64_t p_len);


/******************************************************************************
 * 
 * Name: memcpy32
 * Arguments:
 *          uint32_t * p_dest: memory location that data should be copied to.
 *          uint32_t * p_src: memory locations that data should be copied from.
 *          uint64_t p_len: number of four bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len two bytes from p_src memory  
 *          address location for p_dest memory address location. Four bytes are 
 *          copied at a time. The function does not handle overlapped memory 
 *          locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint32_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in two 
 *          bytes, and this function is a synonym of memcpy and it points to 
 *          the same assembly implementation.
 * 
 ******************************************************************************/

extern void memcpy32(uint32_t * p_dest,const uint32_t * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcpy64
 * Arguments:
 *          uint64_t * p_dest: memory location that data should be copied to.
 *          uint64_t * p_src: memory locations that data should be copied from.
 *          uint64_t p_len: number of eight bytes to be copied from p_src  
 *                          memory address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
  * Description:
 *          The function copies data of p_len two bytes from p_src memory  
 *          address location for p_dest memory address location. Eight bytes 
 *          are copied at a time. The function does not handle overlapped  
 *          memory locations.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint64_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in two 
 *          bytes, and this function is a synonym of memcpy and it points to 
 *          the same assembly implementation.
 * 
 ******************************************************************************/

extern void memcpy64(uint64_t * p_dest,const uint64_t * p_src, uint64_t p_len);



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory move operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
 * 
 * Name: memmove
 * Arguments:
 *          void * p_dest: memory location that data should be copied to
 *          void * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function handles
 *          overlapped memory locations. If source and destination memory
 *          regions do not overlap, it is more efficient to use the memcpy
 *          family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          void * which makes the function portable with any memory pointer
 *          type through casting. The number of memory locations to be copied
 *          is defined in bytes, so for larger data types that are know ahead
 *          e.g. uint64_t, use the corresponding memmove function from the 
 *          defined below with the size of the item as a suffix e.g. memmove64
 * 
 ******************************************************************************/


extern void memmove(void * p_dest,const void * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memmovec
 * Arguments:
 *          char * p_dest: memory location that data should be copied to
 *          char * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function handles
 *          overlapped memory locations. If source and destination memory
 *          regions do not overlap, it is more efficient to use the memcpy
 *          family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          char * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in bytes, 
 *          and this function is a synonym of memmove and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/

extern void memmovec(char * p_dest,const char * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memmove8
 * Arguments:
 *          uint8_t * p_dest: memory location that data should be copied to
 *          uint8_t * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len bytes from p_src memory address 
 *          location for p_dest memory address location. The function handles
 *          overlapped memory locations. If source and destination memory
 *          regions do not overlap, it is more efficient to use the memcpy
 *          family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint8_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in bytes, 
 *          and this function is a synonym of memmove and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/

extern void memmove8(uint8_t * p_dest,const uint8_t * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memmove16
 * Arguments:
 *          uint16_t * p_dest: memory location that data should be copied to
 *          uint16_t * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of two bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len two bytes from p_src memory 
 *          address location for p_dest memory address location. The function 
 *          handles overlapped memory locations. If source and destination 
 *          memory regions do not overlap, it is more efficient to use the 
 *          memcpy family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in two 
 *          bytes.
 * 
 ******************************************************************************/

extern void memmove16(uint16_t * p_dest,const uint16_t * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memmove32
 * Arguments:
 *          uint32_t * p_dest: memory location that data should be copied to
 *          uint32_t * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of four bytes to be copied from p_src memory 
 *                          address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len four bytes from p_src memory 
 *          address location for p_dest memory address location. The function 
 *          handles overlapped memory locations. If source and destination 
 *          memory regions do not overlap, it is more efficient to use the 
 *          memcpy family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint32_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in four 
 *          bytes.
 * 
 ******************************************************************************/


extern void memmove32(uint32_t * p_dest,const uint32_t * p_src, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memmove64
 * Arguments:
 *          uint64_t * p_dest: memory location that data should be copied to
 *          uint64_t * p_src: memory locations that data should be copied from
 *          uint64_t p_len: number of eight bytes to be copied from p_src 
 *                          memory address to p_dest memory address.
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function copies data of p_len eight bytes from p_src memory 
 *          address location for p_dest memory address location. The function 
 *          handles overlapped memory locations. If source and destination 
 *          memory regions do not overlap, it is more efficient to use the 
 *          memcpy family.
 * 
 *          The memory locations of the source and destination are defined as 
 *          uint64_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations to be copied is defined in eight 
 *          bytes.
 * 
 ******************************************************************************/

extern void memmove64(uint64_t * p_dest,const uint64_t * p_src, uint64_t p_len);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory initialization operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
 * 
 * Name: memset
 * Arguments:
 *          void * p_ptr: memory location of memory region to be initialized
 *          const char p_init: the byte value of the character to be used for 
 *                      initialization
 *          uint64_t p_len: number of bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len bytes starting from location pointed 
 *          to by  p_ptr memory address location, with the value of p_init.
 * 
 *          The memory location to be initialized is defined as void * which 
 *          makes the function portable with any memory pointer type through 
 *          casting. The number of memory locations to be initialized is 
 *          defined in bytes, so for larger data types that are known ahead
 *          e.g. uint64_t, use the corresponding memset function from the 
 *          defined below with the size of the item as a suffix e.g. memset64
 * 
 ******************************************************************************/

extern void memset(void * p_ptr, const char p_init, uint64_t p_len);


/******************************************************************************
 * 
 * Name: memsetc
 * Arguments:
 *          char * p_ptr: memory location of memory region to be initialized
 *          const char p_init: the byte value of the character to be used for 
 *                      initialization
 *          uint64_t p_len: number of bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len bytes starting from location pointed 
 *          to by  p_ptr memory address location, with the value of p_init.
 * 
 *          The memory locations to be initialized is defined as char * which 
 *          restricts the p_ptr argument to pointer of that type. The number of 
 *          memory locations to be initialized is defined in bytes and this 
 *          function is a synonym of memset and it points to the same assembly 
 *          implementation.
 * 
 ******************************************************************************/

extern void memsetc(char * p_ptr, const char p_init, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memset8
 * Arguments:
 *          uint8_t * p_ptr: memory location of memory region to be initialized
 *          const uint8_t p_init: the byte value of the character to be used 
 *                          for initialization.
 *          uint64_t p_len: number of bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len bytes starting from location pointed 
 *          to by  p_ptr memory address location, with the value of p_init.
 * 
 *          The memory locations to be initialized is defined as uint8_t * which 
 *          restricts the p_ptr argument to pointer of that type. The number of 
 *          memory locations to be initialized is defined in bytes and this 
 *          function is a synonym of memset and it points to the same assembly 
 *          implementation.
 * 
 ******************************************************************************/

extern void memset8(uint8_t * p_ptr, const uint8_t p_init, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memset16
 * Arguments:
 *          uint16_t * p_ptr: memory location of memory region to be initialized
 *          const uint16_t p_init: the uint16_t value of the character to be  
 *                          used for initialization.
 *          uint64_t p_len: number of two bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len two bytes starting from location  
 *          pointed to by p_ptr memory address location, with the value 
 *          of p_init.
 * 
 *          The memory locations to be initialized is defined as uint16_t * 
 *          which restricts the p_ptr argument to pointer of that type. The 
 *          number of memory locations to be initialized is defined in two 
 *          bytes.
 * 
 ******************************************************************************/

extern void memset16(uint16_t * p_ptr, const uint16_t p_init, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memset32
 * Arguments:
 *          uint16_t * p_ptr: memory location of memory region to be initialized
 *          const uint16_t p_init: the uint32_t value of the character to be  
 *                          used for initialization.
 *          uint64_t p_len: number of four bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len four bytes starting from location  
 *          pointed to by p_ptr memory address location, with the value 
 *          of p_init.
 * 
 *          The memory locations to be initialized is defined as uint32_t * 
 *          which restricts the p_ptr argument to pointer of that type. The 
 *          number of memory locations to be initialized is defined in four 
 *          bytes.
 * 
 ******************************************************************************/

extern void memset32(uint32_t * p_ptr, const uint32_t p_init, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memset64
 * Arguments:
 *          uint64_t * p_ptr: memory location of memory region to be initialized
 *          const uint64_t p_init: the uint64_t value of the character to be  
 *                          used for initialization.
 *          uint64_t p_len: number of two bytes to be initialized.
 * 
 * Return value:
 *      The function does not return any value and is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function initializes p_len eight bytes starting from location  
 *          pointed to by p_ptr memory address location, with the value 
 *          of p_init.
 * 
 *          The memory locations to be initialized is defined as uint64_t * 
 *          which restricts the p_ptr argument to pointer of that type. The 
 *          number of memory locations to be initialized is defined in eight 
 *          bytes.
 * 
 ******************************************************************************/

extern void memset64(uint64_t * p_ptr, const uint64_t p_init, uint64_t p_len);


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory comparison operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
 * 
 * Name: memcmp
 * Arguments:
 *          void * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const void * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the byte at which dissimilarity 
 *      occurs. The function is defined as extern, because it is implemented 
 *      externally in assembly.
 * 
 * Description:
 *          The function compares p_len bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          void * which makes the function portable with any memory pointer
 *          type through casting. The number of memory locations compared
 *          is defined in bytes, so for larger data types that are known ahead
 *          e.g. uint64_t, use the corresponding memcmp function from the 
 *          defined below with the size of the item as a suffix e.g. memcpy64
 * 
 ******************************************************************************/

extern void * memcmp(void * p_ptr1,const void * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcmpc
 * Arguments:
 *          char * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const char * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the byte at which dissimilarity 
 *      occurs. The function is defined as extern, because it is implemented 
 *      externally in assembly.
 * 
 * Description:
 *          The function compares p_len bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          char * which restricts the arguments to pointer of that type.
 *          The number of memory locations compared is defined in bytes, and 
 *          the function is a synonym of memcmp and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/

extern char * memcmpc(char * p_ptr1,const char * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcmp8
 * Arguments:
 *          uint8_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint8_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the byte at which dissimilarity 
 *      occurs. The function is defined as extern, because it is implemented 
 *      externally in assembly.
 * 
 * Description:
 *          The function compares p_len of bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint8_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations compared is defined in bytes, and 
 *          the function is a synonym of memcmp and it points to the same
 *          assembly implementation.
 * 
 ******************************************************************************/

extern uint8_t * memcmp8(uint8_t * p_ptr1,const uint8_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcmp16
 * Arguments:
 *          uint16_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint16_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the uint16_t at which  
 *      dissimilarity occurs. The function is defined as extern, because it is  
 *      implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len of two bytes at a time of the two 
 *          memory regions pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations compared is defined in uint16_t, 
 *          and the number of memory locations to be compared is defined in 
 *          two bytes.
 * 
 ******************************************************************************/

extern uint16_t * memcmp16(uint16_t * p_ptr1,const uint16_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcmp32
 * Arguments:
 *          uint32_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint32_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the uint32_t at which  
 *      dissimilarity occurs. The function is defined as extern, because it is  
 *      implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len of four bytes at a time  of the two 
 *          memory regions pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations compared is defined in uint32_t, 
 *          and the number of memory locations to be compared is defined in 
 *          four bytes.
 * 
 ******************************************************************************/

extern uint32_t * memcmp32(uint32_t * p_ptr1,const uint32_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: memcmp64
 * Arguments:
 *          uint64_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint64_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * 
 * Return value:
 *      The function returns a NULL  pointer if the two memory regions has the
 *      same data, else it returns a pointer to the uint64_t at which  
 *      dissimilarity occurs. The function is defined as extern, because it is  
 *      implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len of eight bytes at a time of the two 
 *          memory regions pointed to by p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type.
 *          The number of memory locations compared is defined in uint32_t, 
 *          and the number of memory locations to be compared is defined in 
 *          eight bytes.
 * 
 ******************************************************************************/

extern uint64_t * memcmp64(uint64_t * p_ptr1,const uint64_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmp
 * Arguments:
 *          void * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const void * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the char item of p_ptr1
 *      at which dissimilarity occurs. The function is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction 
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          void * which makes the function portable with any memory pointer
 *          type through casting. The number of memory locations compared
 *          is defined in bytes, so for larger data types that are known ahead
 *          e.g. uint64_t, use the corresponding rmemcmp function from the 
 *          defined below with the size of the item as a suffix e.g. rmemcpy64
 * 
 ******************************************************************************/

extern void * rmemcmp(void * p_ptr1,const void * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmpc
 * Arguments:
 *          char * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const char * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the char item of p_ptr1
 *      at which dissimilarity occurs. The function is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction  
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          char * which restricts the arguments to pointer of that type. 
 *          The number of memory locations compared is defined in bytes, and 
 *          this function is a synonym of rmemcmp and it points to the same 
 *          assembly implementation.
 * 
 ******************************************************************************/

extern char * rmemcmpc(char * p_ptr1,const char * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmp8
 * Arguments:
 *          uint8_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint8_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the uint8_t item of p_ptr1
 *      at which dissimilarity occurs. The function is defined as extern,
 *      because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction  
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint8_t * which restricts the arguments to pointer of that type. 
 *          The number of memory locations compared is defined in bytes, and 
 *          this function is a synonym of rmemcmp and it points to the same 
 *          assembly implementation.
 * 
 ******************************************************************************/

extern uint8_t * rmemcmp8(uint8_t * p_ptr1,const uint8_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmp16
 * Arguments:
 *          uint16_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint16_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the uint16_t item of 
 *      p_ptr1 at which dissimilarity occurs. The function is defined 
 *      as extern, because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len two bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction  
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint16_t * which restricts the arguments to pointer of that type. 
 *          The number of memory locations compared is defined in uint16_t, 
 *          and the comparison is performed on two bytes at a time
 * 
 ******************************************************************************/

extern uint16_t * rmemcmp16(uint16_t * p_ptr1,const uint16_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmp32
 * Arguments:
 *          uint32_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint32_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the uint32_t item of 
 *      p_ptr1 at which dissimilarity occurs. The function is defined 
 *      as extern, because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len four bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction  
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint32_t * which restricts the arguments to pointer of that type. 
 *          The number of memory locations compared is defined in uint32_t, 
 *          and the comparison is performed on four bytes at a time
 * 
 ******************************************************************************/


extern uint32_t * rmemcmp32(uint32_t * p_ptr1,const uint32_t * p_ptr2, uint64_t p_len);

/******************************************************************************
 * 
 * Name: rmemcmp64
 * Arguments:
 *          uint64_t * p_ptr1: memory location of the first data region for
 *                         memory comparison.
 *          const uint64_t * p_ptr2: memory location of the second data region 
 *                         for memory comparison.
 *          uint64_t p_len: number of bytes to be compared in memory regions  
 *                          pointed to by p_ptr1 and p_ptr2.
 * Return value:
 *      The function returns a NULL pointer if the two memory regions has 
 *      the same data, else it returns a pointer to the uint64_t item of 
 *      p_ptr1 at which dissimilarity occurs. The function is defined 
 *      as extern, because it is implemented externally in assembly.
 * 
 * Description:
 *          The function compares p_len eight bytes of the two memory regions 
 *          pointed to by p_ptr1 and p_ptr2. The comparison starts from the  
 *          end of the memory regions and move back in reverse direction  
 *          towards the pointers p_ptr1 and p_ptr2.
 * 
 *          The memory locations of the p_ptr1 and p_ptr2 are defined as 
 *          uint64_t * which restricts the arguments to pointer of that type. 
 *          The number of memory locations compared is defined in uint64_t, 
 *          and the comparison is performed on four bytes at a time
 * 
 ******************************************************************************/

extern uint64_t * rmemcmp64(uint64_t * p_ptr1,const uint64_t * p_ptr2, uint64_t p_len);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory scan operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
 * 
 * Name: scanc
 * Arguments:
 *          const char * p_ptr: memory location of memory region to be searched
 *          const char p_char: the byte value of the character to be used for 
 *                      the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a char * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of characters for the occurrence of 
 *          a p_char char item. The search starts from p_ptr uphill for a 
 *          number of p_len characters.
 * 
 ******************************************************************************/

extern char * scanc(const char * p_ptr,const char p_char,uint64_t p_len);


/******************************************************************************
 * 
 * Name: scan8
 * Arguments:
 *          const uint8_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint8_t p_uint8: the byte value of the character to be used 
 *                      for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint8_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of characters for the occurrence of 
 *          a p_char uint8_t item. The search starts from p_ptr uphill for a 
 *          number of p_len characters.
 * 
 ******************************************************************************/

extern uint8_t * scan8(const uint8_t * p_ptr,const uint8_t p_uint8,uint64_t p_len);

/******************************************************************************
 * 
 * Name: scan16
 * Arguments:
 *          const uint16_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint16_t p_uint16: the two bytes value of the character to be 
 *                      used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint16_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint16_t two bytes for the 
 *          occurrence of  a p_char uint16_t item. The search starts from  
 *          p_ptr uphill for a number of p_len uint16_t two bytes.
 * 
 ******************************************************************************/

extern uint16_t * scan16(const uint16_t * p_ptr,const uint16_t p_uint16,uint64_t p_len);

/******************************************************************************
 * 
 * Name: scan32
 * Arguments:
 *          const uint32_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint32_t p_uint32: the four bytes value of the character to   
 *                      be used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint32_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint32_t four bytes for the 
 *          occurrence of  a p_char uint32_t item. The search starts from  
 *          p_ptr uphill for a number of p_len uint32_t four bytes.
 * 
 ******************************************************************************/


extern uint32_t * scan32(const uint32_t * p_ptr,const uint32_t p_uint32,uint64_t p_len);

/******************************************************************************
 * 
 * Name: scan64
 * Arguments:
 *          const uint64_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint64_t p_uint64: the eight bytes value of the character to   
 *                      be used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint64_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint64_t four bytes for the 
 *          occurrence of a p_char uint64_t item. The search starts from  
 *          p_ptr uphill for a number of p_len uint64_t eight bytes.
 * 
 ******************************************************************************/

extern uint64_t * scan64(const uint64_t * p_ptr,const uint64_t p_uint64,uint64_t p_len);


/******************************************************************************
 * 
 * Name: rscanc
 * Arguments:
 *          const char * p_ptr: memory location of memory region to be searched
 *          const char p_char: the byte value of the character to be used for 
 *                      the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a char * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of characters for the occurrence of 
 *          a p_char char item. The search starts from p_ptr downhill in 
 *          reverse order from p_len bytes away from p_ptr heading towards 
 *          p_ptr.
 * 
 ******************************************************************************/

extern char * rscanc(const char * p_ptr,char p_char,uint64_t p_len);

/******************************************************************************
 * 
 * Name: rscan8
 * Arguments:
 *          const uint8_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint8_t p_uint8: the byte value of the character to be used 
 *                      for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint8_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of characters for the occurrence of 
 *          a p_char uint8_t item. The search starts from p_ptr downhill in 
 *          reverse order from p_len bytes away from p_ptr heading towards 
 *          p_ptr.
 * 
 ******************************************************************************/

extern uint8_t * rscan8(const uint8_t * p_ptr,uint8_t p_uint8,uint64_t p_len);

/******************************************************************************
 * 
 * Name: rscan16
 * Arguments:
 *          const uint16_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint16_t p_uint16: the two bytes value of the character to be 
 *                      used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint16_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint16_t two bytes for the 
 *          occurrence of  a p_char uint16_t item. The search starts from p_ptr  
 *          downhill in reverse order from p_len two bytes away from p_ptr  
 *           heading towards p_ptr.
 * 
 ******************************************************************************/


extern uint16_t * rscan16(const uint16_t * p_ptr,uint16_t p_uint16,uint64_t p_len);

/******************************************************************************
 * 
 * Name: rscan32
 * Arguments:
 *          const uint32_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint32_t p_uint32: the four bytes value of the character to   
 *                      be used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint32_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint32_t four bytes for the 
 *          occurrence of  a p_char uint32_t item. The search starts from p_ptr  
 *          downhill in reverse order from p_len four bytes away from p_ptr 
 *          heading towards p_ptr.
 * 
 ******************************************************************************/


extern uint32_t * rscan32(const uint32_t * p_ptr,uint32_t p_uint32,uint64_t p_len);

/******************************************************************************
 * 
 * Name: rscan64
 * Arguments:
 *          const uint64_t * p_ptr: memory location of memory region to be 
 *                                  searched
 *          const uint64_t p_uint64: the eight bytes value of the character to   
 *                      be used for the search
 *          uint64_t p_len: number of bytes to be searched starting from the 
 *                          memory location pointed to by p_ptr.
 * 
 * Return value:
 *      The function returns a uint64_t * to the location of the item that
 *      matches if it is found, else it returns NULL.
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 *  
 * Description:
 *          The function searches a memory region pointed to by p_ptr and 
 *          constrained by p_len number of uint64_t four bytes for the 
 *          occurrence of a p_char uint64_t item. The search starts from p_ptr  
 *          downhill in reverse order from p_len eight bytes away from p_ptr 
 *          heading towards p_ptr.
 * 
 ******************************************************************************/

extern uint64_t * rscan64(const uint64_t * p_ptr,uint64_t p_uint64,uint64_t p_len);



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * Memory string operations family
 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/******************************************************************************
 * 
 * Name: strstr
 * Arguments:
 *          char * p_haystack: a pointer to a NULL terminated string to be
 *                          searched for the p_needle NULL terminated string.
 *          const char * p_needle: a pointer to a NULL terminated string to
 *                          be searched for in the NULL terminated string 
 *                          p_haystack.
 * 
 * Return value:
 *      The function returns char * to the first occurrence of the NULL 
 *      terminated string pointed to by p_needle within the NULL terminated 
 *      string pointed to by p_haystack, or returns NULL if p_needle could not
 *      be found in p_haystack.
 *  
 * Description:
 *          The function searches a NULL terminated string  pointed to by 
 *          p_haystack for the first occurrence of the p_needed NULL terminated 
 *          string. The search algorithm used in this function is Boyer-Moore
 *          string search algorithm with bad character rule.
 * 
 ******************************************************************************/

char * strstr (char * p_haystack,const char * p_needle);

/******************************************************************************
 * 
 * Name: scanmem
 * Arguments:
 *          void * p_haystack: a pointer to a memory region to be
 *                          searched for the content of the memory region 
 *                          pointed to by p_needle.
 *          const void * p_needle: a pointer to a memory region whose content 
 *                          to be searched for in the memory region pointed 
 *                          to by p_haystack.
 *          uint64_t p_haystack_len: number of bytes defining the size of the 
 *                          p_haystack memory region.
 *          uint64_t p_needle_len: number of bytes defining the size of the 
 *                          p_needle memory region.
 * Return value:
 *      The function returns void * to the first occurrence of the content of 
 *      the memory region pointed to by p_needle within the memory region 
 *      pointed to by p_haystack, or returns NULL if p_needle could not
 *      be found in p_haystack.
 *  
 * Description:
 *          The function searches the memory region pointed to by p_haystack 
 *          for the first occurrence of the content of the memory region 
 *          pointed to by p_needed string. The search algorithm used in this 
 *          function is Boyer-Moore string search algorithm with bad character 
 *          rule.
 * 
 ******************************************************************************/


void * scanmem (void * p_haystack,const void * p_needle,uint64_t p_haystack_len,uint64_t p_needle_len);

/******************************************************************************
 * 
 * Name: strlen
 * Arguments:
 *          const char * p_ptr: memory location of the NULL terminated string 
 *                          whose length should be returned by the function.
 * 
 * Return value:
 *      The function returns a uint64_t integer value returning the length,
 *      number of chars, in the NULL terminated string pointed to by p_ptr. 
 *      The function is defined as extern, because it is implemented 
 *      externally in assembly.
 * 
 * Description:
 *          The function returns the length of a NUL terminated string pointed
 *          to by p_ptr. The function is defined as extern, because it is
 *          implemented externally in assembly.
 * 
 ******************************************************************************/

extern uint64_t strlen(const char * p_ptr);

void strncat (char * p_dest,const char * p_src, uint64_t p_len);
void strcat (char * p_dest,const char * p_src);

void strncpy (char * p_dest,const char * p_src, uint64_t p_len);
void strcpy (char * p_dest,const char * p_src);
void * strcmp (char * p_dest,const char * p_src);
int lstrcmp (char * p_dest,const char * p_src);
extern int strcmp_sse42(char * p_dest,const char * p_src);
extern uint64_t strlen_sse42(char * p_dest);
#endif //MEMORYUTILITIES_H_