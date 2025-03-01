#include <Arduino.h>

#include <Arduino_LSM9DS1.h> //IMU unit of nano 33 BLE

#include "Display.h"

unsigned long time_last;
unsigned long faceDuration[4] = { 0, 0, 0, 0 };
Display display;

void setup() {
    Serial.begin( 115200 );

    Serial.println( "Setup: Started" );
    Serial.println( "IMU init" );
    if( !IMU.begin() ) {
        Serial.println( "Failed to initialize IMU!" );
        while( 1 )
            ;
    }

    display.setup();
    time_last = millis();
    Serial.println( "Setup: Success" );
}

void loop() {
    if( IMU.accelerationAvailable() ) {
        float x, y, z, delta = 0.1;
        IMU.readAcceleration( x, y, z );

        if( z >= 1 - delta ) {
            Serial.println( "Up-Face = TOP" );
            display.setUpface( FACE::TOP );
        } else if( z <= -1 + delta ) {
            Serial.println( "Up-Face = BOTTOM" );
            display.setUpface( FACE::BOTTOM );
        } else if( y >= 1 - delta ) {
            Serial.println( "Up-Face = LEFT" );
            display.setUpface( FACE::LEFT );
        } else if( y <= -1 + delta ) {
            Serial.println( "Up-Face = RIGHT" );
            display.setUpface( FACE::RIGHT );
        } else if( x <= -1 + delta ) {
            Serial.println( "Reset Down-Face = Display" );
            for( size_t i = 0; i < 4; ++i ) {
                faceDuration[i] = 0;
                display.setFaceDuration( 0, static_cast< FACE >( i ) );
            }
            display.setTotalDuration( 0 );

            display.setUpface( FACE::BACK );
        }
    }

    if( display.upface() == FACE::BACK ) {
        time_last = millis();
        return;
    }

    unsigned long time_now = millis();
    faceDuration[static_cast< int >( display.upface() )] += time_now - time_last;
    display.setFaceDuration( faceDuration[static_cast< int >( display.upface() )], display.upface() );
    time_last = time_now;

    display.setTotalDuration( faceDuration[0] + faceDuration[1] + faceDuration[2] + faceDuration[3] );

    // TODO draw temperature & humidity & Date

    delay( 100 );
}
