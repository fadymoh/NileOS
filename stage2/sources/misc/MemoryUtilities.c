#include "MemoryUtilities.h"
#include "Terminal.h"

char *strstr(char *p_haystack, const char *p_needle)
{
    uint64_t haystack_len = strlen(p_haystack);
    uint64_t needle_len = strlen(p_needle);
    char *haystack_end = p_haystack + haystack_len;
    char *temp = p_haystack;
    if (haystack_len < needle_len)
        return NULL;

    while (temp + needle_len <= haystack_end)
    {
        char *ptr = rmemcmp(temp, p_needle, needle_len);
        if (ptr == NULL)
            return temp;
        else if (ptr == temp)
            temp++;
        else if (ptr != NULL)
        {
            char *ptr1 = rscanc(p_needle, ptr[0], ptr - temp);
            if (ptr1 == NULL)
                temp += needle_len;
            else
                temp += strlen(ptr1) - 1;
        };
    }
    return NULL;
}

void *scanmem(void *p_haystack, const void *p_needle, uint64_t p_haystack_len, uint64_t p_needle_len)
{
    char *haystack_end = p_haystack + p_haystack_len;
    char *temp = p_haystack;
    if (p_haystack_len < p_needle_len)
        return NULL;
    while (temp + p_needle_len <= haystack_end)
    {
        char *ptr = rmemcmp(temp, p_needle, p_needle_len);
        if (ptr == NULL)
            return temp;
        else if (ptr == temp)
            temp++;
        else /*if ( ptr != NULL)*/
        {
            void *ptr1 = rscanc(p_needle, ptr[0], ptr - temp);
            if (ptr1 == NULL)
                temp++; //temp += p_needle_len;
            else
                temp += ((p_needle_len - (ptr1 - p_needle)) - 1);
        }
    }
    return NULL;
}

void strncat(char *p_dest, const char *p_src, uint64_t p_len)
{
    memcpy(p_dest + strlen(p_dest), p_src, p_len);
}

void strcat(char *p_dest, const char *p_src)
{
    memcpy(p_dest + strlen(p_dest), p_src, strlen(p_src) + 1);
}

void strncpy(char *p_dest, const char *p_src, uint64_t p_len)
{
    memcpy(p_dest, p_src, p_len);
}

void strcpy(char *p_dest, const char *p_src)
{
    memcpy(p_dest, p_src, strlen(p_src) + 1);
}

void *strcmp(char *p_dest, const char *p_src)
{
    uint64_t l_dest = strlen(p_dest);
    uint64_t l_src = strlen(p_src);
    uint64_t len = l_dest;
    if (l_src < l_dest)
        len = l_src;
    void *result = memcmp(p_dest, p_src, len);
    if (result != NULL)
        return result;
    if (l_src < l_dest)
        return p_dest + (len - 1);
    else if (l_src > l_dest)
        return ((char *)p_src) + (len - 1);
    else
        return NULL;
}

int lstrcmp(char *p_dest, const char *p_src)
{
    uint64_t l_dest = strlen(p_dest);
    uint64_t l_src = strlen(p_src);
    if (l_dest > l_src)
        return 1;
    else if (l_dest < l_src)
        return -1;
    uint64_t len = l_dest;
    char *diff = memcmp(p_dest, p_src, len);
    if (diff == NULL)
        return 0;
    else if (p_dest[diff - p_dest] > p_src[diff - p_dest])
        return 1;
    else
        return -1;
    /* uint128_t *s = (uint128_t *)p_src;
    uint128_t *d = (uint128_t *)p_dest;
    for ( i = 0 ;  len - i < 8; i += 8)
    {
        if ( *d > *s ) return 1;
        else if ( *s > *d) return -1;
        s++;
        d++;
    }
    for ( ; i < len ; i ++)
        if ( p_dest[i] >  p_src[i]) return 1;
        else if ( p_dest[i] <  p_src[i]) return -1;
    return 0;*/
}