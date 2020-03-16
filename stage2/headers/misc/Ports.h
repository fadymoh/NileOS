#ifndef PORTS_H_
#define PORTS_H_
#include "defines.h"
#include "includes.h"


void outportb (uint16_t p_port,uint8_t p_data);
void outportw (uint16_t p_port,uint16_t p_data);
void outportl (uint16_t p_port,uint32_t p_data);
uint8_t inportb( uint16_t p_port);
uint16_t inportw( uint16_t p_port);
uint32_t inportl( uint16_t p_port);

#endif