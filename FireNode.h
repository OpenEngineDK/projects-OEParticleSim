#ifndef _FIRE_NODE_H_
#define _FIRE_NODE_H_

// base particle system
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/ParticleCollection.h>
#include <ParticleSystem/IParticleEffect.h>

// particle types
#include <ParticleSystem/Particles/IParticle.h>
#include <ParticleSystem/Particles/Position.h>
#include <ParticleSystem/Particles/PreviousPosition.h>
#include <ParticleSystem/Particles/Orientation.h>
#include <ParticleSystem/Particles/Lifespan.h>
#include <ParticleSystem/Particles/Color.h>
#include <ParticleSystem/Particles/Size.h>
#include <ParticleSystem/Particles/Texture.h>

// predefined modifiers
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/LinearColorModifier.h>
#include <ParticleSystem/SizeModifier.h>
#include <ParticleSystem/LifespanModifier.h>
#include <ParticleSystem/VerletModifier.h>
#include <ParticleSystem/RandomTextureRotationModifier.h>

// predefined initializers
#include <ParticleSystem/RandomTextureInitializer.h>
#include <ParticleSystem/RandomLifespanInitializer.h>
#include <ParticleSystem/RandomVerletInitializer.h>

#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Renderers/IRenderNode.h>
#include <Scene/ISceneNode.h>

#include <Renderers/OpenGL/TextureLoader.h>
#include <Resources/ITextureResource.h>
#include <Resources/ResourceManager.h>

#include <Meta/OpenGL.h>

using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
using namespace OpenEngine::ParticleSystem;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers::OpenGL;


typedef Color < Texture <Size < Orientation < PreviousPosition < Position < Lifespan < IParticle > > > > > > > TYPE;

class FireNode : public IRenderNode, public IParticleEffect {
private:
    ParticleCollection<TYPE>* particles;

    ParticleSystem* system;

    //initializers
    RandomVerletInitializer<TYPE> initverlet;
    RandomTextureInitializer<TYPE> inittex;

    //modifiers
    VerletModifier<TYPE> verlet;
    StaticForceModifier<TYPE> wind, antigravity;
    SizeModifier<TYPE> sizemod;
    LinearColorModifier<TYPE> colormod;
    LifespanModifier<TYPE> lifemod;
    RandomTextureRotationModifier<TYPE> rotationmod;

    float percentChanceOfSpawning;
    unsigned int spawns, spawnsVar; 
        
public:
    FireNode(ParticleSystem* system): system(system),
                initverlet(Vector<3,float>(0.0,1.,0.0), PI),
                wind(Vector<3,float>(1.591,0,0)),
                antigravity(Vector<3,float>(0,0.382,0)),
                percentChanceOfSpawning(0.6),
                spawns(100), spawnsVar(2){
        particles = system->CreateParticles<TYPE>(500);     
 
        //load texture resource
        ITextureResourcePtr texr1 = ResourceManager<ITextureResource>::Create("Smoke/smoke01.tga");
        ITextureResourcePtr texr2 = ResourceManager<ITextureResource>::Create("Smoke/smoke02.tga");
        ITextureResourcePtr texr3 = ResourceManager<ITextureResource>::Create("Smoke/smoke03.tga");

        inittex.AddTextureResource(texr1);
        inittex.AddTextureResource(texr2);
        inittex.AddTextureResource(texr3);
}

~FireNode() {
    delete[] particles;
}
 
void Handle(ParticleEventArg e) {

    //emitter code
    float change = (rand()/(float)RAND_MAX);
    if (percentChanceOfSpawning >= change)
        for (unsigned int i = 0; 
             i < spawns + (unsigned int)((rand()/(float)RAND_MAX)*spawnsVar); 
             i++)
            Emit();
    
//     for (particles->iterator.Reset(); 
//          particles->iterator.HasNext(); 
//          particles->iterator.Next()) {
            
//         //TYPE& particle = particles->iterator.Element();
            
//     }



    for (particles->iterator.Reset(); 
         particles->iterator.HasNext(); 
         particles->iterator.Next()) {
        TYPE& particle = particles->iterator.Element();
        
        // custom modify particles
        
        //random force in x-axis
        float change = (rand()/(float)RAND_MAX);
        if (0.7 >= change) {
            Vector<3,float> maxdir (0.0,0.0,0.9);
            particle.position += maxdir*((rand()/(float)RAND_MAX*2)-1);
        }
        
        // predefined particle modifiers
        wind.Process(e.dt, particle);
        antigravity.Process(e.dt, particle);
        sizemod.Process(e.dt,particle);
        verlet.Process(e.dt, particle);
        colormod.Process(e.dt, particle);
        rotationmod.Process(e.dt, particle);
        lifemod.Process(e.dt, particles->iterator);
    }
}

    void inline Emit() {
    if (particles->GetActiveParticles() >= particles->GetSize())
        return;
    //    logger.info << "num parts " << particles->GetActiveParticles() << logger.end;
    //    logger.info << "emit" << logger.end;
    TYPE& particle = particles->NewParticle();
    // static pos
    particle.position = Vector<3,float>(.0);
        
    // lifespan
    float lifespanVar = 1000.0;
    particle.lifespan = 1500.0 + (rand()/((float)RAND_MAX)-1)*lifespanVar;

    initverlet.Process(particle);
    inittex.Process(particle);
    //size
    particle.size = (rand()/((float)RAND_MAX)) * 0.5;

    //color
    particle.color = particle.startColor = Vector<4,float>(0.85,0.7,0.0,0.80);
    particle.endColor = Vector<4,float>(0.825,0.00,0.0,0.03);

    // random direction
    float angle = 0.1*PI;
    Vector<3,float> direction(0.0,1.0,0.0);
    float r = (rand()/((float)RAND_MAX))*angle;
    float p = (rand()/((float)RAND_MAX))*2*PI;
    Quaternion<float> q(r,p, 0);
    q.Normalize();
    particle.previousPosition = particle.position - q.RotateVector(direction);

}

void Apply(IRenderingView* view) {
        
    glPushAttrib(GL_LIGHTING);    
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
        
                
    for (particles->iterator.Reset(); particles->iterator.HasNext(); particles->iterator.Next()) {
        TYPE& particle = particles->iterator.Element();
        ITextureResourcePtr texr = particle.texture;
            
        //Set texture
        if (texr != NULL) {
            if (texr->GetID() == 0) {
                TextureLoader::LoadTextureResource(texr);
                //logger.info << texr->GetID() << logger.end;
            }
            glBindTexture(GL_TEXTURE_2D, texr->GetID());
        }
            
        else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
            
        glPushMatrix();
        glTranslatef(particle.position[0], particle.position[1], particle.position[2]);
            
        // billboard
        float modelview[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
        for( int i=0; i<3; i++ ) 
            for( int j=0; j<3; j++ ) {
                if ( i==j )
                    modelview[i*4+j] = 1.0;
                else
                    modelview[i*4+j] = 0.0;
            }
            
        glLoadMatrixf(modelview);
            
        glRotatef(particle.rotation, 0,0,1);
            
        float scale = particle.size;
        glScalef(scale,scale,scale);
            
        // constant color
        float c[4] = {.2,0.0,0.0,1.0};
            
        for(unsigned int i=0;i<4;i++)
            c[i] = particle.color[i];
            
        glColor4fv(c);
            
        // constant size 
        float s = 3.0;
            
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-1.0*s, -1.0*s, 0.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-1.0*s, s*1.0f, 0);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(s*1, s*1, 0);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(s*1, -1*s, 0);
            
        glEnd();
            
        glPopMatrix();
    }
        
    glDisable(GL_BLEND);
    glPopAttrib();
        
    // render subnodes
    VisitSubNodes(*view);      
}

ISceneNode* GetSceneNode() {
    return this;
}

};

#endif
