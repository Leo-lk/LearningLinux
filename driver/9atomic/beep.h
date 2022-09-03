#ifndef __BEEP_H
#define __BEEP_H

#define DEV_FIFO_TYPE 'k'

#define DEV_FIFO_CLEAN _IO(DEV_FIFO_TYPE,0)
#define DEV_FIFO_GETVALUE _IOR(DEV_FIFO_TYPE,1,int)
#define DEV_FIFO_SETVALUE _IOW(DEV_FIFO_TYPE,2,int)


#endif
