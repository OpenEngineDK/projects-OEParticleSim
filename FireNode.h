#ifndef _FIRE_NODE_H_
#define _FIRE_NODE_H_

#include <ParticleSystem/ParticleGroup.h>
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/RandomTimeBasedEmitterModifier.h>

#include <ParticleSystem/IEmitter.h>
#include <ParticleSystem/PointEmitter.h>

#include <Geometry/Face.h>
#include <Geometry/FaceSet.h>
#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Renderers/IRenderNode.h>

#include <ParticleSystem/Particles/IParticle.h>
#include <ParticleSystem/Particles/Position.h>
#include <ParticleSystem/Particles/Orientation.h>
#include <ParticleSystem/Particles/Lifespan.h>

using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
using namespace OpenEngine::ParticleSystem;

#define TYPE Orientation < Position < Lifespan < IParticle > > >

class FireNode : public IRenderNode {
private:
    ParticleGroup<TYPE>* particleGroup;
public:
    FireNode(ParticleSystem* particleSystem) {
        const unsigned int numberOfParticles = 1000;
        particleGroup = new ParticleGroup<TYPE>(numberOfParticles);
        particleSystem->AddParticleGroup(particleGroup);

        // emitter
        IEmitter* emitter = new PointEmitter(Vector<3,float>(0.1f));
        particleGroup->AddModifier( new RandomTimeBasedEmitterModifier<TYPE>(10,50,emitter) );

        // add updrift
        StaticForceModifier<TYPE>* antigravity = 
	  new StaticForceModifier<TYPE>(Vector<3,float>(0,0.00682,0));
        particleGroup->AddModifier( antigravity );

        StaticForceModifier<TYPE>* wind = 
	  new StaticForceModifier<TYPE>(Vector<3,float>(0.00291,0,0));
        particleGroup->AddModifier( wind );
    }

    ~FireNode() {
    }

    IParticleGroup* FireNode::GetParticleGroup() {
        return particleGroup;
    }

    ISceneNode* FireNode::GetSceneNode() {
        return this;
    }

    void Apply(IRenderingView* view) {
      /*
        IRenderer* rende = view->GetRenderer();
        Vector<3,float> deactiveColor(1.0,0.0,0.0);
        Vector<3,float> activeColor(0.0,1.0,0.0);

        // @todo delete all old nodes in the subNodes list
        this->subNodes.clear();

        // build new sub node tree
        TYPE* particles = particleGroup->GetParticles();
        for (unsigned int i=0; i<particleGroup->GetNumberOfActiveParticles(); i++)
            this->subNodes.push_back( particles[i].GetBillboard(view->GetViewport().GetViewingVolume()->GetPosition()) );

        // render subnodes
        VisitSubNodes(*view);

        // render particles with the renderes Draw function, this is done last as this destroys the render state.
        for (unsigned int i=0; i<particleGroup->GetNumberOfParticles(); i++) {
            if (i < particleGroup->GetNumberOfActiveParticles()) { //draw active particles
                rende->DrawPoint(particles[i].position, activeColor,5.0);
                rende->DrawLine(Line(particles[i].position,particles[i].position+ (particles[i].direction*1000)),activeColor,1.0f);
            } else { // draw deactive particles
                rende->DrawPoint(particles[i].position, deactiveColor,5.0);
            }
        }
      */
    }
};

#endif
