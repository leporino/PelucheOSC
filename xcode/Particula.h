#pragma once
#include <vector>
#include <cmath>
#include "cinder/gl/gl.h"
#include "cinder/Perlin.h"
#include "cinder/app/AppBasic.h"

using namespace std;
using namespace ci;
using namespace ci::app; 

class Particle {
public:
    Perlin mperlo;
	float x, y;
	float xv, yv;
	float xf, yf;
	Particle(float _x = 0, float _y = 0,
             float _xv = 0, float _yv = 0) :
    x(_x), y(_y),
    xv(_xv), yv(_yv) {
	}
	void updatePosition(float timeStep) {
		// f = ma, m = 1, f = a, v = int(a)
		xv += xf;
		yv += yf;
		x += xv * timeStep;
		y += yv * timeStep;
	}
	void resetForce() {
		xf = 0;
		yf = 0;
	}
	void bounceOffWalls(float left, float top, float right, float bottom, float damping = .3) {
		bool collision = false;
        
		if (x > right){
			x = right;
			xv *= -1;
			collision = true;
		} else if (x < left){
			x = left;
			xv *= -1;
			collision = true;
		}
        
		if (y > bottom){
			y = bottom;
			yv *= -1;
			collision = true;
		} else if (y < top){
			y = top;
			yv *= -1;
			collision = true;
		}
        
		if (collision == true){
			xv *= damping;
			yv *= damping;
		}
	}
	void addDampingForce(float damping = .01) {
		xf = xf - xv * damping;
        yf = yf - yv * damping;
	}
	void draw(float per1,float per2,float per3,float per4,float per5,float sl3,float sl4,float sliderValue) {
      
		//glVertex2f(x, y);
        
        Vec3f perloPar = mperlo.dfBm(Vec3f(x,y,getElapsedSeconds()*500)*0.005*sliderValue);
        ColorAT<float> t3(CM_HSV,(per3/127.0f), (per4/127.0f)+perloPar.x, per5/127.0f,0.9 );
        
        gl::color(t3);
        glPushMatrix();
        glTranslatef(x,y,0);
        //gl::rotate((angulo + perloPar.y*pi/6) * 180/pi);
        gl::drawSolidRect(Rectf(0, 0, ((20/2) + per2)*perloPar.x,  ((20/2) +per2)*perloPar.y));
        //gl::drawSolidEllipse(Vec2f(0, 0), ((20/2) + per2)*perloPar.x,  ((20/2) +per2)*perloPar.y);
       //gl::drawStrokedRoundedRect(Rectf(0, 0, ((20/2) + per2)*perloPar.x,  ((20/2) +per2)*perloPar.y), -10.0+perloPar.x);
        gl::drawLine(Vec2f(0+sl3,0 + sl4), Vec2f( 0 + per1, 0+per1 ));
        
        glPopMatrix();

        
	}
    
    
    void draw2(float sliderValue, float per3, float per4,float per5){
        
        Vec3f perlon = mperlo.dfBm(Vec3f(x,y,getElapsedSeconds()*100)*0.001*(sliderValue)); // !!!!!
        
        ColorAT<float> t4(CM_HSV,(per3/127.0f), (per4/127.0f)+perlon.x, per5/127.0f,0.19);
        gl::color(t4);

		glVertex2f(x, y);
   
      
        
	}
};




