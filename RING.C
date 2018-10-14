
/* **********************************************************************
   * Ring.C - Fredric L. Rice, October 1994.                            *
   * The Skeptic Tank, 1:102/890.0.  (818) 335-9601  24 hours  9600bts  *
   *                                                                    *
   ********************************************************************** */

#include <bios.h>
#include <dos.h>
#include <stdio.h>
#include <process.h>
#include <string.h>

/* **********************************************************************
   * Any defines?                                                       *
   *                                                                    *
   ********************************************************************** */

#define TRUE            1
#define FALSE           0
#define USHORT          unsigned short
#define TONE            220
#define SIGNATURE       0x42

/* **********************************************************************
   * Define the errorlevel return values.                               *
   *                                                                    *
   ********************************************************************** */
   
#define No_Problem              0
#define Already_Installed       10
#define Invalid_Com_Port        11

/* **********************************************************************
   * Define function prototypes                                         *
   *                                                                    *
   ********************************************************************** */

    void interrupt (*old_isr)();

/* **********************************************************************
   * Any local data space?                                              *
   *                                                                    *
   ********************************************************************** */

    static USHORT inside = 0;
    static USHORT count = 0;
    static USHORT ringing = FALSE;
    static USHORT com_port = SIGNATURE;

/* **********************************************************************
   * The new interrupt.  Execute our ISR and then chain to the old one. *
   *                                                                    *
   * Stuff forwards into the stack where the AL register lives and put  *
   * our signature into it so that we know we're running.               *
   *                                                                    *
   ********************************************************************** */
   
void interrupt new_int_1c(void)
{
    USHORT result, byte;
    char *stack;

    if (inside == 0) {
        inside = 1;
        count++;

        if (count == 4) {
            count = 0;

            result = bioscom(3, byte, com_port);

            if (ringing) {
                if ((result & 0x40) == 0x00) {
                    ringing = FALSE;
                    nosound();
                }
            }
            else {
                if ((result & 0x40) == 0x40) {
                    ringing = TRUE;
                    sound(TONE);
                }
            }
        }

        inside = 0;
    }

/*
 * Execute the old ISR
 */

    (old_isr)();

/*
 * Plug the signature into the unused AL register.  The AL
 * register is stored 18 bytes backwards into our stack.
 *
 * Add the com port assignment to it so that we can display
 * what COM port it's already installed on.
 */

    stack = (char *)&result;
    stack += 18;
    *stack = SIGNATURE + com_port;
}

/* **********************************************************************
   * The main entry point.                                              *
   *                                                                    *
   ********************************************************************** */

void main(USHORT argc, char *argv[])
{
    USHORT loop;
    union REGS inregs, outregs;

/*
 * Give a sample of the tone
 */

    sound(TONE);
    delay(100);
    nosound();

/*
 * See if we're already loaded
 */

    int86(0x1c, &inregs, &outregs);

    if (outregs.h.al >= SIGNATURE && outregs.h.al <= SIGNATURE + 3) {

        (void)printf("Ring already installed on :COM%d\n",
            outregs.h.al - SIGNATURE);

        fcloseall();
        exit(Already_Installed);
    }

/*
 * Find out which com port to load onto
 */

    for (loop = 1; loop < argc; loop++) {
        if (! strnicmp(argv[loop], "/com", 4)) {
            com_port = argv[loop][4] - '0';
        }
    }

/*
 * Load it if it's valid
 */

    if (com_port < 0 || com_port > 3) {
        (void)printf("Offer either of these: /com0   /com1   /com2  /com3\n");
        (void)fcloseall();
        exit(Invalid_Com_Port);
    }

/*
 * Display what we're going to do
 */

    (void)printf("Ring installed on :COM%d\n", com_port);

/*
 * Get the old vector, set the new, and then terminate with resident
 */

    old_isr = getvect(0x1c);
    setvect(0x1c, new_int_1c);
    keep(No_Problem, (_SS + (_SP / 16) - _psp));
}


