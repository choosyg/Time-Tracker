#pragma once

#include "Field.h"

#include "epd/epd1in54.h"
#include "epd/epdpaint.h"

class Display {
public:
    Display();

    void setup();

    void setFaceDuration( unsigned long ms, FACE upface );
    void setTotalDuration( unsigned long ms );

    FACE upface() const;
    void setUpface( FACE upface );

private:
    void updateDisplay();

    Field faceField_[4];
    Field total_;

    unsigned char image_[500];
    Paint paint_;
    Epd epd_;
};
