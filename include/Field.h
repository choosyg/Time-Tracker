#pragma once

#include "Position.h"

#include "epd/epdpaint.h"
#include "epd/fonts.h"

#include <string>

const int BLACK = 0;
const int WHITE = 1;
enum class FACE { TOP = 0, BOTTOM = 1, LEFT = 2, RIGHT = 3, FRONT = 4, BACK = 5 };

class Field {
public:
    Field( const Position& center, FACE upface );

    const Position& center() const;
    void setCenter( const Position& center );

    FACE upface() const;
    void setUpface( FACE upface );

    void wipe( Paint& paint );
    void draw( const std::string& str, Paint& paint, sFONT* font );

private:
    Position center_;
    FACE upface_;

    struct Rect{
        Position topLeft;
        Position bottomRight;
    } rect_;
};
