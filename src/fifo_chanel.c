
#include "main.h"

void                fifo_chanel(t_abstract *abstr)
{
    int             fifo;

    fifo = mkfifo(FIFO_CL_DAE_CHAN, 777);
    if (fifo < 0 && errno != EEXIST)
        fatal_err_stdin("Creating FIFO", 1);
    fifo = mkfifo(FIFO_DAE_CL_CHAN, 777);
    if (fifo < 0 && errno != EEXIST)
        fatal_err_stdin("Creating FIFO", 1);
    abstr->first_chan =  open(FIFO_DAE_CL_CHAN, O_RDWR);
    abstr->second_chan = open(FIFO_CL_DAE_CHAN, O_RDWR);
    if (abstr->first_chan < 0 || abstr->second_chan < 0)
        fatal_err_stdin("Opening FIFO CL->DAE use sudo", 1);
}
