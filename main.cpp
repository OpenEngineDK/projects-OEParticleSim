// Main setup for the OpenEngine Racer project.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>

// Core structures
#include <Core/Engine.h>

// Display structures
#include <Display/Frustum.h>
#include <Display/ViewingVolume.h>
#include <Display/Camera.h>
#include <Display/IFrame.h>

// SDL implementation
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

// Particle System 
#include <ParticleSystem/ParticleSystem.h>

// OpenGL rendering implementation
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/OpenGL/TextureLoader.h>

// Resources
#include <Resources/File.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>

// OBJ and TGA plugins
#include <Resources/TGAResource.h>
#include <Resources/OBJResource.h>

// Scene structures
#include <Scene/SceneNode.h>

// Utilities and logger
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Utils/Statistics.h>
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>

// OEParticleSim utility files
#include "FireNode.h"

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::ParticleSystem;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&              engine;
    IFrame*               frame;
    Viewport*             viewport;
    IViewingVolume*       viewingvolume;
    Camera*               camera;
    Frustum*              frustum;
    IRenderer*            renderer;
    IMouse*               mouse;
    IKeyboard*            keyboard;
    ISceneNode*           scene;
    ParticleSystem*       particleSystem;
    bool                  resourcesLoaded;
    Config(IEngine& engine)
        : engine(engine)
        , frame(NULL)
        , viewport(NULL)
        , viewingvolume(NULL)
        , camera(NULL)
        , frustum(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)
        , particleSystem(NULL)
        , resourcesLoaded(false)
    {}
};

// Forward declaration of the setup methods
void SetupResources(Config&);
void SetupDisplay(Config&);
void SetupParticleSystem(Config&);
void SetupScene(Config&);
void SetupRendering(Config&);
void SetupDevices(Config&);
void SetupDebugging(Config&);

int main(int argc, char** argv) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
//     logger.info << "========= Running The OpenEngine Racer Project =========" << logger.end;
//     logger.info << "This project is a simple testing project for OpenEngine." << logger.end;
//     logger.info << "It uses an old physics system and is not very stable." << logger.end;
//     logger.info << logger.end;
//     logger.info << "Vehicle controls:" << logger.end;
//     logger.info << "  drive forwards:  up-arrow" << logger.end;
//     logger.info << "  drive backwards: down-arrow" << logger.end;
//     logger.info << "  turn left:       left-arrow" << logger.end;
//     logger.info << "  turn right:      right-arrow" << logger.end;
//     logger.info << logger.end;
//     logger.info << "Camera controls:" << logger.end;
//     logger.info << "  move forwards:   w" << logger.end;
//     logger.info << "  move backwards:  s" << logger.end;
//     logger.info << "  move left:       a" << logger.end;
//     logger.info << "  move right:      d" << logger.end;
//     logger.info << "  rotate:          mouse" << logger.end;
//     logger.info << logger.end;

    // Create an engine and config object
    Engine* engine = new Engine();
    Config config(*engine);

    // Setup the engine
    SetupResources(config);
    SetupDisplay(config);
    SetupParticleSystem(config);
    SetupScene(config);
    SetupRendering(config);
    SetupDevices(config);
    
    // Possibly add some debugging stuff
    // SetupDebugging(config);

    // Start up the engine.
    engine->Start();

    // Return when the engine stops.
    delete engine;
    return EXIT_SUCCESS;
}

void SetupResources(Config& config) {
    // set the resources directory
    // @todo we should check that this path exists
    string resources = "projects/OEParticleSim/data/";
    DirectoryManager::AppendPath(resources);

    // load resource plug-ins
    //ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());
    ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());

    config.resourcesLoaded = true;
}

void SetupDisplay(Config& config) {
    if (config.frame         != NULL ||
        config.viewingvolume != NULL ||
        config.camera        != NULL ||
        config.frustum       != NULL ||
        config.viewport      != NULL)
        throw Exception("Setup display dependencies are not satisfied.");

    config.frame         = new SDLFrame(800, 600, 32);
    config.viewingvolume = new ViewingVolume();
    config.camera        = new Camera( *config.viewingvolume );
    config.frustum       = new Frustum(*config.camera, 20, 3000);
    config.viewport      = new Viewport(*config.frame);
    config.viewport->SetViewingVolume(config.frustum);

    config.engine.InitializeEvent().Attach(*config.frame);
    config.engine.ProcessEvent().Attach(*config.frame);
    config.engine.DeinitializeEvent().Attach(*config.frame);
}

void SetupRendering(Config& config) {
    if (config.viewport == NULL ||
        config.renderer != NULL ||
        config.scene == NULL)
        throw Exception("Setup renderer dependencies are not satisfied.");

    // Create a renderer
    config.renderer = new Renderer();

    // Setup a rendering view
    RenderingView* rv = new RenderingView(*config.viewport);
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    TextureLoader* tl = new TextureLoader();
    config.renderer->InitializeEvent().Attach(*tl);

    // Supply the scene to the renderer
    config.renderer->SetSceneRoot(config.scene);

    config.engine.InitializeEvent().Attach(*config.renderer);
    config.engine.ProcessEvent().Attach(*config.renderer);
    config.engine.DeinitializeEvent().Attach(*config.renderer);
}

void SetupDevices(Config& config) {
    if (config.mouse    != NULL ||
        config.keyboard != NULL ||
        config.camera  == NULL) 
        throw Exception("Setup keyboard dependencies are not satisfied.");

    // Create the mouse and keyboard input modules
    config.keyboard = new SDLInput();
    config.mouse = (IMouse*)config.keyboard;

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);

//     // Register movement handler to be able to move the camera
    MoveHandler* move_h = new MoveHandler(*config.camera, *config.mouse);
    config.keyboard->KeyEvent().Attach(*move_h);

    // Bind to the engine for processing time
    config.engine.InitializeEvent().Attach(*config.mouse);
    config.engine.ProcessEvent().Attach(*config.mouse);
    config.engine.DeinitializeEvent().Attach(*config.mouse);

    config.engine.InitializeEvent().Attach(*move_h);
    config.engine.ProcessEvent().Attach(*move_h);
    config.engine.DeinitializeEvent().Attach(*move_h);
}

void SetupParticleSystem (Config& config) {
    
    config.particleSystem = new ParticleSystem();
    
    // Add to engine for processing time (with its timer)
    ParticleSystemTimer* pstimer = new ParticleSystemTimer(*config.particleSystem);
    config.engine.InitializeEvent().Attach(*config.particleSystem);
    config.engine.ProcessEvent().Attach(*pstimer);
    config.engine.DeinitializeEvent().Attach(*config.particleSystem);

}

void SetupScene(Config& config) {
    if (config.scene  != NULL ||
        config.particleSystem == NULL ||
        config.resourcesLoaded == false)
        throw Exception("Setup scene dependencies are not satisfied.");


    config.camera->SetPosition(Vector<3,float>(100,0,100));
    config.camera->LookAt(Vector<3,float>(0.0,0.0,0.0));

    // Create scene nodes
    config.scene = new SceneNode();


    FireNode* fireNode = new FireNode(config.particleSystem);
    config.scene->AddNode( fireNode );
    config.particleSystem->ProcessEvent().Attach(*fireNode);
}

void SetupDebugging(Config& config) {

    // Visualization of the frustum
    if (config.frustum != NULL) {
        config.frustum->VisualizeClipping(true);
        config.scene->AddNode(config.frustum->GetFrustumNode());
    }

    // Add Statistics module
    config.engine.ProcessEvent().Attach(*(new OpenEngine::Utils::Statistics(1000)));
}
