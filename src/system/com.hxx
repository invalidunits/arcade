#ifndef _ARCADE_COM
#define _ARCADE_COM


#include <atomic>

#include "non_arcade.h"

#ifndef NO_ARCADE

namespace COM {
    inline std::atomic_uint16_t coin_inserted_value = 0;
    void beginCOMThread(void);
    void endCOMThread(void);

} // namespace COM

#endif


#endif // 