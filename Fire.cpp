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
#include <Resources/ITextureResource.h>

using OpenEngine::Math::Vector;
using OpenEngine::Resources::ResourceManager;
using OpenEngine::Resources::ITextureResource;

Fire::Fire(OpenEngine::ParticleSystem::ParticleSystem& system,
                           TextureLoader& textureLoader):
                           
    FireEffect(system,
               200,     //numParticles
               0.04,    //emitRate
               6.0,     //number 
               2.0,     //numberVar
               1.1,     //life
               0.5,     //lifeVar
               0.17,    //angle
               230.0,   //spin
               100.0,   //spinVar
               45.0,    //speed
               10.0,    //speedVar
               Vector<3,float>(0,0.295,0),       //antigravity
               textureLoader)
{

    ITextureResourcePtr tex1 = 
        ResourceManager<ITextureResource>::Create("Smoke/smoke01.tga");
    AddTexture(tex1);


    // color modifier
    colormod.AddValue( .9, Vector<4,float>(0.1, 0.01, .01, .4)); // blackish
    colormod.AddValue( .7, Vector<4,float>( .7,  0.3,  .1, .6)); // redish
    colormod.AddValue( .6, Vector<4,float>( .9, 0.75,  .2, .7)); // orangeish
    colormod.AddValue( .0, Vector<4,float>(0.1,  0.1,  .3, .1)); // blueish

    // size variations 
    sizem.AddValue(1.0, 4); 
    sizem.AddValue(.65, 7);
    sizem.AddValue( .18, 6);    
    sizem.AddValue( .0, 3);    
    
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
