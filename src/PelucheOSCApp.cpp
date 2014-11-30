

#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "OscListener.h"
#include "OscSender.h"
#include "cinder/Color.h"
#include "ParticleSystem.h"
#include "cinder/Rand.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"




using namespace ci;
using namespace ci::app;
using namespace std;

#include "MidiIn.h"
#include "MidiMessage.h"
#include "MidiConstants.h"
#include <list>

#define SLIDER_NOTE 2
#define PERILLA 14


class PelucheOSCApp : public AppBasic {
 public:
	
    void prepareSettings(Settings * settings);
	void setup();
	void mouseDown( MouseEvent event );	
    void mouseUp( MouseEvent event );
	void mouseDrag(MouseEvent event );
	void update();
	void draw();
    void keyDown(KeyEvent);
    void reticula(Vec2f, float,float,float);
    void cuadrito(float r, float g, float b, float al, float x, float y, float ang, int largo);
    float alfa(Vec2f);
    
    float mapeos(float,float,float,float,float);
    
   	Vec2f torsoParticulas;
	
	float timeStep;
	float lineOpacity, pointOpacity;
	float particleNeighborhood, particleRepulsion;
	float centerAttraction;
	
    void fuerzaPrticula();
    void drawpariculas(Vec2f rata);
    void drawpariculas2(Vec2f ratas, Vec2f ratilla);
	int kParticles;
	ParticleSystem particleSystem;
    
	bool isMousePressed, slowMotion;

    bool mIsPressed;
    bool manoBola;
    int mAge;
    int mLifespan;
    bool mIsDead;
    float blend;
    float agePer;
    
	
    Perlin mperlin;
	osc::Listener listener;
    osc::Sender sender;
    std::string host;
	int port;
	
    float positionX;
	float positionY;
    int paso;
    int crece;
    bool deAdentro;
    float down;
    float miX,miY,mdX,mdY,tmD,tmI,tmIF,tmDF,torsoX,torsoZ,velocidad;// osc Msg
    float manoDXmap,manoDYmap,manoIXmap,manoIYmap,velPerlinI,velPerlinD,cuelloColor;
    Vec2f manoD;
    Vec2f manoI;
    bool atraeCentro;
    bool pelosT;
    bool particulaPress;
    float escena;
    
  //  typename Color::ColorT;
    
    void processMidiMessage(midi::Message* message);
	midi::Input mMidiIn;
	float sliderValue,per1,per2,per3,per4,per5,bot1,bot2,bot22,sl3,sl4,per6,bot6,per7,bot7;
};





void PelucheOSCApp::prepareSettings(Settings *settings){
	//settings->setFrameRate(60.0f);
	settings->setWindowSize(1280, 800);
    //settings->setFullScreen( false );
}




void PelucheOSCApp::setup(){
    
    
    Color(CM_HSV,1.0f,1.0f,1.0f);
    
    //ColorAT<float> t2_hsvToRG(1.0,1.0,1.0,1.0);
    
	listener.setup(3000);
    host = "localhost";
	port = 3000;
	sender.setup(host, port);
    
    gl::clear( Color( 0, 0, 0 ) ); 
    paso = 20;
    crece = 0;
   // gl::enableAlphaBlending();

    deAdentro = true;
    atraeCentro = true;
    down = 0;
    
    miX = 0;
    miY = 0;  // msg OSC
    mdX = 0; 
    mdY = 0;
    tmD = 0; 
    tmI = 0;
    tmIF= 0;
    tmDF= 0;
    torsoX = 0;
    torsoZ =0;
    velocidad = 0;
    velPerlinI = 0;
    velPerlinD = 0;
    cuelloColor = 0;
    
    
    
    
    if (mMidiIn.getNumPorts() > 0){
		mMidiIn.listPorts();
		mMidiIn.openPort(1);
		console() << "Opening MIDI port 1" << std::endl;
	}else {
		console() << "No MIDI Ports found!!!!" << std::endl;
	}


    
//setup particles-----------------------------
    
    
    int binPower = 8;
	
	particleSystem.setup(getWindowWidth(), getWindowHeight(), binPower);
    //particleSystem.setup(640, 480, binPower);
	
	kParticles = 10;
	float padding = 0;
	float maxVelocity = .5;
	
    for(int i = 0; i < kParticles * 100; i++) {
		float x = Rand::randFloat(padding, getWindowWidth() - padding);
		float y = Rand::randFloat(padding, getWindowHeight() - padding);
		float xv = Rand::randFloat(-maxVelocity, maxVelocity);
		float yv = Rand::randFloat(-maxVelocity, maxVelocity);
    
	
        Particle particle(x, y, xv, yv);
		particleSystem.add(particle);
	}
   
   
	
	timeStep = 1.0;
	lineOpacity = 0.12f;
	pointOpacity = 0.5f;
	slowMotion = false;
	particleNeighborhood = 2;
	particleRepulsion = 2.5;
	centerAttraction = 0.09;


    
}
//-----------------------MAPEOS------------------------------------------------------

float PelucheOSCApp::mapeos(float val,float inMin,float inMax,float outMin,float outMax)
{
    float temp = val - inMin;
    float rangIn = inMax-inMin;
    float rangOut = outMax-outMin;
    float ratio = rangOut/rangIn;
    float out = (temp*ratio) +  outMin;
    return out;
    
}


//--------------------Mouse---------------------------------------------------------
//----------------------------------------------------------------------------------



void PelucheOSCApp::mouseDown( MouseEvent event )
{
	//isMousePressed = true;
	//mouse = Vec2i(event.getPos());
}

void PelucheOSCApp::mouseUp( MouseEvent event )
{
	//isMousePressed = false;
}

void PelucheOSCApp::mouseDrag( MouseEvent event )
{
	//mouse = Vec2i(event.getPos());
}

//----------------------------------UPDATE-----------------------------------------------

void PelucheOSCApp::update(){
	while (listener.hasWaitingMessages()) {
		osc::Message message;
		listener.getNextMessage(&message);
      
        
		//console() << "New message received" << std::endl;
		console() << "Address: " << message.getAddress() << std::endl;
		console() << "Num Arg: " << message.getNumArgs() << std::endl;
		for (int i = 0; i < message.getNumArgs(); i++) {
			console() << "-- Argument " << i << std::endl;
			console() << "---- type: " << message.getArgTypeName(i) << std::endl;
			if (message.getArgType(i) == osc::TYPE_INT32){
				try {
					console() << "------ value: "<< message.getArgAsInt32(i) << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as int32" << std::endl;
				}
				
			}else if (message.getArgType(i) == osc::TYPE_FLOAT){
				try {
					console() << "------ value: " << message.getArgAsFloat(i) << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as float" << std::endl;
				}
			}else if (message.getArgType(i) == osc::TYPE_STRING){
				try {
					console() << "------ value: " << message.getArgAsString(i).c_str() << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as string" << std::endl;
				}
                
			}
		}

        
       if(message.getAddress()=="/manoI" && message.getNumArgs() != 0)
        {
            miX = message.getArgAsFloat(0);
            miY = message.getArgAsFloat(1);
            
            manoIXmap=  mapeos(miX, -800.0, 800.0, 0.0, getWindowWidth());
            manoIYmap=  mapeos(miY, 1000.0, -700.0, 0.0, getWindowHeight());
            manoI = Vec2f(manoIXmap,manoIYmap);
        

        }
        
        
        
        if(message.getAddress()=="/manoD" && message.getNumArgs() != 0)
        {
            mdX = message.getArgAsFloat(0);
            mdY = message.getArgAsFloat(1);
            manoDXmap =  mapeos(mdX, -800.0, 800.0, 0.0, getWindowWidth());
            manoDYmap =  mapeos(mdY, 1000.0, -700.0, 0.0, getWindowHeight());
            manoD = Vec2f(manoDXmap,manoDYmap);

             
        }
        
        
        if(message.getAddress()=="/torsoManoI" && message.getNumArgs() != 0)
        {
            tmI = message.getArgAsFloat(0);
            tmIF = message.getArgAsFloat(1);
            

        }
		
        
        if(message.getAddress()=="/torsoManoD" && message.getNumArgs() != 0)
        {
            tmD = message.getArgAsFloat(0);
            tmDF = message.getArgAsFloat(1);
            

        }
        
        if(message.getAddress()=="/torso" && message.getNumArgs() != 0)
        {
            torsoX = message.getArgAsFloat(0);
            float torXmap=  mapeos(torsoX, -800.0, 800.0, 0.0, getWindowWidth());
           // console()<< "torsoX" << torXmap<<std::endl;
            
            torsoZ = message.getArgAsFloat(1);
            float torZmap=  mapeos(torsoZ, 800.0, 4000.0, 0.0, getWindowHeight());
            
            //isMousePressed = true;
            torsoParticulas = Vec2f(torXmap,torZmap);// para las particulas mouseDrag!
            
            velocidad = message.getArgAsFloat(2);
            // console()<< "velocidad" << velocidad<<std::endl;
            
        }
        
        
        if(message.getAddress()=="/velPerlinI" && message.getNumArgs() != 0)
        {
            velPerlinI = message.getArgAsFloat(0);
            
        }
        
        
        if(message.getAddress()=="/velPerlinD" && message.getNumArgs() != 0)
        {
            velPerlinD = message.getArgAsFloat(0);
            
        }
        
        if(message.getAddress()=="/cuelloAngulo" && message.getNumArgs() != 0)
        {
            cuelloColor = message.getArgAsFloat(0);
            
        }
        
        
        if(message.getAddress()=="/escena" && message.getNumArgs() != 0)
        {
            escena = message.getArgAsFloat(0);
                if(escena == 3){
                    mIsPressed = true;
                    deAdentro = true;//distanciaAlpha

                    manoBola = false;
                    particulaPress = false;
                    pelosT= false;
                }
                    
                if(escena == 1){
                    manoBola = true;
                    deAdentro = false;//distanciaAlpha

                    mIsPressed = false;
                    particulaPress=false;
                    pelosT=false;
                    }
        
                if(escena == 4){
                    particulaPress = true;
                    mIsPressed=false;
                    manoBola=false;
                    pelosT=false;
                    }
        
                if(escena == 2){
                    pelosT = true;
                    mIsPressed=false;
                    particulaPress=false;
                    manoBola=false;
                    }
        
	}
    }
    
    while (mMidiIn.hasWaitingMessages()) {
		midi::Message message;
		mMidiIn.getNextMessage(&message);
		
		 processMidiMessage(&message);
		}
    
    
      if( mIsPressed == true){
          deAdentro = true;
          
          pelosT = false;
          particulaPress = false;
          manoBola=false;

      }

    
    if( manoBola == true ){
        deAdentro = false;
       
        mIsPressed = false;
        pelosT = false;
        particulaPress = false;
    }
    
    
    if( particulaPress == true ){
       
        mIsPressed = false;
        pelosT = false;
        manoBola=false;
        
    }
    
    if(pelosT == true){
        mIsPressed = false;
        particulaPress = false;
        manoBola=false;
    }
    
    
    //osc::Message mensaje;
	//mensaje.addFloatArg(per6);
	//mensaje.setAddress("/cinder/osc/1");
	//mensaje.setRemoteEndpoint(host, port);
	//sender.sendMessage(mensaje);
}


//------------------------------------Draw---------------------------------------------

void PelucheOSCApp::draw(){
   
    
    if( mIsPressed == true ){
       gl::color(0,0,0,0.1);
       gl::drawSolidRect(Rectf(0,0,getWindowWidth(),getWindowHeight()));
        
        //Vec2f raton = getMousePos(); 
       // Vec2f mouse = Vec2f(positionX,positionY);

        
        //glPushMatrix();
        //glTranslatef(0,10,0);

        reticula(manoD,tmD,tmDF,velPerlinD);// per6, control perlin, ahora con velocidad de manos
        
        //glPopMatrix();
        
        glPushMatrix();
        glTranslatef(0,down,0);
       
        reticula(manoI,tmI,tmIF,velPerlinI);//per7 
        
        glPopMatrix();
       

    }
    
    if( manoBola == true ){
        
        gl::color(0,0,0,0.1);
        gl::drawSolidRect(Rectf(0,0,getWindowWidth(),getWindowHeight()));
        
        //Vec2f rat = getMousePos();
        //Vec2f rats =  Vec2f(getWindowWidth(), getWindowHeight()) - rat;

        reticula(manoD,tmD,tmDF,velPerlinD);// per6, control perlin, ahora con velocidad de manos
        
        glPushMatrix();
        glTranslatef(0,down,0);
       
        reticula(manoI,tmI,tmIF,velPerlinI);//per7
        
        glPopMatrix();
        
        
    }

    if( particulaPress == true ){
      
        particleSystem.setTimeStep(timeStep);
        //Vec2f rata = getMousePos();
        drawpariculas(torsoParticulas); // balance!!
        
    }
   
    if(pelosT == true){
        
        particleSystem.setTimeStep(timeStep);
       // Vec2f ratas = getMousePos();
        //Vec2f ratilla =  Vec2f(getWindowWidth(), getWindowHeight())-ratas;
        
        drawpariculas2(manoD,manoI);
    

    }
    
    
}
//---------------------------------------------------------------------------------------
//-----------------------------Funciones de control--------------------------------------

void PelucheOSCApp::reticula(Vec2f mouse,float posManoTorso,float tamCuad,float valP){
    
    //gl::clear();
    gl::enableAlphaBlending();
    gl::enableAdditiveBlending();
   //glBegin(GL_QUADS);
   
    

    for(int y = getWindowHeight()/4; y < (getWindowHeight()*3)/4; y += paso ){
        for (int x = 0; x < getWindowWidth(); x += paso){
            
            
            
            Vec2f cPos = Vec2f(x,y);            
            Vec2f diferencia = mouse - cPos;
            

            float angulo = atan2(diferencia.x,diferencia.y);

           
            float alfa;
            
            if(deAdentro){
                alfa = diferencia.length()/(getWindowWidth()/4);

            }else{ alfa = 1- diferencia.length()/(getWindowWidth()/4);

            }
            
            
            
  Vec3f perlo = mperlin.dfBm(Vec3f(x,y,getElapsedSeconds()*100)*0.01*(sliderValue + valP)); // !!!!!
            
            ColorAT<float> t2(CM_HSV,(per3/127.0f), (per4/127.0f)+perlo.x, per5/127.0f,alfa );
            gl::color(t2);
            
            
            glPushMatrix();
            
            glTranslatef(x,y-down,0);
            gl::rotate((angulo + perlo.y*pi/6) * 180/pi);

          
            //glRotatef(per2 * 2*pi, 0, 0, 1); // otro recurso locochón
            //glRotatef(getMousePos().y, 0, 1, 0);
            /*
             glVertex2f(x, y);
             glVertex2f(x +((paso/2) + per2)*perlo.x, y +((paso/2) +per2)*perlo.y);
             glVertex2f(x + sl3, y + sl4);
             glVertex2f(x+ per1, y+per1 );
            */
            
            
            gl::drawSolidRect(Rectf(0, 0, ((paso/2) + per2+ tamCuad)*perlo.x,  ((paso/2) +per2 +tamCuad)*perlo.y));
          //gl::drawLine(Vec2f(0+sl3,0 + sl4), Vec2f( per1, per1 ));
            glLineWidth(3.0);
            gl::drawLine(Vec2f(0+sl3,0 + sl4), Vec2f( 0+posManoTorso, 0+posManoTorso ));
            //gl::drawSolidEllipse(Vec2f(0,0), paso/4, paso/4);
            
            

        
         glPopMatrix();
             
            
            
        }
    }
    
gl::disableAlphaBlending();
 //glEnd();
}
//----------------------------fuerzas particulas-------------------------
/*
void PelucheOSCApp::fuerzaPrticula(){
    
    particleSystem.setupForces();
    
    for(int i = 0; i < particleSystem.size(); i++) {
        
        //glBegin(GL_QUAD_STRIP);
        //glBegin(GL_LINE_STRIP);
        
		Particle& cur = particleSystem[i];
		// global force on other particles
		particleSystem.addRepulsionForce(cur, particleNeighborhood + per7, particleRepulsion);
		// forces on this particle
		cur.bounceOffWalls(0, 0, getWindowWidth(), getWindowHeight());
		cur.addDampingForce();
        //glEnd();
	}
     particleSystem.update();
    
}
*/
//------------------------drawParticulas-------------------------------------------------------------

void PelucheOSCApp::drawpariculas(Vec2f rata){
    
    //gl::clear();
    
    gl::color(0,0,0,0.7);
    gl::drawSolidRect(Rectf(0,0,getWindowWidth(),getWindowHeight()));
   
    //Vec2f rata = getMousePos();
	
    gl::enableAlphaBlending();
    gl::enableAdditiveBlending();

    
    particleSystem.setupForces();
    
    for(int i = 0; i < particleSystem.size(); i++) {
        
		Particle& cur = particleSystem[i];
		// global force on other particles
		particleSystem.addRepulsionForce(cur, particleNeighborhood + per7, particleRepulsion);
		// forces on this particle
		cur.bounceOffWalls(0, 0, getWindowWidth(), getWindowHeight());
		cur.addDampingForce();
        //glEnd();
	}
	
    if(atraeCentro){
    particleSystem.addAttractionForce(getWindowWidth()/2, getWindowHeight()/2, getWindowWidth(),centerAttraction + 0.9);
    particleSystem.addRepulsionForce(rata.x, rata.y, per6, 7);
	}else{
        particleSystem.addAttractionForce(rata.x, rata.y, getWindowWidth(),centerAttraction+0.9);}
    
    //if(isMousePressed)
    //particleSystem.addRepulsionForce(torsoParticulas.x, torsoParticulas.y, 100, 5*velocidad);
    
    
    particleSystem.update();

	
    particleSystem.draw( per1, per2, per3, per4, per5, sl3, sl4, sliderValue);
        
	gl::disableAlphaBlending();

    
   
    
}

//--------------------------------------drawParticulas2-------------
void PelucheOSCApp::drawpariculas2( Vec2f _manoD, Vec2f _manoI){
    
    gl::clear();
   // Vec2f ratas = getMousePos();
    //Vec2f ratilla =  Vec2f(getWindowWidth(), getWindowHeight())-ratas;
	
    gl::enableAlphaBlending();
	
   // gl::color(0,0,0,0.2);
    //gl::drawSolidRect(Rectf(0,0,getWindowWidth(),getWindowHeight()));
    

    gl::enableAdditiveBlending();

	
   particleSystem.setupForces();

	for(int i = 0; i < particleSystem.size(); i++) {
        
        glBegin(GL_LINE_STRIP);
        //glBegin(GL_QUAD_STRIP);
        
		Particle& cur = particleSystem[i];
	
		particleSystem.addRepulsionForce(cur, particleNeighborhood + per7, particleRepulsion);

		cur.bounceOffWalls(0, 0, getWindowWidth(), getWindowHeight());
		cur.addDampingForce();
        
        glEnd();
        
	}
    

    if(atraeCentro){
        particleSystem.addAttractionForce(getWindowWidth()/2, getWindowHeight()/2, getWindowWidth(),centerAttraction);
        particleSystem.addRepulsionForce(_manoD.x, _manoD.y, per6, 9);
        particleSystem.addRepulsionForce(_manoI.x, _manoI.y, per6, 9);
	}else{
        particleSystem.addAttractionForce(_manoD.x, _manoD.y, getWindowWidth(),centerAttraction+0.9);
        particleSystem.addAttractionForce(_manoI.x, _manoI.y, getWindowWidth(),centerAttraction+0.9);
    }
    
    
    
	// single global forces
    //if(isMousePressed)
    //particleSystem.addRepulsionForce(torsoParticulas.x, torsoParticulas.y, 100, 5*velocidad);
    //particleSystem.addRepulsionForce(ratas.x, ratas.y, per6, 5);
    
    particleSystem.update();
	
    particleSystem.draw2(sliderValue,  per3,  per4, per5);
    
// console()<< "manoD" << _manoD<<std::endl;
    
	gl::disableAlphaBlending();

    
}



//-------------------------------------------------------------------

void PelucheOSCApp:: keyDown(KeyEvent event){
    
    char tecla = event.getChar();
    
    if(tecla == 'f'){
        setFullScreen(!isFullScreen());}
    
     if(tecla == '+'){ paso++;}
     if(tecla == '-'){ paso--;}
    
     if(tecla == 'o'){deAdentro= !deAdentro;}
    
     if(tecla == 'c'){ crece++;}
     if(tecla == 'd'){ crece--;}
    
     if(tecla == 'm'){ down++;}
    
    if( event.getChar() == 's' ) {
		slowMotion = !slowMotion;
		if(slowMotion)
			timeStep = 0.05;
		else
			timeStep = 0.91;
	}

    
}


//------------------------------------------------MIDI------------------------------------
//------------------------------------------------MIDI------------------------------------

void PelucheOSCApp::processMidiMessage(midi::Message* message){
//console() << "midi port: " << message->port << " ch: " << message->channel << " status: " << message->status;
console() << " byteOne: " << message->byteOne << " byteTwo: " << message->byteTwo << std::endl;
	
	switch (message->status) {
		case MIDI_CONTROL_CHANGE:
			if (message->byteOne == SLIDER_NOTE){
				sliderValue = message->byteTwo / 127.0f;
			}
           //break;
            
            if (message->byteOne == 14){
				per1 = message-> byteTwo* 1.0f;
			}
            //break;
            
            if (message->byteOne == 15){
				per2 = message-> byteTwo * 1.0f;
			}
            
            if (message->byteOne == 23){
				bot1 = message-> byteTwo/127.0f;
                if(bot1 == 1.0){    
                        mIsPressed = true;
                        particulaPress=false;
                        pelosT=false;
                        manoBola = false;
                    
                }else{    mIsPressed = false;}
			}
            
            if (message->byteOne == 24){
				bot2 = message-> byteTwo/127.0f;
                if(bot2 == 1.0){
                    particulaPress = true;
                    mIsPressed = false;
                    pelosT=false;
                    manoBola = false;
                }else{particulaPress = false;}
            }
            
            if (message->byteOne == 16){
				per3 = message-> byteTwo* 1.0f;
			}
            if (message->byteOne == 17){
				per4 = message-> byteTwo* 1.0f;
			}
            
            if (message->byteOne == 18){
				per5 = message-> byteTwo* 1.0f;
			}
            if (message->byteOne == 4){
				sl3 = message-> byteTwo * 1.0f;
			}
            
            if (message->byteOne == 5){
				sl4 = message-> byteTwo * 1.0f;
			}
            if (message->byteOne == 19){
				per6 = message-> byteTwo;
			}
            if (message->byteOne == 20){
				per7 = message-> byteTwo;
			}
            
            if (message->byteOne == 28){
				bot6 = message-> byteTwo/127.0f;
                if(bot6 == 1.0 ){atraeCentro = false;}else{atraeCentro = true;}
			}
            if (message->byteOne == 33){
				bot7 = message-> byteTwo/127.0f;
                if(bot7 == 1.0 ){
                    pelosT = true;
                    mIsPressed= false;
                    particulaPress=false;
                    manoBola = false;

                    
                }else{pelosT = false;}
			}
            
            
            if (message->byteOne == 34){
				bot22 = message-> byteTwo/127.0f;
                if(bot22 == 1.0 ){
                    manoBola = true;
                    pelosT = false;
                    mIsPressed= false;
                    particulaPress=false;
                    
                }else{manoBola = false;}
			}
           
            
			break;
	}
}



CINDER_APP_BASIC( PelucheOSCApp, RendererGl )
