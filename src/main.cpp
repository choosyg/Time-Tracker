#include <Arduino.h>

#include <Arduino_LSM9DS1.h> //IMU unit of nano 33 BLE

//Headers for e-paper display
#include <SPI.h>
#include "epd1in54.h"
#include "epdpaint.h"

#define BLACK   0
#define WHITE   1

unsigned char image[1024];
Paint paint(image, 0, 0);    // width should be the multiple of 8 
Epd epd;

unsigned long time_last;
unsigned long time_now;

unsigned long duration_top = 0;
unsigned long duration_left = 0;
unsigned long duration_right = 0;
unsigned long duration_bottom = 0;

struct Position{
  int x;
  int y;
  Position( int x_, int y_ ): x(x_), y(y_){}
};

Position position2Screen( const Position& pos ){
  Position res = pos;
  switch( paint.GetRotate()){
    case ROTATE_90:  res.x = 200-pos.y-paint.GetWidth();
                     res.y = pos.x;
                     break;
    case ROTATE_180:  res.x = 200-pos.x-paint.GetWidth();
                      res.y = 200-pos.y-paint.GetHeight();
                      break;
    case ROTATE_270:  res.x = pos.y;
                      res.y = 200-pos.x-paint.GetHeight();
                      break;
  }
  return res;
}

void drawTime( unsigned long time_ms, const Position& pos, sFONT* font ){
  if( paint.GetRotate() == ROTATE_90 || paint.GetRotate() == ROTATE_270 ){
    paint.SetWidth(font->Height);
    paint.SetHeight(font->Width*8);
  } else {
    paint.SetWidth(font->Width*8);
    paint.SetHeight(font->Height);
  }
  
  paint.Clear(WHITE);
  
  char time_string[] = {'0', '0', ':','0', '0', ':', '0', '0', '\0'};
  time_string[0] = time_ms / 1000 / 60 / 60 / 10 + '0';
  time_string[1] = time_ms / 1000 / 60 / 60 % 10 + '0';
  time_string[3] = time_ms / 1000 / 60 / 10 + '0';
  time_string[4] = time_ms / 1000 / 60 % 10 + '0';
  time_string[6] = time_ms / 1000 % 60 / 10 + '0';
  time_string[7] = time_ms / 1000 % 60 % 10 + '0';
  paint.DrawStringAt(0, 0, time_string, font, BLACK);

  Position screen = position2Screen( pos );
  
  epd.SetFrameMemory(paint.GetImage(), screen.x, screen.y, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  epd.SetFrameMemory(paint.GetImage(), screen.x, screen.y, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

void drawScreen() {
  paint.SetWidth(200);
  paint.SetHeight(200);

  paint.Clear(WHITE);
  paint.DrawRectangle(35, 35, 165, 165, BLACK);
  paint.DrawLine(0, 0, 35, 35, BLACK);
  paint.DrawLine(0, 200, 35, 200-35, BLACK);
  paint.DrawLine(200, 0, 200-35, 35, BLACK);
  paint.DrawLine(200, 200, 200-35, 200-35, BLACK);
  
  //set it to screen and to frame buffer
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  epd.SetFrameMemory(paint.GetImage(), 0, 0, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

void setup() {
  Serial.begin(115200);    // initialize serial communication

  Serial.print("IMU init");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  Serial.print("e-Paper init");
  if (epd.Init(lut_full_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  paint.SetRotate(ROTATE_0);
  drawScreen();
  
  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  time_last = millis();
  time_now = millis();
  //epd.Sleep();
}

void loop() {
  
  if (IMU.accelerationAvailable())
  {
    float x, y, z, delta = 0.1;
    IMU.readAcceleration(x, y, z);
    /*Serial.println("---------");
    Serial.println(x);
    Serial.println(y);
    Serial.println(z);
    Serial.println("---------");*/
    auto rotate = paint.GetRotate();
    if(z>=1-delta && rotate != ROTATE_0 ){
      Serial.println("Up-Face = TOP");
      rotate = ROTATE_0;
    } else if( z<= -1+delta && rotate != ROTATE_180 ){
      Serial.println("Up-Face = BOTTOM");
      rotate = ROTATE_180;
    }else if( y >= 1 - delta && rotate != ROTATE_270 ){
      Serial.println("Up-Face = LEFT");
      rotate = ROTATE_270;
    } else if( y <= -1 + delta && rotate != ROTATE_90 ){
       Serial.println("Up-Face = RIGHT");
       rotate = ROTATE_90;
    }

    if( rotate != paint.GetRotate() ){
      drawScreen();
      
      //calc x such that time is centered:
      Position pos( (200-Font24.Width*8)/2, (35-Font24.Height)/2);
      paint.SetRotate(ROTATE_0);
      drawTime( duration_top, pos, &Font24 );
      paint.SetRotate(ROTATE_90);
      drawTime( duration_left, pos, &Font24 );
      paint.SetRotate(ROTATE_180);
      drawTime( duration_bottom, pos, &Font24 );
      paint.SetRotate(ROTATE_270);
      drawTime( duration_right, pos, &Font24 );
      
      paint.SetRotate(rotate);
    }
  }

  time_now = millis();
  Position pos( (200-Font24.Width*8)/2, (35-Font24.Height)/2 );
  switch( paint.GetRotate() ){
    case ROTATE_0: duration_top += time_now-time_last;
                   drawTime( duration_top, pos, &Font24 );
                   break;
    case ROTATE_90: duration_left += time_now-time_last;
                   drawTime( duration_left, pos, &Font24 );
                   break;
    case ROTATE_180: duration_bottom += time_now-time_last;
                   drawTime( duration_bottom, pos, &Font24 );
                   break;
    case ROTATE_270: duration_right += time_now-time_last;
                   drawTime( duration_right, pos, &Font24 );
                   break;
  }
  time_last = time_now;

  Position center( (200-Font20.Width*8)/2, (200-Font20.Height)/2 );
  drawTime( duration_top+duration_left+duration_bottom+duration_right, center, &Font20 );

  /*paint.SetHeight(Font24.Height);
  paint.SetWidth(Font24.Width*2);
  paint.Clear(WHITE);
  paint.DrawStringAt(0, 0, "TL", &Font24, BLACK);
  epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();

  drawTime( duration_top, 35, 0 );*/
  //TODO draw total duration
  //TODO draw temperature & humidity
  
  //epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
  //epd.DisplayFrame();

  delay(500);
}
