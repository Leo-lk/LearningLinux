#ifndef __BEEP_H
#define __BEEP_H

#define DEV_FIFO_TYPE 'k'

#define DEV_CLOSE_TIMER _IO(DEV_FIFO_TYPE,0)
#define DEV_MODIFY_TIMER _IOR(DEV_FIFO_TYPE,1,int)
#define DEV_OPEN_TIMER _IOW(DEV_FIFO_TYPE,2,int)


#endif
