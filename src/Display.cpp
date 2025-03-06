#include "Display.h"

#include <string>

namespace {

std::string format( unsigned long n ) {
    if( n < 10 ) {
        return std::string( "0" ) + std::to_string( n );
    }
    return std::to_string( n );
}

std::string formatTime( unsigned long ms ) {
    unsigned long hours = ms / ( 1000 * 60 * 60 );
    ms -= hours * 60 * 60 * 1000;
    unsigned long minutes = ms / ( 1000 * 60 );
    ms -= minutes * 60 * 1000;
    unsigned long seconds = ms / 1000;
    return format( hours ) + ":" + format( minutes ) + ":" + format( seconds );
}

} // namespace

Display::Display()
    : faceField_{ Field( Position( 100, 18 ), FACE::TOP ),
                  Field( Position( 100, 18 ), FACE::BOTTOM ),
                  Field( Position( 100, 18 ), FACE::LEFT ),
                  Field( Position( 100, 18 ), FACE::RIGHT ) },
      total_( Position( 100, 100 ), FACE::TOP ), 
      paint_(image_,0,0) {
}

void Display::setup() {
    Serial.println( "Setup: Display: Started" );
    if( epd_.Init( lut_full_update ) != 0 ) {
        Serial.print( "e-Paper init failed" );
        return;
    }

    Serial.println( "Setup: Display: Drawing Frame" );
    paint_.SetRotate( ROTATE_0 );
    paint_.SetWidth( 200 );
    paint_.SetHeight( 200 );
    paint_.Clear( WHITE );
    paint_.DrawRectangle( 34, 34, 165, 165, BLACK );
    paint_.DrawLine( 0, 0, 34, 34, BLACK );
    paint_.DrawLine( 0, 199, 34, 199 - 34, BLACK );
    paint_.DrawLine( 199, 0, 199 - 34, 34, BLACK );
    paint_.DrawLine( 199, 199, 199 - 34, 199 - 34, BLACK );

    Serial.println( "Setup: Display: Drawing Fields" );
    for( size_t i = 0; i < 4; ++i )
        faceField_[i].draw( formatTime( 0 ), paint_, &Font24 );
    total_.draw( formatTime( 0 ), paint_, &Font20 );
    
    // set it to screen and to frame buffer
    Serial.println( "Setup: Display: Showing on screen" );
    epd_.SetFrameMemory( paint_.GetImage(), 0, 0, paint_.GetWidth(), paint_.GetHeight() );
    epd_.DisplayFrame();
    epd_.SetFrameMemory( paint_.GetImage(), 0, 0, paint_.GetWidth(), paint_.GetHeight() );
    
    if( epd_.Init( lut_partial_update ) != 0 ) {
        Serial.print( "e-Paper init failed" );
        return;
    }
    Serial.println( "Setup: Display: Success" );
}

void Display::setFaceDuration( unsigned long ms, FACE upface ) {
    if( upface >= FACE::FRONT ) {
        return;
    }
    auto idx = static_cast< int >( upface );
    faceField_[idx].draw( formatTime( ms ), paint_, &Font24 );
    updateDisplay();
}

void Display::setTotalDuration( unsigned long ms ) {
    total_.draw( formatTime( ms ), paint_, &Font20 );
    updateDisplay();
}

FACE Display::upface() const {
    return total_.upface();
}

void Display::setUpface( FACE upface ) {
    if( upface == total_.upface() ) {
        return;
    }
    if( upface >= FACE::FRONT ) {
        total_.setUpface( upface );
        return;
    }
    total_.wipe( paint_ );
    updateDisplay();
    total_.setUpface( upface );
}

void Display::updateDisplay(){
    if( paint_.maxx < paint_.minx ){
        return;
    }
    /*
	int x = paint_.minx;
    x -= x%8;
	int y = paint_.miny;
	int w = paint_.maxx - x+1;
	int h = paint_.maxy - paint_.miny+1;
	
	unsigned char si = new unsigned char[w*h];
	int idx = 0;
	for( int j=paint_.miny; j<=paint_.maxy; ++j ){
		for( int i=paint_.minx; i<=paint_.maxx; i+=8 ){
			si[ idx ] = image_[(i + j * 200) / 8];
			idx++;
		}
	}
*/
    // set it to screen and to frame buffer
    epd_.SetFrameMemory( paint_.GetImage(), 0, 0, paint_.GetWidth(), paint_.GetHeight() );
	//epd_.SetFrameMemory( paint_.GetImage(), x, y, w, h );
    epd_.DisplayFrame();
    //epd_.SetFrameMemory( paint_.GetImage(), x, y, w, h );

    paint_.minx = 200;
    paint_.maxx = 0;
    paint_.miny = 200;
    paint_.maxy = 0;
}
