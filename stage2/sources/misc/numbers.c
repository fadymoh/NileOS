#include "numbers.h"
#include "MemoryUtilities.h"

bool is_integer(char * number)
{
    uint64_t len = strlen (number);
    if ( len == 0) return false;
    for (uint64_t i = 0 ; i < len ; i++)
        if ( number[i] < '0' ||  number[i] > '9' ) return false;
    return true;
}


uint64_t atoi (const char * str)
{
    uint64_t res = 0;
    uint16_t i = 0;
    for ( ; str[i] == ' '; i++);
    
    for ( ; str[i] != '\0'; i++)
    {
        if( str[i] < '0' || str[i] > '9') 
            return 0;
        res = (res*10)+(str[i]-'0');
    }
    
    return res;
}
