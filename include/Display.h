#pragma once

#include "Field.h"

#include "epd/epd1in54.h"

class Display {
public:
    Display();

    void setup();

    void setFaceDuration( unsigned long ms, FACE upface );
    void setTotalDuration( unsigned long ms );

    FACE upface() const;
    void setUpface( FACE upface );

private:
    Field faceField_[4];
    Field total_;
    Epd epd_;
};
