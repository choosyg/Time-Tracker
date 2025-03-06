#include "Field.h"

Field::Field( const Position& center, FACE upface ) : center_( center ), upface_( upface ) {
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

void Field::wipe( Paint& paint ) {
    switch( upface_ ) {
        case FACE::TOP:
            paint.SetRotate( ROTATE_0 );
            break;
        case FACE::BOTTOM:
            paint.SetRotate( ROTATE_180 );
            break;
        case FACE::LEFT:
            paint.SetRotate( ROTATE_270 );
            break;
        case FACE::RIGHT:
            paint.SetRotate( ROTATE_90 );
            break;
        default: return;
    }
    paint.DrawFilledRectangle( rect_.topLeft.x, rect_.topLeft.y, rect_.bottomRight.x, rect_.bottomRight.y, WHITE );
    paint.SetRotate( ROTATE_0 );
}

void Field::draw( const std::string& str, Paint& paint, sFONT* font ) {
    if( upface_ >= FACE::FRONT ) {
        return;
    }
    
    int w = font->Width * str.length();
    int h = font->Height;
    rect_.topLeft = center_;
    rect_.topLeft.x -= w / 2; //no need to adjust, since text starts at 0,0
    rect_.topLeft.y -= h / 2;
    rect_.bottomRight.x = rect_.topLeft.x+w;
    rect_.bottomRight.y = rect_.topLeft.y+h;

    switch( upface_ ) {
        case FACE::TOP:
            paint.SetRotate( ROTATE_0 );
            break;
        case FACE::BOTTOM:
            paint.SetRotate( ROTATE_180 );
            break;
        case FACE::LEFT:
            paint.SetRotate( ROTATE_270 );
            break;
        case FACE::RIGHT:
            paint.SetRotate( ROTATE_90 );
            break;
        default: return;
    }

    paint.DrawFilledRectangle( rect_.topLeft.x, rect_.topLeft.y, rect_.bottomRight.x, rect_.bottomRight.y, WHITE );
    paint.DrawStringAt( rect_.topLeft.x, rect_.topLeft.y, str.c_str(), font, BLACK );
    paint.SetRotate( ROTATE_0 );
}
