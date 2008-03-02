// Particle System Simulator demo.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include <Core/GameEngine.h>
#include <Core/IGameFactory.h>
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include "GameFactory.h"

using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;

int main( int argc, char** argv ) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Start the engine.
    IGameEngine& engine = GameEngine::Instance();
    engine.Start(new GameFactory());

    // Return when the engine stops.
    return EXIT_SUCCESS;
}
