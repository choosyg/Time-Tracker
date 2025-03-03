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
unsigned long time_reset;
unsigned long time_flip;
unsigned long time_now;

unsigned long duration_top = 0;
unsigned long duration_left = 0;
unsigned long duration_right = 0;
unsigned long duration_bottom = 0;

char[6] formatTime( unsigned long time_ms ){
  char time_string[6] = {'0', '0', ':', '0', '0', '\0'};
  time_string[0] = time_ms / 1000 / 60 / 10 + '0';
  time_string[1] = time_ms / 1000 / 60 % 10 + '0';
  time_string[3] = time_ms / 1000 % 60 / 10 + '0';
  time_string[4] = time_ms / 1000 % 60 % 10 + '0';
  return time_string;
}

void drawTime( unsigned long time_ms, int x, int y ){
  paint.SetWidth(100);
  paint.SetHeight(20);
  
  paint.Clear(WHITE);
  
  auto str = formatTime( time_ms );
  paint.DrawStringAt(0, 0, str, &Font24, BLACK);
  
  epd.SetFrameMemory(paint.GetImage(), x, y, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

void drawScreen() {
  paint.SetWidth(200);
  paint.SetHeight(200);

  paint.Clear(WHITE);
  paint.DrawRectangle(35, 35, 130, 130, BLACK);
  paint.DrawLine(0, 0, 35, 35, BLACK);
  paint.DrawLine(0, 200, 35, 200-35, BLACK);
  paint.DrawLine(200, 0, 200-35, 35, BLACK);
  paint.DrawLine(200, 200, 200-35, 200-35, BLACK);

  //TODO fill top part BLACK
  //TODO draw text in center (Date and reset_time)
  
  epd.SetFrameMemory(paint.GetImage(), 16, 60, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
  epd.SetFrameMemory(paint.GetImage(), 16, 60, paint.GetWidth(), paint.GetHeight());
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

  /** 
   *  there are 2 memory areas embedded in the e-paper display
   *  and once the display is refreshed, the memory area will be auto-toggled,
   *  i.e. the next action of SetFrameMemory will set the other memory area
   *  therefore you have to clear the frame memory twice.
   */
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  epd.ClearFrameMemory(0xFF);   // bit set = white, bit reset = black
  epd.DisplayFrame();
  
  paint.SetRotate(ROTATE_0);
  drawScreen();
  
  if (epd.Init(lut_partial_update) != 0) {
      Serial.print("e-Paper init failed");
      return;
  }

  time_reset = millis();
  time_flip = millis();
  time_now = millis();
  //epd.Sleep();
}

void loop() {
  // put your main code here, to run repeatedly:
  float x, y, z, delta = 0.05;

  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
 
    if(y <= delta && y >= -delta && rotation != ROTATE_0 ){
      Serial.println("Up-Face = TOP");
      time_flip = millis();
        
      paint.SetRotate(ROTATE_0);
      drawScreen();
        
      paint.SetRotate(ROTATE_90);
      drawTime( duration_left, 4, 54 );
      paint.SetRotate(ROTATE_180);
      drawTime( duration_right, 200-4, 54 );
      paint.SetRotate(ROTATE_270);
      drawTime( duration_bottom, 54, 200-4 );
        
      paint.SetRotate(ROTATE_0);
      drawTime( duration_top, 54, 4 );
      
    } else if(y >= 1 - delta){
      Serial.println("Up-Face = LEFT");
    } else if(y >= 1 - delta){
       Serial.println("Up-Face = RIGHT");
    }
  }

  drawTime( duration_top, 54, 4 );
  //TODO draw total duration
  //TODO draw temperature & humidity
  
  epd.SetFrameMemory(paint.GetImage(), 80, 72, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();

  delay(500);
}
