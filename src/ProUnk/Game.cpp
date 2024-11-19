#include "ProUnk/Game.h"

#include "ProUnk/Keybindings.h"
#include "ProUnk/Objects/BasicObjects.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved) {
  if (DLL_PROCESS_ATTACH == ul_reason_for_call) {
    // printf("Loaded GameCode\n");
  } else if (DLL_PROCESS_ATTACH == ul_reason_for_call) {
    // printf("Unloaded GameCode\n");
  }
  return TRUE;
}

void core_update(CoreEvent* event) {
  using namespace prounk;
  using namespace engone;
  GameInstance *game = (GameInstance *)event->state;
  
  switch(event->type) {
    case CORE_EVENT_INIT: {
        event->state = init_game(game);
    } break;
    case CORE_EVENT_DEINIT: {
        
		glfwTerminate();

		// doesn't have to be a memory leak, it depends on the users code.
		// they may have allocated memory in global scope and never deleted it. But only if they used the tracker functions.
		//GetTracker().clear();

		// GetTracker().printInfo();
		// The tracker may a little tricky to analyize correctly sometimes. Just don't get too fixated on it.

		// this means that the Memory usage is somewhat high because of io_context (and glfw, and reactphysics)

		DestroyNetworking();
    } break;
    case CORE_EVENT_LOAD: {
        log::out << "load\n";
    } break;
    case CORE_EVENT_UNLOAD: {
        log::out << "unload\n";
    } break;
    case CORE_EVENT_UPDATE: {
        game->engine->mainUpdateTimer.step();
        // manageThreading();
        if(game->engine->mainUpdateTimer.accumulate()){
            // manageNonThreading();
            update_game(game);
        }else{
            float sleepy = game->engine->mainUpdateTimer.aimedDelta-game->engine->mainUpdateTimer.accumulator;
            if(sleepy*1000>1)
            Sleep(((int)(sleepy*1000))/1000.f);
        }

        if (game->engine->appInstances.size() == 0) {
            event->stop_loop = true;
            break;
        }

        // function seems to to processing of all inputs and stuff
        // mutex lock for window input stuff
        // special thread for input or just after render thread
        glfwPollEvents(); // window refreh callback to redraw when resizing?

        // sleep a bit if little work was done
        // const float limit = mainUpdateTimer.aimedDelta / 8;
        // if(mainUpdateTimer.delta < limit)
            // engone::Sleep(limit*2);
        // engone::Sleep(0.05);
    } break;
  }
}

// Window listener close callback
engone::EventType GameClose(engone::Event *e) {
    using namespace prounk;
    GameInstance *game = (GameInstance *)e->window->userData;
    e->window->close();
    e->window->getParent()->stopApp(game->app);
    return engone::EventNone;
}

prounk::GameInstance* init_game(prounk::GameInstance* state) {
    using namespace prounk;
    using namespace engone;

    Engone* engone = state ? state->engine : nullptr;
    AppInstance* app = state ? state->app : nullptr;
    bool initialized = app;
    // the dlls global variables won't be the same as the main process so we must
    // make sure they are set here.
    // SetupStaticVariables(engone);
    if (initialized) {
        Engone::UseSharedGlobal(engone);
        log::out << log::GRAY << "GameInstance data already allocated\n";
    } else {
        engone = Engone::InitializeGlobal();

        app = engone->createApp();
    #ifndef LIVE_EDITING
        // app->preUpdateProc = GamePreUpdate;
        // app->postUpdateProc = GamePostUpdate;
        // GameInit(engone,app,0);
    #else
        // app->libraryId = engone.addLibrary("D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.dll",
        // 	"D:\\Backup\\CodeProjects\\ProjectUnknown\\bin\\GameCode\\Debug-MSVC\\GameCode.pdb");
        // app->libraryId = engone->addLibrary("bin\\GameCode.dll",
        // 	"bin\\GameCode.pdb");
    #endif
        // engone->start();
        
        GameInstance *game =
            (GameInstance *)engone->getGameMemory()->allocate(sizeof(GameInstance));
        new (game) GameInstance();
        game->app = app;
        game->engine = engone;
        app->instanceData = game;

        Window *win = engone->createWindow({ModeWindowed, 1000, 800});
        game->mainWindow = win;
        win->userData = game;
        win->setTitle("Project Unknown");
        win->enableFirstPerson(true);

        win->addListener(EventClose, GameClose, "GameClose");

        // win->getStorage()->setRoot("D:/Backup/CodeProjects/ProjectUnknown/ProjectUnknown/assets/");
        win->getStorage()->setRoot("assets/");
        AssetStorage *assets = win->getStorage();
        assets->load<FontAsset>("fonts/consolas42");

        CreateDefualtKeybindings();

        // game->session = ALLOC_NEW(Session)();
        game->session =
            (Session *)engone->getGameMemory()->allocate(sizeof(Session));
        new (game->session) Session(engone);
        Dimension *firstDim = game->session->createDimension("0");

        // firstDim->getWorld()->getPhysicsWorld()->setEventListener(this);

        EngineObject *player = CreatePlayer(firstDim);
        game->playerController.setPlayerObject(player->getUUID());
        player->setPosition({3 * (GetRandom() - 0.5), 3 * (GetRandom() - 0.5),
                            3 * (GetRandom() - 0.5)});

        auto playerInfo = game->session->objectInfoRegistry.getCreatureInfo(
            player->getObjectInfo());
        playerInfo->inventoryDataIndex =
            game->session->inventoryRegistry.createInventory();

        Inventory *inv = game->session->inventoryRegistry.getInventory(
            playerInfo->inventoryDataIndex);
        inv->resizeSlots(16, nullptr);

        // if (info.flags != START_CLIENT) {
        EngineObject *terrain = CreateTerrain(firstDim, "ProtoArena2/ProtoArena2");
        //}

        EngineObject *floor = CreateTerrain(firstDim, "house_floor/house_floor");
        floor->setPosition({5.f,1.f,0.f});

        DirLight *dir = ALLOC_NEW(DirLight)({-0.2, -1, -0.2});
        engone->addLight(dir);
    }
    // registerItems();

    GameInstance *game = (GameInstance *)app->instanceData;

    // #########  CREATE PANELS  ###########
    PanelHandler &ph = game->panelHandler;
    if (!initialized) {
        ph.init(game->mainWindow);
        auto barPanel = ph.createPanel();
        barPanel->extraData = game;
        barPanel->name = "bar";
        barPanel->setSize(200, 70);
        barPanel->setPosition(10, 10);
        barPanel->updateProc = PlayerBarUpdate;

        // auto barPanel = ph.createPanel();
        // barPanel->extraData = game;
        // barPanel->name = "bar";
        // barPanel->setSize(200,70);
        // barPanel->setPosition(10,10);
        // barPanel->updateProc = PlayerBarUpdate;

    } else {
        auto barPanel = ph.getPanel("bar");
        barPanel->updateProc = PlayerBarUpdate;

        // auto barPanel = ph.getPanel("bar");
        // barPanel->updateProc = PlayerBarUpdate;
    }

    // ## OTHER GARBAGE?
    // EngineObject* dummy = CreateDummy(firstDim);
    // dummy->setPosition({ 4,7,8 });
    // firstDim->getWorld()->releaseAccess(dummy->getUUID());

    // // -- Setup network stuff
    // sessionPanel->setDefaultAddress(info.address, "Client");
    // if (info.flags & START_SERVER) {
    //     sessionPanel->setDefaultAddress(info.address, "Server");
    // } else if (info.flags & START_CLIENT) {
    //     sessionPanel->setDefaultAddress(info.address, "Client");
    // }

    // auto address = SplitAddress(info.address);

    // if (info.flags & START_SERVER) {
    //     m_session->getServer().start(address.port);
    // } else if (info.flags & START_CLIENT) {
    //     m_session->getClient().start(address.ip, address.port);
    // }
    return game;
}

void update_game(prounk::GameInstance* game) {
    using namespace prounk;
    using namespace engone;
    // ###### CODE FOR ENGINE #########################
    game->engine->currentLoopInfo = {};
    game->engine->currentLoopInfo.engone = game->engine;
    game->engine->currentLoopInfo.timeStep = game->engine->mainUpdateTimer.aimedDelta;
    game->engine->currentLoopInfo.interpolation = 1; // needed when using interpolation?
    
    for(Window* win : game->engine->windows){
        win->restrictDLL = false;
        win->performPendingActions();
        
        if(!win->getStorage()->getIOProcessors().empty())
            win->getStorage()->getIOProcessors()[0]->process();
        if(!win->getStorage()->getDataProcessors().empty())
            win->getStorage()->getDataProcessors()[0]->process();
        win->runListeners();
        win->restrictDLL = true;
    }
    
    game->engine->currentLoopInfo.instance = game->app;
    
    // ############# THE GAME PRE CODE #######################
    {
        auto info = &game->engine->currentLoopInfo;
        
        info->window = game->mainWindow; // being cheeky here
        // TODO: Check for null
        engone::EngineObject *plr =
            game->session->getDimension("0")->getWorld()->getObject(
                game->playerController.playerId);

        game->session->update(info);

        if (info->window->isKeyPressed(GLFW_KEY_O)) {
            info->engone->setFlags(info->engone->getFlags() ^
                                engone::Engone::EngoneShowHitboxes);
        }
        if (info->window->isKeyPressed(GLFW_KEY_ESCAPE)) {
            info->window->lockCursor(!info->window->isCursorLocked());
        }
        if (info->window->isKeyPressed(GLFW_KEY_LEFT_ALT)) {
            info->window->lockCursor(false);
        }
        if (info->window->isKeyReleased(GLFW_KEY_LEFT_ALT)) {
            info->window->lockCursor(true);
        }
    }
    
    // ############ MIDDLE ENGINE STUFF ################
    for (EngineWorld* world : game->engine->worlds) {
        world->update(game->engine->currentLoopInfo);
    }
    
    // ################ POST GAME CODE ################
    {
        auto info = &game->engine->currentLoopInfo;
        info->window = game->mainWindow; // being cheeky here
        EngineObject* plr =
            game->session->getDimension("0")->getWorld()->getObject(
                game->playerController.playerId);
        info->interpolation = 1;

        game->playerController.update(info, plr);

        game->panelHandler.render(info);
        game->playerController.render(info, plr);

        const char *const game_mem_path = "manual.game_state";

        if (info->window->isKeyPressed(GLFW_KEY_I)) {
            info->engone->saveGameMemory(game_mem_path);
        }
        if (info->window->isKeyPressed(GLFW_KEY_U)) {
            info->engone->loadGameMemory(game_mem_path);
        }

        using namespace engone;
        UIModule &ui = info->window->uiModule;

        auto infoback = ui.makeBox();
        auto infodump = ui.makeText();

        // Special keybindings
        // ui::TextBox infodump = { "G : Flight, C : No Clip, K : Die, O : Hitboxes,
        // R : Respawn, E : Pickup item\nQ : Throw held item, TAB : Inventory, ALT +
        // LM/RM : Change UI layout\nJ : Spawn Dummies, I : Enable
        // Dummies",0,0,17,nullptr,{1} };

        infodump->text =
            "G : Flight, C : No Clip, K : Die, O : Hitboxes, R : Respawn, E : "
            "Pickup item\nQ : Throw held item, TAB : Inventory, ALT + LM/RM : "
            "Change UI layout\nJ : Spawn Dummies, I : Enable Dummies";
        infodump->h = 17;
        infodump->color = {1.f, 1.f, 1.f, 1.f};

        float text_height = ui.getHeightOfText(infodump); // includes newlines

        infodump->x = 3;
        infodump->y = game->mainWindow->getHeight() - text_height - 3;

        // ui::Box infoback = { 0,0,0,0,{0.f,0.5f} };
        infoback->color = {0.f, 0.f, 0.f, 0.5f};

        infoback->w = ui.getWidthOfText(infodump) + 6;
        infoback->h = text_height + 6;

        infoback->x = 0;
        infoback->y = infodump->y;
    }
    // ################# POST ENGINE STUFF ####################
    for(Window* win : game->engine->windows){
        win->restrictDLL = false;
        win->performPendingActions();
        win->resetEvents(false);
    }
		
    for(AppInstance* app : game->engine->removeApps){
        for(int i=0;i<game->engine->appInstances.size();i++){
            if(app==game->engine->appInstances[i]){
                game->engine->appInstances.erase(game->engine->appInstances.begin()+i);
                i--;
                break;
            }	
        }
    }
    game->engine->removeApps.clear();
    game->engine->renderApps();
}


int GAMEAPI GameInit(void *_engone, void *_app, void *extra) {
  using namespace engone;
  using namespace prounk;
  Engone *engone = (Engone *)_engone;
  AppInstance *app = (AppInstance *)_app;

  bool initialized = app->instanceData;

  // the dlls global variables won't be the same as the main process so we must
  // make sure they are set here.
  // SetupStaticVariables(engone);
  Engone::UseSharedGlobal(engone);
  if (initialized) {
    log::out << log::GRAY << "GameInstance data already allocated\n";
  } else {
    GameInstance *game =
        (GameInstance *)engone->getGameMemory()->allocate(sizeof(GameInstance));
    new (game) GameInstance();
    game->app = app;
    app->instanceData = game;

    Window *win = engone->createWindow({ModeWindowed, 1000, 800});
    game->mainWindow = win;
    win->userData = game;
    win->setTitle("Project Unknown");
    win->enableFirstPerson(true);

    win->addListener(EventClose, GameClose, "GameClose");

    // win->getStorage()->setRoot("D:/Backup/CodeProjects/ProjectUnknown/ProjectUnknown/assets/");
    win->getStorage()->setRoot("assets/");
    AssetStorage *assets = win->getStorage();
    assets->load<FontAsset>("fonts/consolas42");

    CreateDefualtKeybindings();

    // game->session = ALLOC_NEW(Session)();
    game->session =
        (Session *)engone->getGameMemory()->allocate(sizeof(Session));
    new (game->session) Session(engone);
    Dimension *firstDim = game->session->createDimension("0");

    // firstDim->getWorld()->getPhysicsWorld()->setEventListener(this);

    EngineObject *player = CreatePlayer(firstDim);
    game->playerController.setPlayerObject(player->getUUID());
    player->setPosition({3 * (GetRandom() - 0.5), 3 * (GetRandom() - 0.5),
                         3 * (GetRandom() - 0.5)});

    auto playerInfo = game->session->objectInfoRegistry.getCreatureInfo(
        player->getObjectInfo());
    playerInfo->inventoryDataIndex =
        game->session->inventoryRegistry.createInventory();

    Inventory *inv = game->session->inventoryRegistry.getInventory(
        playerInfo->inventoryDataIndex);
    inv->resizeSlots(16, nullptr);

    // if (info.flags != START_CLIENT) {
    EngineObject *terrain = CreateTerrain(firstDim, "ProtoArena2/ProtoArena2");
    //}

    EngineObject *floor = CreateTerrain(firstDim, "house_floor/house_floor");
    floor->setPosition({5.f,1.f,0.f});

    DirLight *dir = ALLOC_NEW(DirLight)({-0.2, -1, -0.2});
    engone->addLight(dir);
  }
  // registerItems();

  GameInstance *game = (GameInstance *)app->instanceData;

  // #########  CREATE PANELS  ###########
  PanelHandler &ph = game->panelHandler;
  if (!initialized) {
    ph.init(game->mainWindow);
    auto barPanel = ph.createPanel();
    barPanel->extraData = game;
    barPanel->name = "bar";
    barPanel->setSize(200, 70);
    barPanel->setPosition(10, 10);
    barPanel->updateProc = PlayerBarUpdate;

    // auto barPanel = ph.createPanel();
    // barPanel->extraData = game;
    // barPanel->name = "bar";
    // barPanel->setSize(200,70);
    // barPanel->setPosition(10,10);
    // barPanel->updateProc = PlayerBarUpdate;

  } else {
    auto barPanel = ph.getPanel("bar");
    barPanel->updateProc = PlayerBarUpdate;

    // auto barPanel = ph.getPanel("bar");
    // barPanel->updateProc = PlayerBarUpdate;
  }

  // ## OTHER GARBAGE?
  // EngineObject* dummy = CreateDummy(firstDim);
  // dummy->setPosition({ 4,7,8 });
  // firstDim->getWorld()->releaseAccess(dummy->getUUID());

  // // -- Setup network stuff
  // sessionPanel->setDefaultAddress(info.address, "Client");
  // if (info.flags & START_SERVER) {
  //     sessionPanel->setDefaultAddress(info.address, "Server");
  // } else if (info.flags & START_CLIENT) {
  //     sessionPanel->setDefaultAddress(info.address, "Client");
  // }

  // auto address = SplitAddress(info.address);

  // if (info.flags & START_SERVER) {
  //     m_session->getServer().start(address.port);
  // } else if (info.flags & START_CLIENT) {
  //     m_session->getClient().start(address.ip, address.port);
  // }

  return 0;
}
int GAMEAPI GamePreUpdate(engone::LoopInfo *info) {
  using namespace prounk;

  GameInstance *game = (GameInstance *)info->instance->instanceData;
  info->window = game->mainWindow; // being cheeky here
  // TODO: Check for null
  engone::EngineObject *plr =
      game->session->getDimension("0")->getWorld()->getObject(
          game->playerController.playerId);

  game->session->update(info);

  if (info->window->isKeyPressed(GLFW_KEY_O)) {
    info->engone->setFlags(info->engone->getFlags() ^
                           engone::Engone::EngoneShowHitboxes);
  }
  if (info->window->isKeyPressed(GLFW_KEY_ESCAPE)) {
    info->window->lockCursor(!info->window->isCursorLocked());
  }
  if (info->window->isKeyPressed(GLFW_KEY_LEFT_ALT)) {
    info->window->lockCursor(false);
  }
  if (info->window->isKeyReleased(GLFW_KEY_LEFT_ALT)) {
    info->window->lockCursor(true);
  }

  return 0;
}
int GAMEAPI GamePostUpdate(engone::LoopInfo *info) {
    using namespace prounk;
    GameInstance *game = (GameInstance *)info->instance->instanceData;
    info->window = game->mainWindow; // being cheeky here
    engone::EngineObject *plr =
        game->session->getDimension("0")->getWorld()->getObject(
            game->playerController.playerId);
    info->interpolation = 1;

    game->playerController.update(info, plr);

    game->panelHandler.render(info);
    game->playerController.render(info, plr);

    const char *const game_mem_path = "manual.game_state";

    if (info->window->isKeyPressed(GLFW_KEY_I)) {
        info->engone->saveGameMemory(game_mem_path);
    }
    if (info->window->isKeyPressed(GLFW_KEY_U)) {
        info->engone->loadGameMemory(game_mem_path);
    }

    {
        using namespace engone;
        UIModule &ui = info->window->uiModule;

        auto infoback = ui.makeBox();
        auto infodump = ui.makeText();

        // Special keybindings
        // ui::TextBox infodump = { "G : Flight, C : No Clip, K : Die, O : Hitboxes,
        // R : Respawn, E : Pickup item\nQ : Throw held item, TAB : Inventory, ALT +
        // LM/RM : Change UI layout\nJ : Spawn Dummies, I : Enable
        // Dummies",0,0,17,nullptr,{1} };

        infodump->text =
            "G : Flight, C : No Clip, K : Die, O : Hitboxes, R : Respawn, E : "
            "Pickup item\nQ : Throw held item, TAB : Inventory, ALT + LM/RM : "
            "Change UI layout\nJ : Spawn Dummies, I : Enable Dummies";
        infodump->h = 17;
        infodump->color = {1.f, 1.f, 1.f, 1.f};

        float text_height = ui.getHeightOfText(infodump); // includes newlines

        infodump->x = 3;
        infodump->y = game->mainWindow->getHeight() - text_height - 3;

        // ui::Box infoback = { 0,0,0,0,{0.f,0.5f} };
        infoback->color = {0.f, 0.f, 0.f, 0.5f};

        infoback->w = ui.getWidthOfText(infodump) + 6;
        infoback->h = text_height + 6;

        infoback->x = 0;
        infoback->y = infodump->y;
    }

    return 0;
}