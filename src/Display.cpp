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
      total_( Position( 100, 100 ), FACE::TOP ) {
}

void Display::setup() {
    Serial.println( "Setup: Display: Started" );
    if( epd_.Init( lut_full_update ) != 0 ) {
        Serial.print( "e-Paper init failed" );
        return;
    }

    unsigned char* image = new unsigned char[4096];
    Paint paint( image, 0, 0 );
    paint.SetRotate( ROTATE_0 );
    paint.SetWidth( 200 );
    paint.SetHeight( 200 );
    paint.Clear( WHITE );
    paint.DrawRectangle( 34, 34, 165, 165, BLACK );
    paint.DrawLine( 0, 0, 34, 34, BLACK );
    paint.DrawLine( 0, 199, 34, 199 - 34, BLACK );
    paint.DrawLine( 199, 0, 199 - 34, 34, BLACK );
    paint.DrawLine( 199, 199, 199 - 34, 199 - 34, BLACK );

    for( size_t i = 0; i < 4; ++i )
        faceField_[i].setText( "00:00:00" );
    total_.setText( "00:00:00" );

    // set it to screen and to frame buffer
    epd_.SetFrameMemory( paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight() );
    for( size_t i = 0; i < 4; ++i )
        faceField_[i].draw( epd_, &Font24 );
    total_.draw( epd_, &Font20 );
    epd_.DisplayFrame();

    epd_.SetFrameMemory( paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight() );
    delete[] image;
    for( size_t i = 0; i < 4; ++i )
        faceField_[i].draw( epd_, &Font24 );
    total_.draw( epd_, &Font20 );
    
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
    faceField_[static_cast< int >( upface )].setText( formatTime( ms ) );
    faceField_[static_cast< int >( upface )].draw( epd_, &Font24 );
    epd_.DisplayFrame();
    faceField_[static_cast< int >( upface )].draw( epd_, &Font24 );
}

void Display::setTotalDuration( unsigned long ms ) {
    total_.setText( formatTime( ms ) );
    total_.draw( epd_, &Font20 );
    epd_.DisplayFrame();
    total_.draw( epd_, &Font20 );
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
    total_.wipe( epd_ );
    epd_.DisplayFrame();
    total_.wipe( epd_ );

    total_.setUpface( upface );

    total_.draw( epd_, &Font20 );
    epd_.DisplayFrame();
    total_.draw( epd_, &Font20 );
}
