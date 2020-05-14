// #include "HashTable.h"
// #include "Kernel.h"
// /* this function gives a unique hash code to the given key */

// extern Kernel kernel;

// uint64_t hashcode(uint64_t capacity, char *key)
// {
//     const int p = 31;
//     const int m = capacity;
//     uint64_t hash_value = 0;
//     uint64_t p_pow = 1;
//     uint32_t keyLength = strlen(key);

//     for (int i = 0; i < keyLength; ++i)
//     {
//         hash_value = (hash_value + (key[i] - 'a' + 1) * p_pow) % m;
//         p_pow = (p_pow * p) % m;
//     }

//     return hash_value;
// }

// /* it returns prime number just greater than array capacity */

// int get_prime(int n)
// {
//     if (n % 2 == 0)
//     {
//         n++;
//     }

//     for (; !if_prime(n); n += 2)
//         ;

//     return n;
// }

// /* to check if given input (i.e n) is prime or not */

// int if_prime(int n)
// {
//     int i;
//     if (n == 1 || n == 0)
//     {
//         return 0;
//     }
//     for (i = 2; i < n; i++)
//     {
//         if (n % i == 0)
//         {
//             return 0;
//         }
//     }

//     return 1;
// }

// void init_array(GlobalHashTable *table)
// {
//     int i;
//     int globalCapacity;
//     int IPCapacity = 10;
//     int MACCapacity = 10;
//     globalCapacity = get_prime(table->capacity);
//     IPCapacity = get_prime(IPCapacity);
//     MACCapacity = get_prime(MACCapacity);

//     table->capacity = globalCapacity;

//     // allocated memory for several serviceDataIdentifiers
//     table->array = (ServiceDataIdentifier *)kmalloc(&kernel.memoryAllocator, (globalCapacity) * sizeof(ServiceDataIdentifier));
//     printk("globalCapacity = %d\n", globalCapacity);

//     printk("going into the loop\n");
//     for (i = 0; i < globalCapacity; i++)
//     {
//         // go to the ServiceDataIdentifier and initialize it to zeros
//         table->array[i].key = 0;
//         table->array[i].value = NULL;
//         table->array[i].value->capacity = IPCapacity;

//         table->array[i].value->array = (IPIdentifier *)kmalloc(&kernel.memoryAllocator, (IPCapacity) * sizeof(IPIdentifier));
//         IPIdentifier *IPTable = table->array[i].value->array;
//         for (int j = 0; j < IPCapacity; ++j)
//         {
//             IPTable->coreCount[j] = 0;
//             IPTable->MACAddress[j][0] = 0;
//             uint16_t maxCoresCount = (1 << 8);
//             for (int k = 0; k < maxCoresCount; ++k)
//                 IPTable->coreID[j][k] = -1;
//         }
//     }
// }

// /* to insert a key in the hash table */

// void insert(GlobalHashTable *table, char *serviceName, char *IP, char *MAC, IPIdentifier *value)
// {
//     uint64_t globalIndex = hashcode(table->capacity, serviceName);

//     if (table->array[globalIndex].value == NULL)
//     {
//         /*  key not present, insert it  */
//         table->array[globalIndex].key = serviceName;

//         int ipCapacity = table->array[globalIndex].value->capacity;
//         for (int i = 0; i < ipCapacity; ++i)
//             // check for ip address
//         table->size++;
//         printk("\n Key (%s) has been inserted in location %d\n", key, index);
//     }

//     else if (strcmp(table->array[index].key, key))
//     {
//         /*  updating already existing key  */
//         printk("\n Key (%s) already present, hence updating its value \n", key);
//         table->array[index].value = value;
//     }
//     else
//     {
//         /*  key cannot be insert as the index is already containing some other key  */
//         printk("\n ELEMENT CANNOT BE INSERTED! COLLISION\n");
//     }
// }

// /* to remove a key from hash table */
// RegistryServiceValue *GetElementFromHashTable(GlobalHashTable *table, char *key)
// {
//     uint64_t index = hashcode(table, key);
//     RegistryServiceValue *value = NULL;
//     if (table->array[index].value != NULL)
//         value = table->array[index].value;

//     return value;
// }

// /* to display all the elements of a hash table */
// void display(GlobalHashTable *table)
// {
//     int i;
//     for (i = 0; i < table->capacity; i++)
//     {
//         if (table->array[i].key != 0)
//         {
//             //printk(" and value: coreCount: %d service: %s \n", , );
//             printk("\n array[%d] has elements -:\n", i);
//             printk(" key(%s)", table->array[i].key);
//             // printk(" and value: coreCount: %d", table->array[i].value->core_count);
//             // printk(" service: %s \n", table->array[i].value->service);
//         }
//     }
// }