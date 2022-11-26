#ifndef __SHARE_H__
#define __SHARE_H__

#define TEXT_SZ 2048
struct shared_use_st
{
    int written_by_you;
    char some_text[TEXT_SZ];
};

#endif
