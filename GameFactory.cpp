// Game Factory for the OEParticleSim project.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// Class header
#include "GameFactory.h"

#include <Devices/SDLInput.h>
#include <Display/Viewport.h>
#include <Display/ViewingVolume.h>
#include <Display/SDLFrame.h>
#include <Logging/Logger.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Resources/ResourceManager.h>
#include <Resources/File.h>
#include <Resources/OBJResource.h>
#include <Resources/TGAResource.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Meta/OpenGL.h>

#include <string>
#include <fstream>

// from GenericHandler extension
#include <Utils/MoveHandler.h>
#include <Utils/QuitHandler.h>

// from the ParticleSystem extension
#include <ParticleSystem/ParticleSystem.h>

// Project files
#include "FireNode.h"

// Additional namespaces (others are in the header).
using namespace OpenEngine::Display;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

class GLSettings : public IRenderNode {
public:
    void Apply(IRenderingView* view) {
        // Enable color material, including face colors when blending. 
        glEnable(GL_COLOR_MATERIAL);

        GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
        GLfloat lm_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glClearColor(1.0f,1.0f,1.0f,0.5f);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);	// Turn Depth Testing Off

        VisitSubNodes(*view);
    }
};


/**
 * Factory constructor.
 *
 * Initializes the different components so they are accessible when
 * the setup method is executed.
 */
GameFactory::GameFactory() {

    // Create a frame and viewport.
    this->frame = new SDLFrame(800, 600, 32);
    //this->frame    = new SDLFrame(1400, 900, 32, FRAME_FULLSCREEN);

    // Main viewport
    viewport = new Viewport(*frame);

    // Create a renderer.
    this->renderer = new Renderer();

    // Setup the camera
    camera = new Camera(*(new ViewingVolume()));
    camera->SetPosition(Vector<3,float>(0,20,100));
    viewport->SetViewingVolume(camera);

    // Add a rendering view to the renderer
    this->renderer->AddRenderingView(new RenderingView(*viewport));
}

/**
 * Setup handler.
 *
 * This is the main setup method of the game factory. Here you can add
 * any non-standard modules and perform other setup tasks prior to the
 * engine start up.
 *
 * @param engine The game engine instance.
 */
bool GameFactory::SetupEngine(IGameEngine& engine) {
    // First we set the resources directory
    string resourcedir = "./projects/OEParticleSim/data/";
    ResourceManager::AppendPath(resourcedir);
    logger.info << "Resource directory: " << resourcedir << logger.end;

    // load the resource plug-ins
    ResourceManager::AddTexturePlugin(new TGAPlugin());

    // Setup input handling
    SDLInput* input = new SDLInput();
    engine.AddModule(*input);

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler();

    Listener<QuitHandler, KeyboardEventArg>* quit_l
      = new Listener<QuitHandler, KeyboardEventArg> (*quit_h, &QuitHandler::HandleQuit);
    IKeyboard::keyUpEvent.Add(quit_l);

    // Register the handler as a listener on up and down keyboard events.
    MoveHandler* move_h = new MoveHandler(*camera);
    move_h->RegisterWithEngine(engine);


    // Create scene root
    ISceneNode* root = new GLSettings();
    this->renderer->SetSceneRoot(root);

    ParticleSystem* particleSystem = new ParticleSystem();
    engine.AddModule(*particleSystem);

    FireNode* fireNode = new FireNode(particleSystem);
    root->AddNode( fireNode );

    // Return true to signal success.
    return true;
}

// Other factory methods. The returned objects are all created in the
// factory constructor.
IFrame*      GameFactory::GetFrame()    { return this->frame;    }
IRenderer*   GameFactory::GetRenderer() { return this->renderer; }
