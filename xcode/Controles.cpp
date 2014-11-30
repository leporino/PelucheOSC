
/*
#include <iostream>
#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
//#include "Controles.h"


using namespace ci;
using std::list;

//ParticleController::ParticleController()
{
}

//ParticleController::ParticleController(int amt, const Vec2i &mouseLoc)
{
}

/*
ParticleController::ParticleController( int res )
{
	mXRes = app::getWindowWidth()/res;
	mYRes = app::getWindowHeight()/res;
	
	for( int y=0; y<mYRes; y++ ){
		for( int x=0; x<mXRes; x++ ){
			addParticle( x, y, res );
		}
	}
}

 

void ParticleController::update()
{
	for( list<Particula>::iterator p = mParticles.begin(); p != mParticles.end();){
		//p->update();
	
    if( p->mIsDead ) {
        p = mParticles.erase( p );
    }
    else {
        p->update();
        ++p;
    }
    
    }
}

void ParticleController::draw(float per3, float sl3)
{
	for( list<Particula>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->draw(per3, sl3);
	}
}


void ParticleController::addParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		float x = Rand::randFloat( app::getWindowWidth() );
		float y = Rand::randFloat( app::getWindowHeight() );
		mParticles.push_back( Particula( Vec2f( x, y ) ) );
	}
}


void ParticleController::addParticles( int amt, const Vec2i &mouseLoc ) {
    
    for( int i=0; i<amt; i++ ) {
        Vec2f randVec = Rand::randVec2f() * 5.0f;
        mParticles.push_back( Particula( mouseLoc + randVec ) );
    }
}

void ParticleController::removeParticles( int amt )
{
	for( int i=0; i<amt; i++ )
	{
		mParticles.pop_back();
	}
}

*/