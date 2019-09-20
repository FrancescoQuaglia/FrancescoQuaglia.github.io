#ifndef _CONTROL_H
#define _CONTROL_H

#define MODNAME "LW SCHEDULER"

int setup_my_irq(void);

void cleanup_my_irq(void);

typedef struct _elem{
        struct pt_regs *context;
        int ticks;
        struct _elem * next;
        struct _elem * prev;
} elem;


#endif /* IBS_CONTROL_H */
