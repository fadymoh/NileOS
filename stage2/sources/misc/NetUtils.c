#include "NetUtils.h"

void mac_str_to_bin(char *mac_str, uint8_t *mac_bin)
{
        uint64_t l = strlen(mac_str);
        uint8_t j = 0;
        for (uint64_t i = 0; i <= l - 2; i++)
        {
                mac_bin[j] = 0;
                if (mac_str[i + 2] == ':' || i + 2 == l)
                {
                        if (mac_str[i] >= '0' && mac_str[i] <= '9')
                                mac_bin[j] += 16 * (mac_str[i] - '0');
                        else if (mac_str[i] >= 'A' && mac_str[i] <= 'Z')
                                mac_bin[j] += 16 * (mac_str[i] - 'A' + 10);
                        else if (mac_str[i] >= 'a' && mac_str[i] <= 'z')
                                mac_bin[j] += 16 * (mac_str[i] - 'a' + 10);
                        i++;
                }
                if (mac_str[i + 1] == ':' || i + 1 == l)
                {
                        if (mac_str[i] >= '0' && mac_str[i] <= '9')
                                mac_bin[j] += (mac_str[i] - '0');
                        else if (mac_str[i] >= 'A' && mac_str[i] <= 'Z')
                                mac_bin[j] += (mac_str[i] - 'A' + 10);
                        else if (mac_str[i] >= 'a' && mac_str[i] <= 'z')
                                mac_bin[j] += (mac_str[i] - 'a' + 10);
                        i++;
                }
                j++;
        }
}