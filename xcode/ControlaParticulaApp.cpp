/*
//#include "cinder/app/AppBasic.h"
//#include "cinder/gl/gl.h"
#include <list>
#include "Controles.h"
#include "cinder/Rand.h"
//#include "cinder/Perlin.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class ControlaParticulaApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
    void mouseUp( MouseEvent event );
    void keyDown (KeyEvent event);
    void particula(Vec2f);
	void update();
	void draw();
    
    
    bool mIsPressed;
    Vec2f mMouseLoc;
    int mAge;
    int mLifespan;
    bool mIsDead;
    float blend;
    float agePer;

    Perlin mperlin;

    ParticleController mParticleController;
        
    };

//-----------------------------------------------------------------------------------------

void ControlaParticulaApp::setup()
{

    mMouseLoc = getMousePos();
    gl::enableAlphaBlending();
    blend = Rand::randFloat( 0.1,1.0 );
    Color( CM_HSV, 1.0f, 0.85f, 1.0f );

}


void ControlaParticulaApp::mouseDown( MouseEvent event ) {
    mIsPressed = true;
    
}
void ControlaParticulaApp::mouseUp( MouseEvent event ) {
    mIsPressed = false;
}

void ControlaParticulaApp::update()

{
    if( mIsPressed )
        mParticleController.addParticles( 20, mMouseLoc );

       mParticleController.update();
     
     
}

void ControlaParticulaApp::draw()
{

  

   
    gl::clear( Color( 0, 0, 0 ) ); 
    mMouseLoc = getMousePos();
    
  // gl::color( CM_HSV, 0.0f, 0.0f, blend );    
    gl::color(1, 1, 1, blend );
    
	mParticleController.draw();
    
}

void ControlaParticulaApp::keyDown (KeyEvent event){
    
    char tecla = event.getChar();
    
    if(tecla == 'f'){
        setFullScreen(!isFullScreen());}

    
}

CINDER_APP_BASIC( ControlaParticulaApp, RendererGl )
 */
