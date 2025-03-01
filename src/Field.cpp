#include "Field.h"

#include <string>

Field::Field( const Position& center, FACE upface ) : center_( center ), upface_( upface ), paint_( image_, 0, 0 ) {
}

const Position& Field::center() const {
    return center_;
}
void Field::setCenter( const Position& center ) {
    center_ = center;
}

FACE Field::upface() const {
    return upface_;
}

void Field::setUpface( FACE upface ) {
    upface_ = upface;
}

void Field::setText( const std::string& str ) {
    str_ = str;
}

void Field::wipe( Epd& epd ) {
    paint_.Clear( WHITE );
    epd.SetFrameMemory( paint_.GetImage(), lastDrawPos_.x, lastDrawPos_.y, paint_.GetWidth(), paint_.GetHeight() );
}

void Field::draw( Epd& epd, sFONT* font ) {
    if( upface_ >= FACE::FRONT ) {
        return;
    }
    
    int w = font->Width * str_.length();
    int h = font->Height;

    switch( upface_ ) {
        case FACE::TOP:
            paint_.SetRotate( ROTATE_0 );
            paint_.SetWidth( w );
            paint_.SetHeight( h );
            break;
        case FACE::BOTTOM:
            paint_.SetRotate( ROTATE_180 );
            paint_.SetWidth( w );
            paint_.SetHeight( h );
            break;
        case FACE::LEFT:
            paint_.SetRotate( ROTATE_270 );
            paint_.SetWidth( h );
            paint_.SetHeight( w );
            break;
        case FACE::RIGHT:
            paint_.SetRotate( ROTATE_90 );
            paint_.SetWidth( h );
            paint_.SetHeight( w );
            break;
    }
    paint_.Clear( WHITE );
    paint_.DrawStringAt( 0, 0, str_.c_str(), font, BLACK );
    
    //calculate draw position, which is the top left of the text-square w x h
    //note that the paint extended its with to be a multiple of 8, so for x we need to adjust
    switch( upface_ ) {
        case FACE::TOP:
            lastDrawPos_ = center_;
            lastDrawPos_.x -= w / 2; //no need to adjust, since text starts at 0,0
            lastDrawPos_.y -= h / 2;
            break;
        case FACE::BOTTOM:
            lastDrawPos_.x = epd.width - center_.x - w/2 - (paint_.GetWidth()-w);
            lastDrawPos_.y = epd.height - center_.y - h/2 - (paint_.GetHeight()-h)-1;
            break;
        case FACE::LEFT:
            lastDrawPos_.x = center_.y + 2 - static_cast<int>(h/2);
            lastDrawPos_.y = epd.height - center_.x - w/2 - (paint_.GetHeight()-w);
            break;
        case FACE::RIGHT:
            lastDrawPos_.x = epd.width - center_.y - h/2 - (paint_.GetWidth()-h) +3;
            lastDrawPos_.y = center_.x - w/2;
            break;
    }

    /*Serial.println( "---------------------------------------------------------------------------------------" );
    Serial.println( ("Center: " + std::to_string( center_.x ) + " / " + std::to_string( center_.y )).c_str() );
    Serial.println( ("Size: " + std::to_string( w ) + " / " + std::to_string( h )).c_str() );
    Serial.println( ("Upface: " + std::to_string( static_cast<int>( upface_ ) )).c_str() );
    Serial.println( ("Size Paint: " + std::to_string( paint_.GetWidth() ) + " / "
                    + std::to_string( paint_.GetHeight() )).c_str() );
    Serial.println( ("top-left: " + std::to_string( lastDrawPos_.x ) + " / " + std::to_string( lastDrawPos_.y )).c_str() );
    Serial.println( "---------------------------------------------------------------------------------------" );
*/
    epd.SetFrameMemory( paint_.GetImage(), lastDrawPos_.x, lastDrawPos_.y, paint_.GetWidth(), paint_.GetHeight() );
}
