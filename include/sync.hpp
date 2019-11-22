#ifndef __SYNC_HPP__
#define __SYNC_HPP__

#include <sys/types.h>

namespace m_sum {
    static int it = 0;
    static pthread_mutex_t glit_lock;
    static pthread_cond_t cond_v;

    void init();
    void destroy();
    void subsribe();
    void wait();
};

#endif