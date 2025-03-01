#pragma once

#include "Position.h"

#include "epd/epd1in54.h"
#include "epd/epdpaint.h"
#include "epd/fonts.h"

#include <string>

const int BLACK = 0;
const int WHITE = 1;
enum class FACE { TOP = 0, BOTTOM = 1, LEFT = 2, RIGHT = 3, FRONT = 4, BACK = 5 };

class Field {
public:
    // center given in "visual" coordinates, not logical display coords. Means the origin is the corner of the display
    // that is currently visually top/left
    //  Upface describes the current upper side of the Cube
    Field( const Position& center, FACE upface );

    const Position& center() const;
    void setCenter( const Position& center );

    FACE upface() const;
    void setUpface( FACE upface );

    void setText( const std::string& str );

    void wipe( Epd& epd );
    void draw( Epd& epd, sFONT* font );

private:
    unsigned char image_[4096];
    Position center_;
    FACE upface_;
    Paint paint_;
    std::string str_;

    Position lastDrawPos_;
};
