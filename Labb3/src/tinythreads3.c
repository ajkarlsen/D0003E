#include <setjmp.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "tinythreads.h"

#define NULL            0
#define DISABLE()       cli()
#define ENABLE()        sei()
#define STACKSIZE       80
#define NTHREADS        4
#define SETSTACK(buf,a) *((unsigned int *)(buf)+8) = (unsigned int)(a) + STACKSIZE - 4; \
                        *((unsigned int *)(buf)+9) = (unsigned int)(a) + STACKSIZE - 4

struct thread_block {
    void (*function)(int);   // code to run
    int arg;                 // argument to the above
    thread next;             // for use in linked lists
    jmp_buf context;         // machine state
    char stack[STACKSIZE];   // execution stack space
};

struct thread_block threads[NTHREADS];

struct thread_block initp;

thread freeQ   = threads;
thread readyQ  = NULL;
thread current = &initp;

extern mutex blink_mutex;
extern mutex button_mutex;

volatile int system_ticks = 0;
int get_ticks(void) {
    return system_ticks;
}

int initialized = 0;

static void initialize(void) {
    int i;
    for (i=0; i<NTHREADS-1; i++)
        threads[i].next = &threads[i+1];
    threads[NTHREADS-1].next = NULL;

    // Functionality for joystick button interrupt
    /*PORTB = (1 << PB7);
    PCMSK1 = (1 << PCINT15); 
    EIMSK = (1 << PCIE1);
    */
    // Functionality for timer interrupt
    /*
    TCNT1 = 0; // Reset timer count
    OCR1A = 3906; // (8 000 000 / 1024) * 0,5 = 3906.25 --> 500ms interrupt interval
    TIMSK1 = (1 << OCIE1A); 
    TCCR1B = (1 << WGM12) | (1 << CS10) | (0 << CS11) | (1 << CS12);
    TCCR1A = (1 << COM1A1) | (1 << COM1A0);
    */

    initialized = 1;
}

/*
ISR(PCINT1_vect) {

   int pressed = !(PINB & (1 << PB7));
   static int BUTTON_PRESSED = 0;

	// Check if pressed NOW and if last state was not pressed
	if (pressed == 1 && BUTTON_PRESSED == 0) {
		yield();
		BUTTON_PRESSED = 1;
	}
	// Update last state
	else if (pressed == 0) {
		BUTTON_PRESSED = 0;
	}
}
*/

/*ISR(TIMER1_COMPA_vect) {
    unlock(&blink_mutex); // Signal the blink thread to toggle the LED
}*/

static void enqueue(thread p, thread *queue) {
    p->next = *queue;
    *queue = p;
}

static thread dequeue(thread *queue) {
    thread p = *queue;
    if (*queue) {
        *queue = (*queue)->next;
    } else {
        while (1) ;  
    }
    return p;
}

static void dispatch(thread next) {
    if (setjmp(current->context) == 0) {
        current = next;
        longjmp(next->context,1);
    }
}

void spawn(void (* function)(int), int arg) {
    thread newp;

    DISABLE();
    if (!initialized) initialize();

    newp = dequeue(&freeQ);
    newp->function = function;
    newp->arg = arg;
    newp->next = NULL;
    if (setjmp(newp->context) == 1) {
        ENABLE();
        current->function(current->arg);
        DISABLE();
        enqueue(current, &freeQ);
        dispatch(dequeue(&readyQ));
    }
    SETSTACK(&newp->context, &newp->stack);

    enqueue(current, &readyQ);
    dispatch(newp);
    
    ENABLE();
}

void yield(void) {
    DISABLE();
    enqueue(current, &readyQ);
    dispatch(dequeue(&readyQ));
    ENABLE();
}

void lock(mutex *m) {
    DISABLE();
    if (m->locked == 0) {
        m->locked = 1; // Lock if not already locked
    } else {
        enqueue(current, &m->waitQ); // add to mutex wait queue
        dispatch(dequeue(&readyQ));
    } 
    ENABLE();
}

void unlock(mutex *m) {
    DISABLE();
    if (m->waitQ != NULL) { // Check if there are waiting threads
        thread next = dequeue(&m->waitQ); // Take the next thread from mutexQ
        enqueue(current, &readyQ);
        dispatch(next); // Dispatch the waiting thread

    } else {
        m->locked = 0; // Unlock if no threads are waiting
    }
    ENABLE();
}

