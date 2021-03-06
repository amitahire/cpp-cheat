/*
Signals are a simple way for processes to communicate.

Signals are limited to passing a single byte between two processes.

What almost each signal does is all predefined by ANSI C
or specific operating systems

# Example: control + c

    An example most Linux people have used directly already:
    hit Ctrl + C in a terminal and this will send an INT signal
    to the running process.

    Unless the process implements a handler for this process,
    this has the effect of destroying the process,
    which is what happens by default for most, but not all signals.

# ANSI C

    There is a small ANSI C signal interface,
    and individual operating systems may implement many more
    which is the case for Linux.

    6 signals defined:

    # SIGABRT

        Abort.

        Abnormal termination, such as is initiated by the `abort` function.

    # SIGFPE

        Floating-Point Exception.

        Erroneous arithmetic operation such as division by zero divide
        or an operation resulting in overflow.

        Unlike the name suggests, it is not necessarily generated by a floating-point operation:
        it may also happen on an integer operation.

        Floating point division by 0 `1.0 / 0.0` does not generate exceptoins since it is defined by IEEE:
        http://stackoverflow.com/questions/7267838/division-by-zero-does-not-throw-sigfpe

        Integer division by 1 / 0 may generate a `SIGFPE`.

        1 / 0 is undefined behaviour. On x86, it generates a Divide Error,
        which Linux delivers as a SIGFPE.

        TODO how to deal with it? it just keeps coming back time after time.

        Very similar question, but his behaviour is not exactly the same as mine:
        http://stackoverflow.com/questions/14905947/what-does-c-c-handler-sigfpe

    # SIGILL

        Illegal Instruction.

        Things that can generate this on Linux x86:

        - making an assembly operation that is only legal from kernel mode on user mode,
            such as modify cs, ds or ss registers from a user program.

    # SIGSEGV

        Segmentation Violation.

        Invalid access to storage: When a program tries to read or write
        outside on an address and the OS does not like it because it is using too much memory.

    # SIGINT

        Interrupt

        Ask the process to terminate, possibly nicely.

        Sample cause: Linux terminal Ctrl + C.

    # SIGTERM

        Terminate.

        Termination request sent to program. Major cause: shutdown, window close.

        The only difference between `SIGINT` and `SIGTERM` is that `SIGINT` is meant to be generated manually by users
        from the terminal, while `SIGTERM` is meant to be generated by other applications.

    Functions defined (handlers):

    # SIG_DFL

        Default handling: The signal is handled by the default action for that particular signal.

    # SIG_IGN

        Ignore Signal: The signal is ignored.

    # SIG_ERR

        Special return value indicating failure.

# raise

    TODO

# abort

    TODO

# Allowed opeartions on signal handlers

    TODO can any operation be done in a signal handler (print to stdout for example?)

# sig_atomic_t

    TODO

# Sources

-   http://www.alexonlinux.com/signal-handling-in-linux

    good intro, covers lots of POSIX / linux specific stuff,
    but also has nice examples that help understand the ANSI C model.
*/


#include "common.h"

/* This is the only type of global that can be modified from the signal handler:
 * https://stackoverflow.com/questions/15500844/signal-handler-wont-see-global-variable/15502073#15502073 */
volatile sig_atomic_t global = 0;

void signal_handler(int sig) {
    /*
     * Only a very limited set standard library functions (async-signal-safe)
     * may be called from signal handlers, and printf is not one of them.
     *
     * In POSIX, we can use write(STDOUT_FILENO, but still there is
     * no way to format the string apparently.
     *
	 * - https://stackoverflow.com/questions/16891019/how-to-avoid-using-printf-in-a-signal-handler
	 * - https://stackoverflow.com/questions/14573000/print-int-from-signal-handler-using-write-or-async-safe-functions
     * */
	/*printf("sig: %d\n", sig);*/

	global = 1;
    /* After the signal is dealt with, the handler is then changed to its default action
     * if you want to continue using this handler for future signals, you have to reregister. */
	signal(sig, signal_handler);
}

int main(void) {

    /* TODO: why don't we handle SIGABRT? */
    signal(SIGABRT, signal_handler);
	/*abort();*/
    /*assert(i == 1);*/

    /* # Floating point exception
     *
     * TODO why hangs? */
	{
		signal(SIGFPE, signal_handler);
		int i = 0;
		int j = 0;
		/* cannot do 1 / 0 or the compiler will give a warning. Lets dupe him: */
		j = 1 / i;
		/* you need this `printf` or the compiler may optimize your division away */
		printf("%d", j);
		assert(global == 1);
	}

    /*
    int i = 0;
    while (i < 10) {
        printf( "%d\n", i );
        i++;

        TODO how to wait here for say 1 sec, so that user can try signals out?
        there is no simple ANSI way of doing that...

        on posix we would:
        //sleep( 1 );
    }
    */

    /*
    TODO ctrl + c on getchar does not work:

        puts( "press any key to exit" );
        signal(SIGINT, signal_handler);
        getchar();

    nor does this:

        puts( "press any key to exit" );
        const int bufsiz = 16;
        char buf[bufsiz];
        fgets( buf, bufsiz, stdin );

    if the user enters a C-C, the program exits

    TODO is it because C-C also outputs chars to the stream, including a newline?
    */

    return EXIT_SUCCESS;
}
