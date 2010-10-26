// custom fire effect
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "Fire.h"

#include <Math/Vector.h>
#include <ParticleSystem/ParticleSystem.h>
#include <Renderers/TextureLoader.h>
#include <Resources/ResourceManager.h>
#include <Resources/ITexture2D.h>
#include <Math/Math.h>

using OpenEngine::Math::Vector;
using OpenEngine::Resources::ResourceManager;
using OpenEngine::Resources::ITexture2D;

Fire::Fire(OpenEngine::ParticleSystem::ParticleSystem& system,
           TextureLoader& textureLoader):
                           
    FireEffectEdit(system,
               200,     //numParticles
               0.04,    //emitRate
               8.0,     //number 
               2.0,     //numberVar
               .8,     //life
               0.2,     //lifeVar
               2*OpenEngine::Math::PI,    //angle
               0.0,   //spin
               0.0,   //spinVar
               10.0,    //speed
               1.0,    //speedVar
               Vector<3,float>(0,200.82,0),       //antigravity
               textureLoader)
{

    ITexture2DPtr tex1 = 
        // ResourceManager<ITexture2D>::Create("Smoke/smoke01.tga");
        // ResourceManager<ITexture2D>::Create("fire.jpg");
        //ResourceManager<ITexture2D>::Create("RealFlame_02.png");
        ResourceManager<ITexture2D>::Create("star.jpg");
    AddTexture(tex1);


    // color modifier
    colormod.AddValue( .9, Vector<4,float>(0.1, 0.01, .01, .4)); // blackish
    colormod.AddValue( .7, Vector<4,float>( .7,  0.3,  .1, .6)); // redish
    colormod.AddValue( .6, Vector<4,float>( .9, 0.75,  .2, .7)); // orangeish
    colormod.AddValue( .0, Vector<4,float>(0.1,  0.1,  .3, .1)); // blueish

    // size variations 
    sizem.AddValue(1.0, 30); 
    // sizem.AddValue(.65, 7);
    // sizem.AddValue( .18, 6);    
    sizem.AddValue( .0, 20);    
    
    system.ProcessEvent().Attach(*this);
}

Fire::~Fire() {
    system.ProcessEvent().Detach(*this);
} 

void Fire::Handle(ParticleEventArg e) {
    FireEffect::Handle(e);
//     for (particles->iterator.Reset(); 
//          particles->iterator.HasNext(); 
//          particles->iterator.Next()) {
    
//         TYPE& particle = particles->iterator.Element();
        
//     }

}
