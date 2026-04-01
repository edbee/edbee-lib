#include "config-oniguruma-edbee.h"

#if defined(_WIN32)
    #include "config.win.h"
#elif defined(__FreeBSD__)
    #include "config.bsd.h"
#else
    #include "config.unix.h"
#endif

