/*
Copyright (c) 2015 Intel Corporation.  All right reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
#include "conf.h"
#include "aux_regs.h"
#include "interrupt.h"

struct _IsrTableEntry
{
    void *arg;
    void (*isr)(void *);
};

struct _IsrTableEntry __attribute__((section(".data"))) _IsrTable[_WRS_CONFIG_NUM_IRQS];

static void _dummy_isr(void *arg)
{
    for(;;);
}

void interrupt_connect(unsigned int irq,
		       void (*isr)(void *arg),
		       void *arg)
{
    int index = irq - 16;
    unsigned int flags = interrupt_lock();
    _IsrTable[index].isr = isr;
    _IsrTable[index].arg = arg;
    interrupt_unlock(flags);
}

void interrupt_disconnect(unsigned int irq)
{
    int index = irq - 16;
    _IsrTable[index].isr = _dummy_isr;
}

void interrupt_enable(unsigned int irq)
{
    unsigned int flags = interrupt_lock();
    aux_reg_write (ARC_V2_IRQ_SELECT, irq);
    aux_reg_write (ARC_V2_IRQ_PRIORITY, 1);
    aux_reg_write (ARC_V2_IRQ_TRIGGER, ARC_V2_INT_LEVEL);
    aux_reg_write (ARC_V2_IRQ_ENABLE, ARC_V2_INT_ENABLE);
    interrupt_unlock(flags);
}

void interrupt_disable(unsigned int irq)
{
    unsigned int flags = interrupt_lock();
    aux_reg_write (ARC_V2_IRQ_SELECT, irq);
    aux_reg_write (ARC_V2_IRQ_ENABLE, ARC_V2_INT_DISABLE);
    interrupt_unlock(flags);
}

void interrupt_priority_set (int irq, unsigned char priority)
{
    unsigned int flags = interrupt_lock();
    aux_reg_write (ARC_V2_IRQ_SELECT, irq);
    aux_reg_write (ARC_V2_IRQ_PRIORITY, priority);
    interrupt_unlock(flags);
}

void interrupt_unit_device_init(void)
{
    int irq_index;
    for (irq_index = 16; irq_index < 256; irq_index++)
    {
        aux_reg_write(ARC_V2_IRQ_SELECT, irq_index);
        aux_reg_write(ARC_V2_IRQ_PRIORITY, 1);
        aux_reg_write(ARC_V2_IRQ_ENABLE, ARC_V2_INT_DISABLE);
        aux_reg_write(ARC_V2_IRQ_TRIGGER, ARC_V2_INT_LEVEL);
    }
}
