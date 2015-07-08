#ifndef lib_net_event_reader_h
#define lib_net_event_reader_h

#include <lib.core/defines.h>
#include <lib.core/object.h>
#include <lib.net/private-structures.h>

#define CUSTOM_EVENT_STOP ((uint64_t)1)

void event_reader        (Thread *thread);
void event_reader_stop   (NetPrivate *net, EpollEvent event);
bool event_reader_signal (NetPrivate *net, EpollEvent event);

#endif
