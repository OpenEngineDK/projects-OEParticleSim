// custom fire effect
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _OESIM_FIRE_EFFECT_
#define _OESIM_FIRE_EFFECT_

#include <Effects/FireEffect.h>

namespace OpenEngine {
    namespace ParticleSystem {
        class ParticleSystem;
        class ParticleEventArg;
    }

    namespace Renderers {
        class TextureLoader;
    }
}

using OpenEngine::Effects::FireEffect;
using OpenEngine::Renderers::TextureLoader;

using OpenEngine::ParticleSystem::ParticleEventArg;

class Fire : public FireEffect {
private:
public:
    Fire(OpenEngine::ParticleSystem::ParticleSystem& system,
                 TextureLoader& textureLoader);
    
    virtual ~Fire();

    void Handle(ParticleEventArg e);

};

#endif
