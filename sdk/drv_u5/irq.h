#ifndef IRQ_H_INCLUDED
#define IRQ_H_INCLUDED

#include "type.h"

#ifndef  PLATFORM_SETUP_H
  #error "platform_setup not included"
#endif // PLATFORM_SETUP_H

#ifndef HARDWARE_H
  #error "hardware not included"
#endif // HARDWARE_H

#define IRQ_SVC_SET_PRIO    1
#define IRQ_SVC_PRIV_SWITCH 2
#define IRQ_SVC_WDT         3

#define IRQ_SVC_FN(id, ...) __SVC_##id (__VA_ARGS__)

#define IRQ_SVC(id, name, ...) \
    void __svc(id) name(__VA_ARGS__);  \
    void IRQ_SVC_FN(id, __VA_ARGS__)

#define IRQ_SVC_RET(ret, id, name, ...) \
    ret __svc(id) name(__VA_ARGS__);  \
    ret IRQ_SVC_FN(id, __VA_ARGS__)

void irq_enable(IRQn_Type IRQn, s32 prio);

#endif // ! IRQ_H_INCLUDED

