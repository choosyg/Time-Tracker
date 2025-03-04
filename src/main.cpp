#include <Arduino.h>
#include <string>
#include <array>

#include <Arduino_LSM9DS1.h> //IMU unit of nano 33 BLE

//Headers for e-paper display
#include <SPI.h>
#include "epd1in54.h"
#include "epdpaint.h"

struct Position{
  int x;
  int y;
  Position() = default;
  Position( int x_, int y_ ): x(x_), y(y_){}
};

const int BLACK = 0;
const int WHITE = 1;
enum class FACE { TOP=0, BOTTOM=1, LEFT=2, RIGHT=3, FRONT=4, BACK=5 };

class Field{
  public:
    //center given in "visual" coordinates, not logical display coords. Means the origin is the corner of the display that is currently visually top/left
	// Upface describes the current upper side of the Cube
    Field( const Position& center, FACE upface ): center_(center), upface_(upface), paint_(image_,0,0){
    }

    const Position& center() const { return center_; }
    void setCenter( const Position &center ){ 
      center_ = center; 
    }

    FACE upface() const { return upface_; }
    void setUpface( FACE upface ){
      upface_ = upface;
    }

    void setText( const std::string& str ){
      str_ = str;
    }
	
	void wipe( Epd& epd, sFONT* font ){
		std::string backup = str_;
		draw( epd, font );
		str_ = backup;
	}

    void draw( Epd& epd, sFONT* font ){
	  if( upface_ >= FACE::FRONT ){
		  return;
	  }
      int w = font->Width * str_.length();
      int h = font->Height;
      switch( upface_ ){
        case FACE::TOP: paint_.SetRotate( ROTATE_0 ); 
                  paint_.SetWidth( w ); 
                  paint_.SetHeight( h ); 
                  break;
        case FACE::BOTTOM: paint_.SetRotate( ROTATE_180 ); 
                     paint_.SetWidth( w ); 
                     paint_.SetHeight( h ); 
                     break;
        case FACE::LEFT: paint_.SetRotate( ROTATE_270 ); 
                   paint_.SetWidth( h ); 
                   paint_.SetHeight( w ); 
                   break;
        case FACE::RIGHT: paint_.SetRotate( ROTATE_90 ); 
                    paint_.SetWidth( h ); 
                    paint_.SetHeight( w ); 
                    break;
      }
	    paint_.Clear( WHITE );
      paint_.DrawStringAt(0, 0, str_.c_str(), font, BLACK);
      
	  Position topleft = center_;
	  topleft.x -= paint_.GetWidth() / 2;
	  topleft.y -= paint_.GetHeight() /2;
	  Position pos;
      switch( upface_ ){
		case FACE::TOP: pos = topleft;
        case FACE::BOTTOM: pos.x = 200-topleft.x-paint_.GetWidth();
                     pos.y = 200-topleft.y-paint_.GetHeight();
                     break;
        case FACE::LEFT:   pos.x = topleft.y;
                     pos.y = 200-topleft.x-paint_.GetHeight();
                     break;
        case FACE::RIGHT:  pos.x = 200-topleft.y-paint_.GetWidth();
                     pos.y = topleft.x;
                     break;
      }
      
      epd.SetFrameMemory(paint_.GetImage(), pos.x, pos.y, paint_.GetWidth(), paint_.GetHeight());
    }
  private:
    unsigned char image_[1024];
    Position center_;
    FACE upface_;
    Paint paint_;
    std::string str_;
  };


unsigned char image[1024];
Paint paint(image, 0, 0);
Epd epd_;

class Display{
public:
  Display() :
    faceField_{Field( Position(100,17), FACE::TOP ),Field( Position(100,17), FACE::BOTTOM ),Field( Position(100,17), FACE::LEFT ),Field( Position(100,17), FACE::RIGHT )},
    total_( Position(100,100), FACE::TOP )
  {  
  }
  
  void setup(){

    Serial.print("e-Paper init");
    if (epd_.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
    }
     
    //unsigned char* image = new unsigned char[1024];
    //Paint paint(image, 0, 0);
    paint.SetRotate( ROTATE_0 );
    paint.SetWidth(200);
    paint.SetHeight(200);
    paint.Clear(WHITE);
    paint.DrawRectangle(35, 35, 165, 165, BLACK);
    paint.DrawLine(0, 0, 35, 35, BLACK);
    paint.DrawLine(0, 200, 35, 200-35, BLACK);
    paint.DrawLine(200, 0, 200-35, 35, BLACK);
    paint.DrawLine(200, 200, 200-35, 200-35, BLACK);
    
    //for( size_t i=0; i<4; ++i ) faceField_[i].setText( "00:00:00" );
	  //total_.setText( "00:00:00" );
    
    //set it to screen and to frame buffer
    epd_.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
    //for( size_t i=0; i<4; ++i ) faceField_[i].draw( epd_, &Font24 );
    //total_.draw( epd_, &Font24 );
    epd_.DisplayFrame();
	
    epd_.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
    //delete[] image;
    //for( size_t i=0; i<4; ++i ) faceField_[i].draw( epd_, &Font24 );
	  //total_.draw( epd_, &Font24 );
    epd_.DisplayFrame();
return;
    if (epd_.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
    }
  }
  
  void setFaceDuration( unsigned long ms, FACE upface ){
	if( upface >= FACE::FRONT ){
	  return;
	}
	faceField_[static_cast<int>(upface)].setText( formatTime( ms ) );
    faceField_[static_cast<int>(upface)].draw( epd_, &Font24 );
    epd_.DisplayFrame();
    faceField_[static_cast<int>(upface)].draw( epd_, &Font24 );
  }
  
  void setTotalDuration( unsigned long ms ){
    total_.setText( formatTime( ms ) );
    total_.draw( epd_, &Font20 );
    epd_.DisplayFrame();
    total_.draw( epd_, &Font20 );
  }
  
  FACE upface() const {
	  return total_.upface();
  }
  
  void setUpface( FACE upface ){
	if( upface == total_.upface() ){
	  return;
	}
	if( upface >= FACE::FRONT ){
	  total_.setUpface( upface );
	  return;
	}
	total_.wipe( epd_, &Font20 );
    epd_.DisplayFrame();
    total_.wipe( epd_, &Font20 );	
	
	total_.setUpface( upface ); 
	
    total_.draw( epd_, &Font20 );
    epd_.DisplayFrame();
    total_.draw( epd_, &Font20 );	
  }
  
private:
  std::string format( unsigned long n ){
	  if( n<10 ) {
		  return std::string("0") + std::to_string(n);
	  }
	  return std::to_string(n);
  }
  std::string formatTime( unsigned long ms ){
    unsigned long hours = ms / (1000 * 60 * 60);
    ms -= hours*60*60*1000;
    unsigned long minutes = ms / (1000*60);
    ms -= minutes*60*1000;
    unsigned long seconds = ms / 1000;
	return format( hours ) + ":" + format( minutes ) + ":" + format( seconds );
  }

  Field faceField_[4];
  Field total_;
  //Epd epd_;
};


unsigned long time_last;
unsigned long faceDuration[4] = {0,0,0,0};
Display display;

void setup() {
  Serial.begin(115200);

  Serial.print("IMU init");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("e-Paper init");
    if (epd_.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
    }
     
    //unsigned char* image = new unsigned char[1024];
    //Paint paint(image, 0, 0);
    paint.SetRotate( ROTATE_0 );
    paint.SetWidth(200);
    paint.SetHeight(200);
    paint.Clear(WHITE);
    paint.DrawRectangle(35, 35, 165, 165, BLACK);
    paint.DrawLine(0, 0, 35, 35, BLACK);
    paint.DrawLine(0, 200, 35, 200-35, BLACK);
    paint.DrawLine(200, 0, 200-35, 35, BLACK);
    paint.DrawLine(200, 200, 200-35, 200-35, BLACK);
    
    //for( size_t i=0; i<4; ++i ) faceField_[i].setText( "00:00:00" );
	  //total_.setText( "00:00:00" );
    
    //set it to screen and to frame buffer
    epd_.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
    //for( size_t i=0; i<4; ++i ) faceField_[i].draw( epd_, &Font24 );
    //total_.draw( epd_, &Font24 );
    epd_.DisplayFrame();
	
    epd_.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
    //delete[] image;
    //for( size_t i=0; i<4; ++i ) faceField_[i].draw( epd_, &Font24 );
	  //total_.draw( epd_, &Font24 );
    epd_.DisplayFrame();

  //display.setup();
  time_last = millis();
}

void loop() {
  /*
  if (IMU.accelerationAvailable())
  {
    float x, y, z, delta = 0.1;
    IMU.readAcceleration(x, y, z);

    if(z>=1-delta ){
      Serial.println("Up-Face = TOP");
      display.setUpface( FACE::TOP );
    } else if( z<= -1+delta ){
      Serial.println("Up-Face = BOTTOM");
      display.setUpface( FACE::BOTTOM );
    }else if( y >= 1 - delta ){
      Serial.println("Up-Face = LEFT");
      display.setUpface( FACE::LEFT );
    } else if( y <= -1 + delta ){
       Serial.println("Up-Face = RIGHT");
       display.setUpface( FACE::RIGHT );
    } else if ( x<=-1+delta ) {
      Serial.println("Reset Down-Face = Display");
	  for( size_t i=0; i<4; ++i ){
	    faceDuration[i]=0;
		  display.setFaceDuration( 0, static_cast<FACE>(i) );
	  }
	  display.setTotalDuration( 0 );
	  
	  display.setUpface( FACE::BACK );
    }
  }

  if( display.upface() == FACE::BACK ){
	  time_last = millis();
	  return;
  }
  
  unsigned long time_now = millis();
  faceDuration[ static_cast<int>(display.upface()) ] += time_now-time_last;
  display.setFaceDuration( faceDuration[ static_cast<int>(display.upface()) ], display.upface() );
  time_last = time_now;

  display.setTotalDuration( faceDuration[ 0 ]+faceDuration[ 1 ]+faceDuration[ 2 ]+faceDuration[ 3 ] );
  
  //TODO draw temperature & humidity & Date
*/
Serial.println("TEST");
  delay(100);
}
