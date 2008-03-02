#ifndef _FIRE_NODE_H_
#define _FIRE_NODE_H_

#include <ParticleSystem/ParticleSet.h>
#include <ParticleSystem/ParticleSystem.h>
#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/RandomTimeBasedEmitterModifier.h>
#include <ParticleSystem/BillboardedParticle.h>

#include <ParticleSystem/IEmitter.h>
#include <ParticleSystem/PointEmitter.h>

#include <Geometry/Face.h>
#include <Geometry/FaceSet.h>
#include <Renderers/IRenderer.h>
#include <Renderers/IRenderingView.h>
#include <Renderers/IRenderNode.h>

using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
using namespace OpenEngine::ParticleSystem;

#define TYPE BillboardedParticle //TEMP

class FireNode : public IRenderNode {
private:
    ParticleSet<TYPE>* particleSet;
public:
    FireNode(ParticleSystem* particleSystem) {
        const unsigned int numberOfParticles = 1000;
        particleSet = new ParticleSet<TYPE>(numberOfParticles);
        particleSystem->AddParticleSet(particleSet);

        // emitter
        IEmitter* emitter = new PointEmitter(Vector<3,float>(0.1f));
        particleSet->AddModifier( new RandomTimeBasedEmitterModifier<TYPE>(10,50,emitter) );

        // add updrift
        StaticForceModifier<TYPE>* antigravity = new StaticForceModifier<TYPE>(Vector<3,float>(0,0.00682,0));
        particleSet->AddModifier( antigravity );

        StaticForceModifier<TYPE>* wind = new StaticForceModifier<TYPE>(Vector<3,float>(0.00291,0,0));
        particleSet->AddModifier( wind );
    }

    ~FireNode() {
    }

    void Apply(IRenderingView* view) {

        IRenderer* rende = view->GetRenderer();
        Vector<3,float> deactiveColor(1.0,0.0,0.0);
        Vector<3,float> activeColor(0.0,1.0,0.0);

        // @todo delete all old nodes in the subNodes list
        this->subNodes.clear();

        // build new sub node tree
        TYPE* particles = particleSet->GetParticles();
        for (unsigned int i=0; i<particleSet->GetNumberOfActiveParticles(); i++)
            this->subNodes.push_back( particles[i].GetBillboard(view->GetViewport().GetViewingVolume()->GetPosition()) );

        // render subnodes
        VisitSubNodes(*view);

        // render particles with the renderes Draw function, this is done last as this destroys the render state.
        for (unsigned int i=0; i<particleSet->GetNumberOfParticles(); i++) {
            if (i < particleSet->GetNumberOfActiveParticles()) { //draw active particles
                rende->DrawPoint(particles[i].position, activeColor,5.0);
                rende->DrawLine(Line(particles[i].position,particles[i].position+ (particles[i].direction*1000)),activeColor,1.0f);
            } else { // draw deactive particles
                rende->DrawPoint(particles[i].position, deactiveColor,5.0);
            }
        }
    }
};

#endif
