// Game Factory for a test project.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// header define to avoid including definitions several times
#ifndef _TEST_GAME_FACTORY_H_
#define _TEST_GAME_FACTORY_H_

// Game engine interface
#include <Core/IGameFactory.h>
#include <Display/Camera.h>
#include <Display/Viewport.h>
#include <Scene/ISceneNode.h>

// Namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers;

/**
 * Game factory definition.
 *
 * The factory is implemented in GameFactory.cpp
 * Documentation on game factories can be found in Core/IGameFactory.h
 * 
 * @see IGameFactory
 */
class GameFactory : public IGameFactory {

private:
    IFrame*      frame;         // member variable for the engine frame
    IRenderer*   renderer;      // member variable for the engine renderer

    Viewport* viewport;

    Camera*    camera;
public:
    GameFactory();
    bool         SetupEngine(IGameEngine& engine);
    IFrame*      GetFrame();
    IRenderer*   GetRenderer();

};

#endif // _TEST_GAME_FACTORY_H_
