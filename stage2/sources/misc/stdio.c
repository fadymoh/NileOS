#include "stdio.h"

void sprintf_raw(char *str, const char *str_template, va_list args)
{
    uint64_t len = strlen(str_template);
    uint64_t x = 0;
    for (uint64_t i = 0; i < len;)
    {
        if (str_template[i] == '%')
        {
            if (str_template[i + 1] == 'c')
            {
                char c = va_arg(args, int);
                ((char *)(str + x))[0] = c;
                x++;
                i += 2;
            }
            else if (str_template[i + 1] == 's')
            {
                char *s = va_arg(args, char *);
                if (s != NULL)
                {
                    strcpy(str + x, s);
                    x += strlen(s);
                    i += 2;
                }
            }
            else if (str_template[i + 1] == 'd')
            {
                uint64_t divisor = 0x8AC7230489E80000;
                uint64_t decimal = va_arg(args, uint64_t);
                if (decimal == 0)
                {
                    str[x] = '0';
                    x++;
                }
                else
                {
                    bool add_zero = false;
                    for (; divisor > 0; divisor /= 10)
                    {
                        uint8_t v = decimal / divisor;
                        if (v > 0 || add_zero)
                        {
                            str[x] = v + '0';
                            x++;
                            add_zero = true;
                        }
                        decimal = decimal % divisor;
                    }
                }
                i += 2;
            }
            else if (str_template[i + 1] == 'x' || str_template[i + 1] == 'y')
            {
                bool l_no_zeros = true;
                uint64_t hexa = va_arg(args, uint64_t);
                if (hexa == 0)
                {
                    str[x] = '0';
                    x++;
                    str[x] = '0';
                    x++;
                }
                else
                {

                    if (str_template[i + 1] == 'y' && hexa < 0x10)
                    {
                        str[x] = '0';
                        x++;
                    }
                    for (uint8_t y = 60; y <= 60 && y >= 0; y -= 4)
                    {
                        uint8_t l_tmp = (hexa >> y) & 0xF;
                        if (l_tmp == 0 && l_no_zeros)
                            continue;
                        if (l_tmp >= 0xA)
                        {
                            str[x] = l_tmp - 0xA + 'a';
                            x++;
                            l_no_zeros = false;
                        }
                        else
                        {
                            str[x] = l_tmp + '0';
                            x++;
                            l_no_zeros = false;
                        }
                    }
                }
                i += 2;
            }
            else if (str_template[i + 1] == 'v' || str_template[i + 1] == 'z')
            {
                bool l_no_zeros = true;
                uint128_t hexa = va_arg(args, uint128_t);
                if (hexa == 0)
                {
                    str[x] = '0';
                    x++;
                    str[x] = '0';
                    x++;
                }
                else
                {
                    if (str_template[i + 1] == 'z' && hexa < 0x10)
                    {
                        str[x] = '0';
                        x++;
                    }
                    for (uint8_t y = 124; y <= 124 && y >= 0; y -= 4)
                    {
                        uint8_t l_tmp = (hexa >> y) & 0xF;
                        if (l_tmp == 0 && l_no_zeros)
                            continue;
                        if (l_tmp >= 0xA)
                        {
                            str[x] = l_tmp - 0xA + 'a';
                            x++;
                            l_no_zeros = false;
                        }
                        else
                        {
                            str[x] = l_tmp + '0';
                            x++;
                            l_no_zeros = false;
                        }
                    }
                }
                i += 2;
            }
            else
            {
                str[x] = str_template[i];
                i++;
                x++;
            }
        }
        else
        {
            str[x] = str_template[i];
            i++;
            x++;
        }
    }
}
void sprintf(char *str, const char *str_template, ...)
{
    va_list args;
    va_start(args, str_template);
    sprintf_raw(str, str_template, args);
    va_end(args);
}
