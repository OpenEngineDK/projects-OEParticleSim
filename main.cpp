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
#include <Display/ViewingVolume.h>
#include <Display/Camera.h>
#include <Display/OpenGL/RenderCanvas.h>

// SDL implementation
#include <Display/SDLEnvironment.h>

// Particle System 
#include <Renderers/OpenGL/ParticleRenderer.h>
#include <ParticleSystem/ParticleSystem.h>
#include <Effects/FireEffect.h>
// #include <Effects/FireEffectEdit.h>

// OpenGL rendering implementation
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/TextureLoader.h>

// Resources
#include <Resources/File.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>

// OBJ and SDLImage plugins
#include <Resources/SDLImage.h>
#include <Resources/OBJResource.h>

// Scene structures
#include <Scene/SceneNode.h>

// Utilities and logger
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Utils/Statistics.h>
#include <Utils/BetterMoveHandler.h>
#include <Utils/QuitHandler.h>

// OEParticleSim utility files
// #include "Fire.h"

// mouse tools
// #include <Utils/MouseSelection.h>
// #include <Utils/SelectionSet.h>
// #include <Utils/SelectionTool.h>
// #include <Utils/TransformationTool.h>
// #include <Utils/CameraTool.h>
// #include <Utils/WidgetTool.h>
// #include <Utils/ToolChain.h>
//#include <Resources/SDLFont.h>
#include <Resources/CairoFont.h>
#include <Utils/FPSSurface.h>
#include <Display/HUD.h>

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Display::OpenGL;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Display;
using namespace OpenEngine::ParticleSystem;
using namespace OpenEngine::Effects;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&              engine;
    IEnvironment*         env;
    IFrame*               frame;
    IRenderCanvas*              canvas;
    IViewingVolume*       viewingvolume;
    Camera*               camera;
    IRenderer*            renderer;
    IMouse*               mouse;
    IKeyboard*            keyboard;
    ISceneNode*           scene;
    ParticleSystem*       particleSystem;
    bool                  resourcesLoaded;
    TextureLoader*        tl;
    // MouseSelection*       ms;
    FireEffect*           fire;
    Config(IEngine& engine)
        : engine(engine)
        , frame(NULL)
        , canvas(NULL)
        , viewingvolume(NULL)
        , camera(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)
        , particleSystem(NULL)
        , resourcesLoaded(false)
        , tl(NULL)
        // , ms(NULL)
        , fire(NULL)
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
    SetupDevices(config);
    SetupParticleSystem(config);
    SetupRendering(config);
    SetupScene(config);
    // Possibly add some debugging stuff
    SetupDebugging(config);

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
    //    ResourceManager<ITextureResource>::AddPlugin(new TGAPlugin());
    ResourceManager<ITextureResource>::AddPlugin(new SDLImagePlugin());
    // ResourceManager<IFontResource>::AddPlugin(new SDLFontPlugin());
    ResourceManager<IFontResource>::AddPlugin(new CairoFontPlugin());
    config.resourcesLoaded = true;
}

void SetupDisplay(Config& config) {
    if (config.frame         != NULL ||
        config.viewingvolume != NULL ||
        config.camera        != NULL ||
        config.canvas        != NULL)
        throw Exception("Setup display dependencies are not satisfied.");

    config.env           = new SDLEnvironment();
    config.frame         = &config.env->CreateFrame();
    config.viewingvolume = new ViewingVolume();
    config.camera        = new Camera( *config.viewingvolume );
    config.canvas        = new RenderCanvas();
    config.frame->SetCanvas(config.canvas);
    config.canvas->SetViewingVolume(config.camera);
    config.engine.InitializeEvent().Attach(*config.env);
    config.engine.ProcessEvent().Attach(*config.env);
    config.engine.DeinitializeEvent().Attach(*config.env);
}

void SetupRendering(Config& config) {
    if (config.canvas == NULL ||
        config.renderer != NULL)
        throw Exception("Setup renderer dependencies are not satisfied.");

    // Create a renderer
    config.renderer = new Renderer();
    config.canvas->SetRenderer(config.renderer);
    // Setup a rendering view
    IRenderingView* rv = new RenderingView();
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    config.tl = new TextureLoader(*config.renderer);
    config.renderer->PreProcessEvent().Attach(*config.tl);


    // config.ms = new MouseSelection(*config.frame, *config.mouse, NULL);

    config.fire = new FireEffect(*config.particleSystem);



    // SelectionSet<ISceneNode>* ss = new SelectionSet<ISceneNode>();
    // TransformationTool* tt = new TransformationTool(*config.tl);
    // ss->ChangedEvent().Attach(*tt);
    // CameraTool* ct   = new CameraTool();
    // WidgetTool* wt   = new WidgetTool(*config.tl);
    // ToolChain*  tc   = new ToolChain();
    // SelectionTool* st = new SelectionTool(*ss);
    // tc->PushBackTool(wt);
    // tc->PushBackTool(ct);
    // tc->PushBackTool(st);

    // wt->AddWidget(new FireEffectEditWidget(config.fire));

    // config.ms->BindTool(config.viewport, tc);

    // config.renderer->PostProcessEvent().Attach(*config.ms);
    // config.mouse->MouseMovedEvent().Attach(*config.ms);
    // config.mouse->MouseButtonEvent().Attach(*config.ms);
    // config.keyboard->KeyEvent().Attach(*config.ms);

}

void SetupDevices(Config& config) {
    if (config.mouse    != NULL ||
        config.keyboard != NULL ||
        config.camera  == NULL) 
        throw Exception("Setup keyboard dependencies are not satisfied.");

    // Create the mouse and keyboard input modules
    config.keyboard = config.env->GetKeyboard();
    config.mouse = config.env->GetMouse();

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);

//     // Register movement handler to be able to move the camera
    BetterMoveHandler* move_h = new BetterMoveHandler(*config.camera, *config.mouse);
    config.keyboard->KeyEvent().Attach(*move_h);
    config.mouse->MouseMovedEvent().Attach(*move_h);
    config.mouse->MouseButtonEvent().Attach(*move_h);
    config.engine.InitializeEvent().Attach(*move_h);
    config.engine.DeinitializeEvent().Attach(*move_h);
    config.engine.ProcessEvent().Attach(*move_h);

    // Bind to the engine for processing time
    // config.engine.InitializeEvent().Attach(*move_h);
    // config.engine.ProcessEvent().Attach(*move_h);
    // config.engine.DeinitializeEvent().Attach(*move_h);
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
    config.canvas->SetScene(config.scene);
    // config.ms->SetScene(config.scene);


    // add a post process particle renderer
    ParticleRenderer<FireEffect::TYPE>* pr = new ParticleRenderer<FireEffect::TYPE>(config.fire->GetParticles());

    config.renderer->PostProcessEvent().Attach(*pr);

    // config.scene->AddNode( config.fire->GetSceneNode() );
    // config.particleSystem->ProcessEvent().Attach(*config.fire);
    config.fire->SetActive(true);


    // Setup fps counter
    FPSSurfacePtr fps = FPSSurface::Create();
    config.tl->Load(fps, TextureLoader::RELOAD_QUEUED);
    config.engine.ProcessEvent().Attach(*fps);
    HUD* hud = new HUD();
    HUD::Surface* fpshud = hud->CreateSurface(fps);
    config.renderer->PostProcessEvent().Attach(*hud);
    fpshud->SetPosition(HUD::Surface::LEFT, HUD::Surface::TOP);


}

void SetupDebugging(Config& config) {

    // Visualization of the frustum
//     if (config.frustum != NULL) {
//         config.frustum->VisualizeClipping(true);
//         config.scene->AddNode(config.frustum->GetFrustumNode());
//     }

    // Add Statistics module
    //config.engine.ProcessEvent().Attach(*(new OpenEngine::Utils::Statistics(1000)));
}
