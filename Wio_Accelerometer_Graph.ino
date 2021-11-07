#include"LIS3DHTR.h"
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
TFT_eSPI tft = TFT_eSPI();  // Invoke library
LIS3DHTR<TwoWire> lis;

#define TFT_GREY  0x5AEB
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN   0xBFF7
#define LTCYAN    0xC7FF
#define LTRED     0xFD34
#define LTMAGENTA 0xFD5F
#define LTYELLOW  0xFFF8
#define LTORANGE  0xFE73
#define LTPINK    0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY    0xE71C

#define BLUE      0x001F
#define TEAL      0x0438
#define GREEN     0x07E0
#define CYAN      0x07FF
#define RED       0xF800
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFC00
#define PINK      0xF81F
#define PURPLE    0x8010
#define GREY      0xC618
#define WHITE     0xFFFF
#define BLACK     0x0000

#define DKBLUE    0x000D
#define DKTEAL    0x020C
#define DKGREEN   0x03E0
#define DKCYAN    0x03EF
#define DKRED     0x6000
#define DKMAGENTA 0x8008
#define DKYELLOW  0x8400
#define DKORANGE  0x8200
#define DKPINK    0x9009
#define DKPURPLE  0x4010
#define DKGREY    0x4A49



int n = 0;
boolean display1 = true;
boolean update1 = true;
double ox = -999, oy = -999; // Force them to be off screen

void TraceAndBackground(TFT_eSPI& tft, double x,  double y,  byte dp,
           double gx, double gy,
           double w, double h,
           double xlo, double xhi, double xinc,
           double ylo, double yhi, double yinc,
           char* title, char* xlabel, char* ylabel,
           boolean& update1, unsigned int color) 
{
    double ydiv, xdiv;
    double i;
    double temp;
    int rot, newrot;

    unsigned int gcolor = DKBLUE;     // gcolor = graph grid color
    unsigned int acolor = RED;        // acolor = main axes and label color
    unsigned int pcolor = color;      // pcolor = color of your plotted data
    unsigned int tcolor = WHITE;      // tcolor = text color
    unsigned int bcolor = BLACK;      // bcolor = background color

    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    if (update1) {
        update1 = false;

        ox = (x - xlo) * (w) / (xhi - xlo) + gx;
        oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        if ((ox < gx) || (ox > gx + w)) {
            update1 = true;
            return;
        }
        if ((oy < gy - h) || (oy > gy)) {
            update1 = true;
            return;
        }

        tft.setTextDatum(MR_DATUM);

        // draw y scale
        for (i = ylo; i <= yhi; i += yinc) 
        {
            // compute the transform
            temp = (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

            if (i == 0) 
            {
                tft.setTextColor(acolor, bcolor);
                tft.drawString(xlabel, (int)(gx + w), (int)temp, 2);
            }
            // draw the axis labels
            tft.setTextColor(tcolor, bcolor);
            // precision is default Arduino--this could really use some format control
            tft.drawFloat(i, dp, gx - 4, temp, 1);
////////////////////////////////////////////////////////////////
            if (i == 0) 
            {
                tft.drawLine(gx, temp, gx + w, temp, acolor);
                tft.setTextColor(acolor, bcolor);
                tft.drawString(xlabel, (int)(gx + w), (int)temp, 2);
            } else {
                tft.drawLine(gx, temp, gx + w, temp, gcolor);
            }
///////////////////////////////////////////////////////////////
        }

        // draw x scale
        for (i = xlo; i <= xhi; i += xinc) {

            // compute the transform


/////////////////////////////////////////////////////////////////////////
            temp = (i - xlo) * (w) / (xhi - xlo) + gx;
            if (i == 0) {
                tft.drawLine(temp, gy, temp, gy - h, acolor);
                tft.setTextColor(acolor, bcolor);
                tft.setTextDatum(BC_DATUM);
                tft.drawString(ylabel, (int)temp, (int)(gy - h - 8), 2);
            } else {
                tft.drawLine(temp, gy, temp, gy - h, gcolor);
            }
/////////////////////////////////////////////////////////////////////////
            

            // draw the axis labels
            tft.setTextColor(tcolor, bcolor);
            tft.setTextDatum(TC_DATUM);
            // precision is default Arduino--this could really use some format control
            tft.drawFloat(i, dp, temp, gy + 7, 1);
        }

        //now draw the graph labels
        tft.setTextColor(tcolor, bcolor);
        tft.drawString(title, (int)(gx + w / 2), (int)(gy - h - 30), 4);
    }

    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy are initialized above
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((x < gx) || (x > gx + w)) {
        update1 = true;
        return;
    }
    if ((y < gy - h) || (y > gy)) {
        update1 = true;
        return;
    }

    tft.drawLine(ox, oy, x, y, pcolor);
    // it's up to you but drawing 2 more lines to give the graph some thickness
    //tft.drawLine(ox, oy + 1, x, y + 1, pcolor);
    //tft.drawLine(ox, oy - 1, x, y - 1, pcolor);
    ox = x;
    oy = y;
}

void TraceOnly(TFT_eSPI& tft, double x,  double y,  byte dp,
           double gx, double gy,
           double w, double h,
           double xlo, double xhi, double xinc,
           double ylo, double yhi, double yinc,
           char* title, char* xlabel, char* ylabel,
           boolean& update1, unsigned int color) 
{
    unsigned int pcolor = color;      // pcolor = color of your plotted data

    // initialize old x and old y in order to draw the first point of the graph
    // but save the transformed value
    // note my transform funcition is the same as the map function, except the map uses long and we need doubles
    if (update1) 
    {
        update1 = false;

        ox = (x - xlo) * (w) / (xhi - xlo) + gx;
        oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        if ((ox < gx) || (ox > gx + w)) 
        {
            update1 = true;
            return;
        }
        if ((oy < gy - h) || (oy > gy)) 
        {
            update1 = true;
            return;
        }
    }
    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy are initialized above
    x = (x - xlo) * (w) / (xhi - xlo) + gx;
    y = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((x < gx) || (x > gx + w)) 
    {
        update1 = true;
        return;
    }
    if ((y < gy - h) || (y > gy)) 
    {
        update1 = true;
        return;
    }

    tft.drawLine(ox, oy, x, y, pcolor);
    ox = x;
    oy = y;
}

void setup() 
{
  tft.init();
  tft.setRotation(1);
  Serial.begin(115200);
  lis.begin(Wire1);

  if (!lis) 
  {
    Serial.println("ERROR");
    while(1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g

  tft.fillScreen(TFT_GREY);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);  tft.setTextSize(1);
  tft.println("    Hello World!");
  delay(2000);
  
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  double x, y;
}

void loop() 
{
  float x_values, y_values, z_values;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();
  z_values = lis.getAccelerationZ();

  // Serial.print("X: "); Serial.print(x_values);
  // Serial.print(" Y: "); Serial.print(y_values);
  // Serial.print(" Z: "); Serial.print(z_values);
  // Serial.println();
  
  double a;
  double b;
  double c;
  double x;
  uint16_t state = 0;
  for (x = 0; x <= 599; x += 1)
  {
    x_values = lis.getAccelerationX();
    a = x_values * 250;
    y_values = lis.getAccelerationY();
    b = y_values * 250;
    z_values = lis.getAccelerationZ();
    c = z_values * 250;

    update1 = true;
    TraceAndBackground(tft, x, a, 1, 40, 220, 270, 190, 0, 605, 100, -400, 400, 100, "3 Axis IMU", "", "", update1, GREEN);
    update1 = true;
    TraceOnly(tft, x, b, 1, 40, 220, 270, 190, 0, 605, 100, -400, 400, 100, "", "", "", update1, CYAN);
    update1 = true;
    TraceOnly(tft, x, c, 1, 40, 220, 270, 190, 0, 605, 100, -400, 400, 100, "", "", "", update1, WHITE);

    // delay(10);
    // Serial.print("X: "); Serial.print(a);
    // Serial.print(" Y: "); Serial.print(b);
    // Serial.print(" Z: "); Serial.print(c);
    // Serial.println();
  }
  tft.fillScreen(BLACK);
}
