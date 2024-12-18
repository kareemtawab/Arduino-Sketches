// update 3 :mirror image
// reverseScreen=true/false, turn=front camera, false=Selfie
/*
  Read the temperature pixels from the MLX90640 IR array
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/14769

  This example initializes the MLX90640 and outputs the 768 temperature values
  from the 768 pixels.

  This example will work with a Teensy 3.1 and above. The MLX90640 requires some
  hefty calculations and larger arrays. You will need a microcontroller with 20,000
  bytes or more of RAM.

  This relies on the driver written by Melexis and can be found at:
  https://github.com/melexis/mlx90640-library

  Hardware Connections:
  Connect the SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  to the Qwiic board
  Connect the male pins to the Teensy. The pinouts can be found here: https://www.pjrc.com/teensy/pinout.html
  Open the serial monitor at 9600 baud to see the output
*/
#include <ESP32-Chimera-Core.h>
#include <Wire.h>
#include <M5StackUpdater.h>

#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

#include <WiFi.h>
#include <time.h>

const byte MLX90640_address = 0x33; //Default 7-bit unshifted address of the MLX90640
#define TA_SHIFT 8 //Default shift for MLX90640 in open air

#define COLS 32
#define ROWS 24
#define COLS_2 (COLS * 2)
#define ROWS_2 (ROWS * 2)

// -------------------------------------------------
// Initial Settings
// -------------------------------------------------

// *** Set your WiFi Settings. ***
// If you are launching from M5Stack LovyanLauncher, WiFi settings are not required.
// https://github.com/lovyan03/M5Stack_LovyanLauncher
const char* ssid      = "";
const char* pass      = "";
const char* ntpServer = "ntp.jst.mfeed.ad.jp";

bool reverseScreen = true;
byte speed_setting = 2 ; // High is 1 , Low is 2
// -------------------------------------------------
// End of Initial Settings
// -------------------------------------------------
bool autoCapture = false;
uint16_t autoCaptureInterval = 30000; // msec
long autoCaptureTargetTime;

float pixelsArraySize = COLS * ROWS;
float pixels[COLS * ROWS];
float pixels_2[COLS_2 * ROWS_2];
float reversePixels[COLS * ROWS];


// #define INTERPOLATED_COLS 32
// #define INTERPOLATED_ROWS 32
#define INTERPOLATED_COLS 32
#define INTERPOLATED_ROWS 32

static float mlx90640To[COLS * ROWS];
paramsMLX90640 mlx90640;
float signedMag12ToFloat(uint16_t val);

//low range of the sensor (this will be blue on the screen)
int MINTEMP = 20; // For color mapping
int min_v = 24; //Value of current min temp
int min_cam_v = -40; // Spec in datasheet


//high range of the sensor (this will be red on the screen)
int MAXTEMP = 50; // For color mapping
int max_v = 35; //Value of current max temp
int max_cam_v = 300; // Spec in datasheet
int resetMaxTemp = 45;

//the colors we will be using

const uint16_t camColors[] = {0x480F,
                              0x400F, 0x400F, 0x400F, 0x4010, 0x3810, 0x3810, 0x3810, 0x3810, 0x3010, 0x3010,
                              0x3010, 0x2810, 0x2810, 0x2810, 0x2810, 0x2010, 0x2010, 0x2010, 0x1810, 0x1810,
                              0x1811, 0x1811, 0x1011, 0x1011, 0x1011, 0x0811, 0x0811, 0x0811, 0x0011, 0x0011,
                              0x0011, 0x0011, 0x0011, 0x0031, 0x0031, 0x0051, 0x0072, 0x0072, 0x0092, 0x00B2,
                              0x00B2, 0x00D2, 0x00F2, 0x00F2, 0x0112, 0x0132, 0x0152, 0x0152, 0x0172, 0x0192,
                              0x0192, 0x01B2, 0x01D2, 0x01F3, 0x01F3, 0x0213, 0x0233, 0x0253, 0x0253, 0x0273,
                              0x0293, 0x02B3, 0x02D3, 0x02D3, 0x02F3, 0x0313, 0x0333, 0x0333, 0x0353, 0x0373,
                              0x0394, 0x03B4, 0x03D4, 0x03D4, 0x03F4, 0x0414, 0x0434, 0x0454, 0x0474, 0x0474,
                              0x0494, 0x04B4, 0x04D4, 0x04F4, 0x0514, 0x0534, 0x0534, 0x0554, 0x0554, 0x0574,
                              0x0574, 0x0573, 0x0573, 0x0573, 0x0572, 0x0572, 0x0572, 0x0571, 0x0591, 0x0591,
                              0x0590, 0x0590, 0x058F, 0x058F, 0x058F, 0x058E, 0x05AE, 0x05AE, 0x05AD, 0x05AD,
                              0x05AD, 0x05AC, 0x05AC, 0x05AB, 0x05CB, 0x05CB, 0x05CA, 0x05CA, 0x05CA, 0x05C9,
                              0x05C9, 0x05C8, 0x05E8, 0x05E8, 0x05E7, 0x05E7, 0x05E6, 0x05E6, 0x05E6, 0x05E5,
                              0x05E5, 0x0604, 0x0604, 0x0604, 0x0603, 0x0603, 0x0602, 0x0602, 0x0601, 0x0621,
                              0x0621, 0x0620, 0x0620, 0x0620, 0x0620, 0x0E20, 0x0E20, 0x0E40, 0x1640, 0x1640,
                              0x1E40, 0x1E40, 0x2640, 0x2640, 0x2E40, 0x2E60, 0x3660, 0x3660, 0x3E60, 0x3E60,
                              0x3E60, 0x4660, 0x4660, 0x4E60, 0x4E80, 0x5680, 0x5680, 0x5E80, 0x5E80, 0x6680,
                              0x6680, 0x6E80, 0x6EA0, 0x76A0, 0x76A0, 0x7EA0, 0x7EA0, 0x86A0, 0x86A0, 0x8EA0,
                              0x8EC0, 0x96C0, 0x96C0, 0x9EC0, 0x9EC0, 0xA6C0, 0xAEC0, 0xAEC0, 0xB6E0, 0xB6E0,
                              0xBEE0, 0xBEE0, 0xC6E0, 0xC6E0, 0xCEE0, 0xCEE0, 0xD6E0, 0xD700, 0xDF00, 0xDEE0,
                              0xDEC0, 0xDEA0, 0xDE80, 0xDE80, 0xE660, 0xE640, 0xE620, 0xE600, 0xE5E0, 0xE5C0,
                              0xE5A0, 0xE580, 0xE560, 0xE540, 0xE520, 0xE500, 0xE4E0, 0xE4C0, 0xE4A0, 0xE480,
                              0xE460, 0xEC40, 0xEC20, 0xEC00, 0xEBE0, 0xEBC0, 0xEBA0, 0xEB80, 0xEB60, 0xEB40,
                              0xEB20, 0xEB00, 0xEAE0, 0xEAC0, 0xEAA0, 0xEA80, 0xEA60, 0xEA40, 0xF220, 0xF200,
                              0xF1E0, 0xF1C0, 0xF1A0, 0xF180, 0xF160, 0xF140, 0xF100, 0xF0E0, 0xF0C0, 0xF0A0,
                              0xF080, 0xF060, 0xF040, 0xF020, 0xF800,
                             };



float get_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void set_point(float *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f);
void get_adjacents_1d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void get_adjacents_2d(float *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(float *src, uint8_t src_rows, uint8_t src_cols, float *dest, uint8_t dest_rows, uint8_t dest_cols);

long loopTime, startTime, endTime, fps;

void setup()
{
  M5.begin();
  M5.Power.begin();
  Wire.begin();
  if(digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.ScreenShot.begin();
  Wire.setClock(450000); //Increase I2C clock speed to 400kHz
  Serial.begin(115200);
  M5.Lcd.begin();
  M5.Lcd.setRotation(1);

  M5.Lcd.fillScreen(TFT_BLACK);

  // for ScreenShot
  adjustClockfromNTP();
  
  M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);

  while (!Serial); //Wait for user to open terminal
  Serial.println("M5Stack MLX90640 IR Camera");
  M5.Lcd.setTextSize(2);

  //Get device parameters - We only have to do this once
  int status;
  uint16_t eeMLX90640[832];//32 * 24 = 768
  status = MLX90640_DumpEE(MLX90640_address, eeMLX90640);
  if (status != 0)
    Serial.println("Failed to load system parameters");

  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
  if (status != 0)
    Serial.println("Parameter extraction failed");

  int SetRefreshRate;
  //Setting MLX90640 device at slave address 0x33 to work with 16Hz refresh rate:
  // 0x00 – 0.5Hz
  // 0x01 – 1Hz
  // 0x02 – 2Hz
  // 0x03 – 4Hz
  // 0x04 – 8Hz // OK 
  // 0x05 – 16Hz // OK
  // 0x06 – 32Hz // Fail
  // 0x07 – 64Hz
  SetRefreshRate = MLX90640_SetRefreshRate (0x33, 0x05);
  //Once params are extracted, we can release eeMLX90640 array

  //Display bottom side colorList and info
  M5.Lcd.fillScreen(TFT_BLACK);
  int icolor = 0;
  for (int icol = 0; icol <= 248;  icol++)
  {
    //彩色条
    M5.Lcd.drawRect(36, 208, icol, 284 , camColors[icolor]);
    icolor++;
  }
  infodisplay();
}


void loop()
{
  M5.update();
  loopTime = millis();
  startTime = loopTime;
  if (autoCapture) {
    if ((millis() - autoCaptureTargetTime) > autoCaptureInterval) {
      M5.ScreenShot.snap("Thermal_Auto");
      autoCaptureTargetTime = millis();
    }
  }

  
  ///////////////////////////////
  // Set Min Value - LongPress //
  ///////////////////////////////
  if (M5.BtnA.pressedFor(1000)) {
    if (MINTEMP <= 5 )
    {
      MINTEMP = MAXTEMP - 5;
    }
    else
    {
      MINTEMP = MINTEMP - 5;
    }
    infodisplay();
  }

  ///////////////////////////////
  // Set Min Value - SortPress //
  ///////////////////////////////
//  if (M5.BtnA.wasPressed()) {
//    if (MINTEMP <= 0)
//    {
//      MINTEMP = MAXTEMP - 1;
//    }
//    else
//    {
//      MINTEMP--;
//    }
//    infodisplay();
//  }

  ///////////////////////////
  // AutoCapture switch    //
  ///////////////////////////
//  if (M5.BtnA.isPressed() && M5.BtnB.isPressed()) {
  if (M5.BtnA.wasPressed()) {
    if (autoCapture) {
      autoCapture = false;
      infodisplay();
    } else {
      autoCapture = true;
      autoCaptureTargetTime = millis();
      infodisplay();
      M5.ScreenShot.snap("Thermal_Auto");
    }
  }

  ///////////////////////////////
  // Set Default Value - LongPress //
  ///////////////////////////////
  if (M5.BtnB.pressedFor(3000)) {
    MINTEMP = 20;
    MAXTEMP = 50;
    infodisplay();
  }
  ////////////////////////
  // ScreenShot(Manual) //
  ////////////////////////
  if (M5.BtnB.wasPressed() && !M5.BtnA.isPressed()) {
    M5.ScreenShot.snap("Thermal_Manual");
  }


  /////////////////////////////
  // Reset MINTEMP & MAXTEMP //
  /////////////////////////////
  if (M5.BtnA.isPressed() && M5.BtnC.isPressed()) {
    MINTEMP = min_v - 1;
    MAXTEMP = max_v + 1;
    infodisplay();
  }

  ///////////////////////////////
  // Set Max Value - LongPress //
  ///////////////////////////////
  if (M5.BtnC.pressedFor(1000)) {
    if (MAXTEMP >= max_cam_v)
    {
      MAXTEMP = MINTEMP + 1;
    }
    else
    {
      MAXTEMP = MAXTEMP + 5;
    }
    infodisplay();
  }

  ///////////////////////////////
  // Set Max Value - SortPress //
  ///////////////////////////////
//  if (M5.BtnC.wasPressed()) {
//    if (MAXTEMP >= max_cam_v )
//    {
//      MAXTEMP = MINTEMP + 1;
//    }
//    else
//    {
//      MAXTEMP++;
//    }
//    infodisplay();
//  }


  for (byte x = 0 ; x < speed_setting ; x++) // x < 2 Read both subpages
  {
    uint16_t mlx90640Frame[834];
    int status = MLX90640_GetFrameData(MLX90640_address, mlx90640Frame);
    if (status < 0)
    {
      Serial.print("GetFrame Error: ");
      Serial.println(status);
    }

    float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
    float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);
    float tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
    float emissivity = 0.95;
    MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, pixels); //save pixels temp to array (pixels)
	int mode_ = MLX90640_GetCurMode(MLX90640_address);
    //amendment
    MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, pixels, mode_, &mlx90640);
    //MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, pixels, mode_, &mlx90640);
  }

  //Reverse image (order of Integer array)
  if (reverseScreen == 1)
  {
    for (int x = 0 ; x < pixelsArraySize ; x++)
    {
      if (x % COLS == 0) //32 values wide
      {
        for (int j = 0 + x, k = (COLS-1) + x; j < COLS + x ; j++, k--)
        {
          reversePixels[j] = pixels[k];
  //         Serial.print(x);Serial.print(" = Rev "); Serial.print(j);Serial.print(" ,  Nor ");Serial.println(k);
        }
      }
    }
  }

  float dest_2d[INTERPOLATED_ROWS * INTERPOLATED_COLS];
  int ROWS_i,COLS_j;

  if (reverseScreen == 1)
  {
    // ** reversePixels
    interpolate_image(reversePixels, ROWS, COLS, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
  }
  else
  {

    interpolate_image(pixels, ROWS, COLS, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);
    // 32 * 24 = 768
    // 63 * 48 = 3072
    //pixels_2
    for(int y = 0;y < ROWS;y++)
    {
      for(int x = 0;x < COLS;x++)
      {
        // 原始数据
        pixels_2[(((y * 2) * (COLS*2)) + (x * 2))] = pixels[y*COLS+x];
        
        if(x != 31)
          pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+1)] = ( pixels_2[(((y * 2) * (COLS*2)) + (x * 2))] + pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+2)]) / 2;
        else
          pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+1)] = ( pixels_2[(((y * 2) * (COLS*2)) + (x * 2))] );
          
        //Serial.print(pixels_2[(((y * 2) * (COLS*2)) + (x * 2))]);
        //Serial.print(pixels[y*COLS+x]);
        //Serial.print(" ");
      }
      //Serial.println("\r\n");
    }
    /*
   //-------------------------
    // 计算x间隔插入数据
    for(int y = 0;y < ROWS;y++)//24
    {
      for(int x = 0;x < COLS;x++)//32
      {
        if(x != 31)
          pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+1)] = ( pixels_2[(((y * 2) * (COLS*2)) + (x * 2))] + pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+2)]) / 2;
        else
          pixels_2[(((y * 2) * (COLS*2)) + (x * 2)+1)] = ( pixels_2[(((y * 2) * (COLS*2)) + (x * 2))] );
      }
    }
    */
    ///*
    // 计算y间隔插入数据 
    for(int y = 0;y < ROWS;y++)//24
    {
      for(int x = 0;x < COLS_2;x++)//64
      {
        if(y != 23)
          pixels_2[(((y * 2) + 1) * (COLS_2)) + x] = ( pixels_2[(((y * 2) * COLS_2) + x)] + pixels_2[((((y * 2) + 2) * COLS_2) + x)] ) / 2;
        else
          pixels_2[(((y * 2) + 1) * (COLS_2)) + x] = ( pixels_2[(((y * 2) * COLS_2) + x)] + pixels_2[(((y * 2) * COLS_2) + x)] ) / 2;
      }
    }   
    //*/
/*
       //打印数据
    for(int y = 0;y < ROWS_2;y++)
    {
      for(int x = 0;x < COLS_2;x++)
      {
        Serial.print(pixels_2[y * COLS_2 + x]);
        Serial.print(" ");
      }
      Serial.println("\r\n");
    }
    //-------------------------
   // */ 
  }

  uint16_t boxsize = min(M5.Lcd.width() / INTERPOLATED_ROWS, M5.Lcd.height() / INTERPOLATED_COLS);
  uint16_t boxWidth = M5.Lcd.width() / INTERPOLATED_ROWS;
  uint16_t boxHeight = (M5.Lcd.height() - 31) / INTERPOLATED_COLS; // 31 for bottom info
  drawpixels(dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS, boxWidth, boxHeight, false);
  max_v = MINTEMP;
  min_v = MAXTEMP;
  int spot_v = pixels[463]; // 32pixel * (15rows - 1) + (16cols - 1)
  
   for ( int itemp = 0; itemp < sizeof(pixels) / sizeof(pixels[0]); itemp++ )
  {
    if ( pixels[itemp] > max_v )
    {
      max_v = pixels[itemp];
    }
    if ( pixels[itemp] < min_v )
    {
      min_v = pixels[itemp];
    }
  }


  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRect(164, 220, 75, 18, TFT_BLACK); // clear max temp text
  M5.Lcd.fillRect(60, 220, 200, 18, TFT_BLACK); // clear spot temp text
  int icolor = 0;
  //for (int icol = 0; icol <= 248;  icol++)
  //{
   // M5.Lcd.drawRect(36, 208, icol, 284 , camColors[icolor]);
   // icolor++;
  //}

  M5.Lcd.setCursor(60, 222);      // update min & max temp
  M5.Lcd.setTextColor(TFT_WHITE);

  if (max_v > max_cam_v | max_v < min_cam_v ) {
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.printf("Error", 1);
  }
  else
  {
    M5.Lcd.print("Min:");
    M5.Lcd.print(min_v, 1);
    M5.Lcd.print("C  ");
    M5.Lcd.print("Max:");
    M5.Lcd.print(max_v, 1);
    M5.Lcd.print("C");
    M5.Lcd.setCursor(180, 94); // update spot temp text
    M5.Lcd.print(spot_v, 1);
    M5.Lcd.printf("C");
    M5.Lcd.drawCircle(160, 120, 6, TFT_WHITE);     // update center spot icon
    M5.Lcd.drawLine(160, 110, 160, 130, TFT_WHITE); // vertical line
    M5.Lcd.drawLine(150, 120, 170, 120, TFT_WHITE); // horizontal line
  }
  loopTime = millis();
  endTime = loopTime;
  fps = 1000 / (endTime - startTime);
  //M5.Lcd.fillRect(310, 209, 10, 12, TFT_BLACK); //Clear fps text area
// M5.Lcd.fillRect(284, 209, 20, 12, TFT_BLACK);   //Clear fps text area
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setCursor(284, 204);
  if (autoCapture) {
    M5.Lcd.print("CAP:A");
  } else {
    M5.Lcd.print("CAP:M");
  }
  M5.Lcd.setCursor(284, 212);
  M5.Lcd.print("fps:" + String( fps ));
  M5.Lcd.setTextSize(1);

}


/***infodisplay()*****/
void infodisplay(void) {
  int icolor = 0;
  for (int icol = 0; icol <= 248;  icol++)
  {
    //彩色条
    M5.Lcd.drawRect(36, 208, icol, 284 , camColors[icolor]);
    icolor++;
  }
  M5.Lcd.fillRect(0, 194, 320, 2, TFT_WHITE);
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(284, 222); //move to bottom right
  M5.Lcd.print(MAXTEMP , 1);  // update MAXTEMP
  M5.Lcd.print("C");
  M5.Lcd.setCursor(0, 222);  // update MINTEMP text
  M5.Lcd.print(MINTEMP , 1);
  M5.Lcd.print("C");
  M5.Lcd.setCursor(106, 224);
}

void drawpixels(float *p, uint8_t rows, uint8_t cols, uint8_t boxWidth, uint8_t boxHeight, boolean showVal) {
  int colorTemp;
  for (int y = 0; y < rows; y++) 
  {
    for (int x = 0; x < cols; x++) 
    {
      float val = get_point(p, rows, cols, x, y);
      
      if (val >= MAXTEMP) 
        colorTemp = MAXTEMP;
      else if (val <= MINTEMP) 
        colorTemp = MINTEMP;
      else colorTemp = val;

      uint8_t colorIndex = map(colorTemp, MINTEMP, MAXTEMP, 0, 255);
      colorIndex = constrain(colorIndex, 0, 255);// 0 ~ 255
      //draw the pixels!
      uint16_t color;
      color = val * 2;
      M5.Lcd.fillRect(boxWidth * x, boxHeight * y, boxWidth, boxHeight, camColors[colorIndex]);
    }
  }

  int grX = 2;
  int grY = 2;
  
  for (int y = 0; y < rows; y++) 
  {
    for (int x = 0; x < cols; x++) 
    {
      float val = 0;
      if (((x%grX) == 0) && ((y%grY)==0))   
      {
        val = get_point(p, rows, cols, x, y);
        M5.Lcd.setTextSize(0);
        M5.Lcd.setTextColor(TFT_WHITE);            
        M5.Lcd.setCursor((boxWidth *x)+((boxWidth*grX)/3) + 2, (boxHeight *y)+((boxHeight*grY)/3) + 2); // update spot temp text
        M5.Lcd.print(val, 0);
        M5.Lcd.fillCircle((boxWidth *x)+((boxWidth*grX)/3), (boxHeight *y)+((boxHeight*grY)/3), 1, TFT_BLACK);
      }
    }
  } 

}

//Returns true if the MLX90640 is detected on the I2C bus
boolean isConnected()
{
  Wire.beginTransmission((uint8_t)MLX90640_address);
  if (Wire.endTransmission() != 0)
    return (false); //Sensor did not ACK
  return (true);
}

void adjustClockfromNTP() {
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextSize(2);
  if (!strcmp(ssid,"")) {
    M5.Lcd.println("Connecting to the previous WiFiAP.");
    M5.Lcd.println("If you don't connect to WiFi. Check MLX90640.ino");
    WiFi.begin();
  } else {
    M5.Lcd.printf("Connentng to SSID:%s\n", ssid);
    WiFi.begin(ssid, pass);
  }
  M5.Lcd.println("Button A: Skip Time Setting");
  while (WiFi.status() != WL_CONNECTED) {
    M5.update();
    if (M5.BtnA.wasPressed()) {
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      return;
    }
    M5.Lcd.print(".");
    delay(500);
  }
  M5.Lcd.println("\nWiFi connected");
  
  configTime(9 * 3600, 0, ntpServer);
  struct tm timeinfo;

  M5.Lcd.println("Get Time from NTP");
  while (!getLocalTime(&timeinfo)) {
    M5.Lcd.print(".");
    delay(1000);
  }

  M5.Lcd.setTextSize(3);
  M5.Lcd.println("Time adjusted");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(2000);

}
