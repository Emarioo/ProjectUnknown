#pragma once

enum CoreEventType {
    CORE_EVENT_UPDATE,
    CORE_EVENT_INIT,
    CORE_EVENT_DEINIT,
    CORE_EVENT_UNLOAD,
    CORE_EVENT_LOAD,
};
struct CoreEvent {
    CoreEventType type;
    bool stop_loop;
    void* state;
};
typedef void(*FnCoreUpdate)(CoreEvent*);

void start_driver(int argc, char** argv);