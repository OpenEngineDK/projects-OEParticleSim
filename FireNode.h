#ifndef _FIRE_NODE_H_
#define _FIRE_NODE_H_

#include <ParticleSystem/ParticleGroup.h>
#include <ParticleSystem/ParticleSystem.h>

#include <ParticleSystem/StaticForceModifier.h>
#include <ParticleSystem/RandomTimeBasedEmitterModifier.h>
#include <ParticleSystem/ColorModifier.h>
#include <ParticleSystem/SizeModifier.h>

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
#include <ParticleSystem/Particles/Color.h>
#include <ParticleSystem/Particles/Size.h>

#include <Renderers/OpenGL/TextureLoader.h>
#include <Resources/ITextureResource.h>
#include <Resources/ResourceManager.h>

using namespace OpenEngine::Renderers;
using namespace OpenEngine::Scene;
using namespace OpenEngine::ParticleSystem;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Renderers::OpenGL;


#define TYPE Color < Size < Orientation < Position < Lifespan < IParticle > > > > >

class FireNode : public IRenderNode {
private:
    ParticleGroup<TYPE>* particleGroup;
    ITextureResourcePtr texr;

public:
    FireNode() {
        const unsigned int numberOfParticles = 1000;
        particleGroup = new ParticleGroup<TYPE>(numberOfParticles);

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
    
        ColorModifier<TYPE>* colormodifier = 
	  new ColorModifier<TYPE>();
        particleGroup->AddModifier( colormodifier );

        SizeModifier<TYPE>* sizemodifier = 
	  new SizeModifier<TYPE>();
        particleGroup->AddModifier( sizemodifier );

        //load texture resource
        texr = ResourceManager<ITextureResource>::Create("Smoke/smoke01.tga");
    }

    ~FireNode() {
    }

    IParticleGroup* GetParticleGroup() {
        return particleGroup;
    }

    ISceneNode* GetSceneNode() {
        return this;
    }

    void Apply(IRenderingView* view) {

            glPushAttrib(GL_LIGHTING);    
            glDisable(GL_LIGHTING);
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_COLOR_MATERIAL);
            glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

            TYPE* particles = particleGroup->GetParticles();

            //glBegin(GL_LINES);
            for (unsigned int i = 0; i < particleGroup->GetNumberOfActiveParticles(); i++) {

                TYPE particle = particles[i];

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
                
                // TODO: how to determine rotation
                //glRotatef(pat.rotation, 0,0,1);

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
};

#endif
