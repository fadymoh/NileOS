/* 
 * File:        isr.h
 * Type:        Interfaces
 * Author:      Nile OS Team
 * Project:     best OS evaaaaa
 * Version:     30.0
 * Copyright:   2020
 *
 * Description:
 * This header file is responsible of defining the prototype definition of all 
 * external functions that are written in assembly and will be used to built
 * the IDT table. Also all the handlers written in C and will be called directly
 * from assembly interrupt handlers will be defined here
 *
 * 
 * 
 * 
 */


#ifndef ISR_H_
#define ISR_H_

#include "includes.h"
#include "InterruptContext.h"

// Handler functions written in C and will be called from the assembly
// interrupt handler routines.


extern  void IdtHandler(InterruptContext * p_interruptContext);


// 32 Exception Handlers of the hardware. 
extern  void isr0();
extern  void isr1();
extern  void isr2();
extern  void isr3();
extern  void isr4();
extern  void isr5();
extern  void isr6();
extern  void isr7();
extern  void isr8();
extern  void isr9();
extern  void isr10();
extern  void isr11();
extern  void isr12();
extern  void isr13();
extern  void isr14();
extern  void isr15();
extern  void isr16();
extern  void isr17();
extern  void isr18();
extern  void isr19();
extern  void isr20();
extern  void isr21();
extern  void isr22();
extern  void isr23();
extern  void isr24();
extern  void isr25();
extern  void isr26();
extern  void isr27();
extern  void isr28();
extern  void isr29();
extern  void isr30();
extern  void isr31();


// 64 IRQ handlers, the first 15 of which will be used by the PIC

extern  void irq0 ();
extern  void irq1 ();
extern  void irq2 ();
extern  void irq3 ();
extern  void irq4 ();
extern  void irq5 ();
extern  void irq6 ();
extern  void irq7 ();
extern  void irq8 ();
extern  void irq9 ();
extern  void irq10();
extern  void irq11();
extern  void irq12();
extern  void irq13();
extern  void irq14();
extern  void irq15();
extern  void irq16();
extern  void irq17();
extern  void irq18();
extern  void irq19();
extern  void irq20();
extern  void irq21();
extern  void irq22();
extern  void irq23();
extern  void irq24();
extern  void irq25();
extern  void irq26();
extern  void irq27();
extern  void irq28();
extern  void irq29();
extern  void irq30();
extern  void irq31(); 
extern  void irq32();
extern  void irq33();
extern  void irq34();
extern  void irq35();
extern  void irq36();
extern  void irq37();
extern  void irq38();
extern  void irq39();
extern  void irq40();
extern  void irq41();
extern  void irq42();
extern  void irq43();
extern  void irq44();
extern  void irq45();
extern  void irq46();
extern  void irq47();
extern  void irq48();
extern  void irq49();
extern  void irq50();
extern  void irq51();
extern  void irq52();
extern  void irq53();
extern  void irq54();
extern  void irq55();
extern  void irq56();
extern  void irq57();
extern  void irq58();
extern  void irq59();
extern  void irq60();
extern  void irq61();
extern  void irq62();
extern  void irq63();



// 32 handlers used to handle IPI (Inter-Processor INterrupts)

extern  void apirq0 ();
extern  void apirq1 ();
extern  void apirq2 ();
extern  void apirq3 ();
extern  void apirq4 ();
extern  void apirq5 ();
extern  void apirq6 ();
extern  void apirq7 ();
extern  void apirq8 ();
extern  void apirq9 ();
extern  void apirq10();
extern  void apirq11();
extern  void apirq12();
extern  void apirq13();
extern  void apirq14();
extern  void apirq15();
extern  void apirq16();
extern  void apirq17();
extern  void apirq18();
extern  void apirq19();
extern  void apirq20();
extern  void apirq21();
extern  void apirq22();
extern  void apirq23();
extern  void apirq24();
extern  void apirq25();
extern  void apirq26();
extern  void apirq27();
extern  void apirq28();
extern  void apirq29();
extern  void apirq30();
extern  void apirq31();


// 126 sw interrupt handlers used to implement different system calls

extern  void swirq0 ();
extern  void swirq1 ();
extern  void swirq2 ();
extern  void swirq3 ();
extern  void swirq4 ();
extern  void swirq5 ();
extern  void swirq6 ();
extern  void swirq7 ();
extern  void swirq8 ();
extern  void swirq9 ();
extern  void swirq10();
extern  void swirq11();
extern  void swirq12();
extern  void swirq13();
extern  void swirq14();
extern  void swirq15();
extern  void swirq16();
extern  void swirq17();
extern  void swirq18();
extern  void swirq19();
extern  void swirq20();
extern  void swirq21();
extern  void swirq22();
extern  void swirq23();
extern  void swirq24();
extern  void swirq25();
extern  void swirq26();
extern  void swirq27();
extern  void swirq28();
extern  void swirq29();
extern  void swirq30();
extern  void swirq31();
extern  void swirq32();
extern  void swirq33();
extern  void swirq34();
extern  void swirq35();
extern  void swirq36();
extern  void swirq37();
extern  void swirq38();
extern  void swirq39();
extern  void swirq40();
extern  void swirq41();
extern  void swirq42();
extern  void swirq43();
extern  void swirq44();
extern  void swirq45();
extern  void swirq46();
extern  void swirq47();
extern  void swirq48();
extern  void swirq49();
extern  void swirq50();
extern  void swirq51();
extern  void swirq52();
extern  void swirq53();
extern  void swirq54();
extern  void swirq55();
extern  void swirq56();
extern  void swirq57();
extern  void swirq58();
extern  void swirq59();
extern  void swirq60();
extern  void swirq61();
extern  void swirq62();
extern  void swirq63();
extern  void swirq64();
extern  void swirq65();
extern  void swirq66();
extern  void swirq67();
extern  void swirq68();
extern  void swirq69();
extern  void swirq70();
extern  void swirq71();
extern  void swirq72();
extern  void swirq73();
extern  void swirq74();
extern  void swirq75();
extern  void swirq76();
extern  void swirq77();
extern  void swirq78();
extern  void swirq79();
extern  void swirq80();
extern  void swirq81();
extern  void swirq82();
extern  void swirq83();
extern  void swirq84();
extern  void swirq85();
extern  void swirq86();
extern  void swirq87();
extern  void swirq88();
extern  void swirq89();
extern  void swirq90();
extern  void swirq91();
extern  void swirq92();
extern  void swirq93();
extern  void swirq94();
extern  void swirq95();
extern  void swirq96();
extern  void swirq97();
extern  void swirq98();
extern  void swirq99();
extern  void swirq100();
extern  void swirq101();
extern  void swirq102();
extern  void swirq103();
extern  void swirq104();
extern  void swirq105();
extern  void swirq106();
extern  void swirq107();
extern  void swirq108();
extern  void swirq109();
extern  void swirq110();
extern  void swirq111();
extern  void swirq112();
extern  void swirq113();
extern  void swirq114();
extern  void swirq115();
extern  void swirq116();
extern  void swirq117();
extern  void swirq118();
extern  void swirq119();
extern  void swirq120();
extern  void swirq121();
extern  void swirq122();
extern  void swirq123();
extern  void swirq124();
extern  void swirq125();
extern  void swirq126();


// A stub handler for IRQ 255 which does nothing
extern  void isr255();



#endif /* ISR_H_ */
