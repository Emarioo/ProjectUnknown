
#include "Engone/PlatformLayer.h"
#include "Engone/Logger.h"
#include "Engone/Driver.h"

#ifndef LIVE_EDITING
    #include "ProUnk/Game.h"
#endif

void start_driver(int argc, char** argv) {
    using namespace engone;
    void* state = nullptr;
    const char* dll_path = "bin/game.dll";
    const char* tmp_dll_path = "bin/game_.dll";
    
    void* dll_handle = nullptr;
    FnCoreUpdate dll_core_update = nullptr;
    double last_timestamp = 0.0;
    
    auto last_time = StartMeasure();
   
    double reload_time = 0.0;
    const double RELOAD_DELAY = 1;
    
    CoreEvent event;
    event.state = nullptr;
    event.stop_loop = false;
    event.type = CORE_EVENT_UPDATE;
    
    #ifndef LIVE_EDITING
    dll_core_update = core_update;
    event.type = CORE_EVENT_INIT;
    core_update(&event);
    #endif
    
    while(true) {
        auto now = StartMeasure();
        double delta = DiffMeasure(now - last_time);
        last_time = now;
        
        #ifdef LIVE_EDITING
        reload_time += delta;
        if(reload_time > RELOAD_DELAY){
            reload_time = 0.0;
        
            double now_timestamp=0;
            bool yes = FileLastWriteSeconds(dll_path, &now_timestamp);
            if(!yes) {
                log::out << log::RED << "Driver : Cannot load '"<<tmp_dll_path<<"'\n";	
            } else {
                // log::out << "sec/modified : "<<seconds << " - "<<lib.lastModified<<"\n";
                if(now_timestamp > last_timestamp){
                    last_timestamp = now_timestamp;
                    bool was_loaded = dll_handle;
                    if(dll_handle) {
                        event.type = CORE_EVENT_UNLOAD;
                        dll_core_update(&event);
                        UnloadDynamicLibrary(dll_handle);
                        dll_handle = nullptr;
                        dll_core_update = nullptr;
                    }
                    
                    FileCopy(dll_path, tmp_dll_path);
                    
                    // std::string pdbTemp = lib.pdbPath;
                    // ind = pdbTemp.find_last_of(".");
                    // if(ind==std::string::npos) ind = pdbTemp.size();
                    // pdbTemp.insert(ind,"_");
                    
                    // bool failed = FileCopy(lib.pdbPath,pdbTemp.c_str());
                    // Todo: check if copy failed
                    
                    void* new_dll = LoadDynamicLibrary(tmp_dll_path);
                    if(!new_dll){
                        log::out << log::RED << "Driver : Cannot load '"<<tmp_dll_path<<"'\n";	
                    }else{
                        if(was_loaded)
                            log::out << log::LIME << "Driver : Reloaded '"<<tmp_dll_path<<"'\n";
                        else
                            log::out << log::LIME << "Driver : Loaded '"<<tmp_dll_path<<"'\n";
                        dll_handle = new_dll;
                        
                        dll_core_update = (FnCoreUpdate)GetFunctionPointer(dll_handle, "core_update");
                        
                        if(!dll_core_update) {
                            log::out << log::RED << "Driver : Cannot load function 'core_update' from dll '"<<tmp_dll_path<<"'\n";
                        } else {
                            event.type = CORE_EVENT_LOAD;
                            dll_core_update(&event);
                            
                            if(!was_loaded) {
                                event.type = CORE_EVENT_INIT;
                                dll_core_update(&event);
                            }
                        }
                    }
                }
            }
        }
        #endif
        
        
        if(dll_core_update) {
            event.type = CORE_EVENT_UPDATE;
            dll_core_update(&event);
        }
        
        if(event.stop_loop) {
            #ifdef LIVE_EDITING
            if(dll_core_update) {
                event.type = CORE_EVENT_DEINIT;
                dll_core_update(&event);
                event.type = CORE_EVENT_UNLOAD;
                dll_core_update(&event);
                UnloadDynamicLibrary(dll_handle);
                dll_handle = nullptr;
                dll_core_update = nullptr;
            }
            #endif
            break;
        }
    }
}