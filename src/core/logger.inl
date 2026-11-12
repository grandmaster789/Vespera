#ifndef VESPERA_CORE_LOGGER_INL
#define VESPERA_CORE_LOGGER_INL

namespace vespera::core {
    template <typename...Ts>
    void log(Ts...args) {
        (void)sizeof...(Ts);
    }
}

#endif