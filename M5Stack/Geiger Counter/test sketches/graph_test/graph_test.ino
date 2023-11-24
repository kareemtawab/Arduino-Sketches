#include <M5Unified.h>
#include <M5GFX.h>

#define LTBLUE 0xB6DF
#define LTTEAL 0xBF5F
#define LTGREEN 0xBFF7
#define LTCYAN 0xC7FF
#define LTRED 0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW 0xFFF8
#define LTORANGE 0xFE73
#define LTPINK 0xFDDF
#define LTPURPLE 0xCCFF
#define LTGREY 0xE71C

#define BLUE 0x001F
#define TEAL 0x0438
#define GREEN 0x07E0
#define CYAN 0x07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define YELLOW 0xF607
#define ORANGE 0xFC00
#define PINK 0xF81F
#define PURPLE 0x8010
#define GREY 0xC618
#define WHITE 0xFFFF
#define BLACK 0x0000

#define DKBLUE 0x000D
#define DKTEAL 0x020C
#define DKGREEN 0x03E0
#define DKCYAN 0x03EF
#define DKRED 0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW 0x8400
#define DKORANGE 0x8200
#define DKPINK 0x9009
#define DKPURPLE 0x4010
#define DKGREY 0x4A49

M5GFX disp;

bool drawGraph = true;
int ox, x;
float oy, y;
float graphYPlotPoints[60];

void drawSample(double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, String title, String xlabel, String ylabel, unsigned int gcolor, unsigned int acolor, unsigned int pcolor, unsigned int tcolor, unsigned int bcolor, bool &redraw) {

  if (redraw == true) {
    ox = (x - xlo) * (w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    //now draw the labels
    disp.setTextSize(2);
    disp.setTextColor(tcolor, bcolor);
    disp.setCursor(disp.width() / 2 - title.length() / 2 * 12, gy - h - 30);
    disp.println(title);
    disp.setTextSize(1);
    disp.setTextColor(acolor, bcolor);
    disp.setCursor(disp.width() / 2 - xlabel.length() / 2 * 6, gy + 25);
    disp.println(xlabel);
    disp.setTextSize(1);
    disp.setTextColor(acolor, bcolor);
    disp.setCursor(gx - 30, gy - h - 10);
    disp.println(ylabel);
  }
  // draw y scale
  for (float i = ylo; i <= yhi; i += yinc) {
    float temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    if (i == 0) {
      disp.drawLine(gx, temp, gx + w, temp, acolor);
    } else {
      disp.drawLine(gx, temp, gx + w, temp, gcolor);
    }
    if (redraw) {
      disp.setTextSize(1);
      disp.setTextColor(tcolor, bcolor);
      disp.setCursor(gx - 40, temp);
      disp.println(i);
    }
  }
  // draw x scale
  for (int i = xlo; i <= xhi; i += xinc) {
    int temp = (i - xlo) * (w) / (xhi - xlo) + gx;
    if (i == 0) {
      disp.drawLine(temp, gy, temp, gy - h, acolor);
    } else {
      disp.drawLine(temp, gy, temp, gy - h, gcolor);
    }
    if (redraw) {
      disp.setTextSize(1);
      disp.setTextColor(tcolor, bcolor);
      disp.setCursor(temp, gy + 10);
      disp.println(i);
    }
  }
  //now plot the data
  y = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  if (x == -59) {
    ox = -59;
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
  } else {
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    disp.drawLine(ox, oy, x, y, pcolor);
    disp.drawLine(ox, oy + 1, x, y + 1, pcolor);
    //disp.drawLine(ox, oy - 1, x, y - 1, pcolor);
  }
  ox = x;
  oy = y;
  redraw = false;
}

void setup() {
  disp.init();
  disp.fillScreen(BLACK);
}


void loop(void) {
  for (int i = 0; i <= 59; i++) {
    graphYPlotPoints[i] = float(random(10, 30) / 100.0f);
  }
  delay(350);
  drawGraphAll();
}

void drawGraphAll() {
  disp.fillRect(35, 70, 276, 110, BLACK);
  for (x = -59; x <= 0; x++) {
    y = graphYPlotPoints[59 + int(x)];
    drawSample(
      x,                         // x data point
      y,                         // y data point
      35,                        // x graph location (lower left)
      180,                       // y graph location (lower left)
      275,                       // width of graph
      110,                       // height of graph
      -59,                       // lower bound of x axis
      000,                       // upper bound of x axis
      15,                        // division of x axis (distance not count)
      00.0,                      // lower bound of y axis
      05.0,                      // upper bound of y asis
      01.0,                      // division of y axis (distance not count)
      "Radiation Dose History",  // title = title of graph
      "Time (Seconds)",          // x asis label
      "Dose (uSv/h)",            // y asis label
      YELLOW,                    // graph line colors
      YELLOW,                    // axis line colors
      YELLOW,                    // color of your plotted data
      WHITE,                     // text color
      BLACK,                     // background color
      drawGraph);                // flag to redraw graph on fist call only
  }
}
