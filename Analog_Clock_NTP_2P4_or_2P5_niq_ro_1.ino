/******************************************************************************
    -----------
    Steps to use
    -----------

    1) In the sketch (i.e. this example):
    
    - Set values for NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, PANEL_CHAIN. 
      There are comments beside them explaining what they are in more detail.
    - Other than where the matrix is defined and matrix.begin in the setup, you 
      should now be using the virtual display for everything (drawing pixels, writing text etc).
       You can do a find and replace of all calls if it's an existing sketch
      (just make sure you don't replace the definition and the matrix.begin)
    - If the sketch makes use of MATRIX_HEIGHT or MATRIX_WIDTH, these will need to be 
      replaced with the width and height of your virtual screen. 
      Either make new defines and use that, or you can use virtualDisp.width() or .height()

    Thanks to:

    * Brian Lough for the original example as raised in this issue:
      https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA/issues/26

      YouTube: https://www.youtube.com/brianlough
      Tindie: https://www.tindie.com/stores/brianlough/
      Twitter: https://twitter.com/witnessmenow

    * Galaxy-Man for the kind donation of panels make/test that this is possible:
      https://github.com/Galaxy-Man

*****************************************************************************/


 /******************************************************************************
  * VIRTUAL DISPLAY / MATRIX PANEL CHAINING CONFIGURATION
  * 
  * Note 1: If chaining from the top right to the left, and then S curving down
  *         then serpentine_chain = true and top_down_chain = true
  *         (these being the last two parameters of the virtualDisp(...) constructor.
  * 
  * Note 2: If chaining starts from the bottom up, then top_down_chain = false.
  * 
  * Note 3: By default, this library has serpentine_chain = true, that is, every 
  *         second row has the panels 'upside down' (rotated 180), so the output
  *         pin of the row above is right above the input connector of the next 
  *         row.
  
  Example 1 panel chaining:
  +-----------------+-----------------+-------------------+
  | 64x32px PANEL 3 | 64x32px PANEL 2 | 64x32px PANEL 1   |
  |        ------------   <--------   | ------------xx    |
  | [OUT]  |   [IN] | [OUT]      [IN] | [OUT]    [ESP IN] |
  +--------|--------+-----------------+-------------------+
  | 64x32px|PANEL 4 | 64x32px PANEL 5 | 64x32px PANEL 6   |
  |       \|/   ---------->           |  ----->           |
  | [IN]      [OUT] | [IN]      [OUT] | [IN]      [OUT]   |
  +-----------------+-----------------+-------------------+

  Example 1 configuration:
    
    #define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
    #define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

    #define NUM_ROWS 2 // Number of rows of chained INDIVIDUAL PANELS
    #define NUM_COLS 3 // Number of INDIVIDUAL PANELS per ROW

    virtualDisp(dma_display, NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, true, true);

    = 192x64 px virtual display, with the top left of panel 3 being pixel co-ord (0,0)

  ==========================================================

  Example 2 panel chaining:

  +-------------------+
  | 64x32px PANEL 1   |
  | ----------------- |
  | [OUT]    [ESP IN] |
  +-------------------+
  | 64x32px PANEL 2   |
  |                   |
  | [IN]      [OUT]   |
  +-------------------+
  | 64x32px PANEL 3   |
  |                   |
  | [OUT]      [IN]   |
  +-------------------+
  | 64x32px PANEL 4   |
  |                   |
  | [IN]      [OUT]   |
  +-------------------+    

  Example 2 configuration:
    
    #define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
    #define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

    #define NUM_ROWS 4 // Number of rows of chained INDIVIDUAL PANELS
    #define NUM_COLS 1 // Number of INDIVIDUAL PANELS per ROW

    virtualDisp(dma_display, NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, true, true);

    virtualDisp(dma_display, NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, true, true);     

    = 128x64 px virtual display, with the top left of panel 1 being pixel co-ord (0,0)

  ==========================================================

  Example 3 panel chaining (bottom up):

  +-----------------+-----------------+
  | 64x32px PANEL 4 | 64x32px PANEL 3 | 
  |             <----------           | 
  | [OUT]      [IN] | [OUT]      [in] | 
  +-----------------+-----------------+
  | 64x32px PANEL 1 | 64x32px PANEL 2 | 
  |             ---------->           | 
  | [ESP IN]  [OUT] | [IN]      [OUT] | 
  +-----------------+-----------------+

  Example 1 configuration:
    
    #define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
    #define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

    #define NUM_ROWS 2 // Number of rows of chained INDIVIDUAL PANELS
    #define NUM_COLS 2 // Number of INDIVIDUAL PANELS per ROW

    virtualDisp(dma_display, NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, true, false);

    = 128x64 px virtual display, with the top left of panel 4 being pixel co-ord (0,0)  

ver.0 - niq_ro created replica for Ady Sos to simulated Analog Clock on 2xP4 or 2xP5 (64x32) led display 
        instead P3 (64x64) led display as at https://github.com/bmzstore/Analog-Clock-NTP-ESP-32-With-Panel-LED-P3
        original Analog Clock NTP is based on https://github.com/3tawi/RGBmatrixPanelGP library
        clock personalised as https://nicuflorica.blogspot.com/search?q=ceas+analogic 
ver.1 - added wifi and NTP clock,
*/

#define PANEL_RES_X 64 // Number of pixels wide of each INDIVIDUAL panel module. 
#define PANEL_RES_Y 32 // Number of pixels tall of each INDIVIDUAL panel module.

#define NUM_ROWS 2 // Number of rows of chained INDIVIDUAL PANELS
#define NUM_COLS 1 // Number of INDIVIDUAL PANELS per ROW
#define PANEL_CHAIN NUM_ROWS*NUM_COLS    // total number of panels chained one to another

int WIDTH1 = PANEL_RES_X * NUM_COLS;
int HEIGHT1 = PANEL_RES_Y * NUM_ROWS;
int CENTRE_X = (WIDTH1 / 2) - 1;
int CENTRE_Y = (HEIGHT1 / 2) - 1;
#define R_HOUR 17
#define R_MIN  25
#define R_SEC  29

// Change this to your needs, for details on VirtualPanel pls read the PDF!
#define SERPENT true
#define TOPDOWN false

// library includes
#include "WiFi.h"
#include <DS1307ESP.h>                         // DS1307ESP Library: https://github.com/3tawi/DS1307ESP
#include <ESP32-VirtualMatrixPanel-I2S-DMA.h>  // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-DMA

// placeholder for the matrix object
MatrixPanel_I2S_DMA *dma_display = nullptr;

// placeholder for the virtual display object
VirtualMatrixPanel  *virtualDisp = nullptr;

DS1307ESP rtc;

char ssid[] = "niq_ro"; // Nama Wifi network
char pass[] = "Beremutasirece1!"; // Password Wifi network
#define TZ (+2*60*60) // Timezone UTC+2

int ora;
int minut;
int secunda;
uint32_t lastTime; 

/******************************************************************************
 * Setup!
 ******************************************************************************/
void setup() {
  
  delay(2000);
  Serial.begin(115200);
  Serial.println(""); Serial.println(""); Serial.println("");
  Serial.println("*****************************************************");
  Serial.println(" Analog Clock NTP on 2xP4 (2xP5) 64x32 led display !");
  Serial.println("*****************************************************");

  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  configTime(TZ, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp"); 


  /******************************************************************************
   * Create physical DMA panel class AND virtual (chained) display class.   
   ******************************************************************************/

  /*
    The configuration for MatrixPanel_I2S_DMA object is held in HUB75_I2S_CFG structure,
    All options has it's predefined default values. So we can create a new structure and redefine only the options we need

	Please refer to the '2_PatternPlasma.ino' example for detailed example of how to use the MatrixPanel_I2S_DMA configuration
  */

  HUB75_I2S_CFG mxconfig(
                PANEL_RES_X,   // module width
                PANEL_RES_Y,   // module height
                PANEL_CHAIN    // chain length
  );

  //mxconfig.driver = HUB75_I2S_CFG::FM6126A;     // in case that we use panels based on FM6126A chip, we can set it here before creating MatrixPanel_I2S_DMA object

  // Sanity checks
  if (NUM_ROWS <= 1) {
    Serial.println(F("There is no reason to use the VirtualDisplay class for a single horizontal chain and row!"));
  }

  // OK, now we can create our matrix object
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // let's adjust default brightness to about 75%
  dma_display->setBrightness8(192);    // range is 0-255, 0 - 0%, 255 - 100%

  // Allocate memory and start DMA display
  if( not dma_display->begin() )
      Serial.println("****** !KABOOM! I2S memory allocation failed ***********");

  // create VirtualDisplay object based on our newly created dma_display object
  virtualDisp = new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y, SERPENT, TOPDOWN);
/*
  // So far so good, so continue
  virtualDisp->fillScreen(virtualDisp->color444(0, 0, 0));
  virtualDisp->drawDisplayTest(); // draw text numbering on each screen to check connectivity

  delay(3000);
*/
  Serial.println("Chain of 64x32 panels for this example:");
  Serial.println("+--------+---------+");
  Serial.println("|   4    |    3    |");
  Serial.println("|        |         |");
  Serial.println("+--------+---------+");
  Serial.println("|   1    |    2    |");
  Serial.println("| (ESP)  |         |");
  Serial.println("+--------+---------+");

//   virtualDisp->setFont(&FreeSansBold12pt7b);
   virtualDisp->setTextColor(virtualDisp->color565(0, 0, 255));
   virtualDisp->setTextSize(1); 
   virtualDisp->setCursor(10, virtualDisp->height()-20); 
   
   // Red text inside red rect (2 pix in from edge)
   virtualDisp->print("clock");
   virtualDisp->drawRect(1,1, virtualDisp->width()-2, virtualDisp->height()-2, virtualDisp->color565(255,0,0));
/*
   // White line from top left to bottom right
   virtualDisp->drawLine(0,0, virtualDisp->width()-1, virtualDisp->height()-1, virtualDisp->color565(255,255,255));
*/
delay(3000);
  virtualDisp->fillScreen(virtualDisp->color565(0, 0, 0));  // clear the display

ora = 2;  // random (0,12);
minut = 22;  // random (0,60);
secunda = 0; //random (0,60);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED)
    ESP.restart();

drawtime();

delay(1000);

} // end loop

void drawtime()
{
  time_t t;
  static time_t last_t;
  struct tm *tm;
  t = time(NULL);
  if (last_t == t) return;
  last_t = t;
  tm = localtime(&t);

            double radians;
            int x;
            int y;
            char jam[9];
            char menit[9];
            int j = 0;
            rtc.DSread();
            ora = tm->tm_hour;
            minut = tm->tm_min;
            secunda = tm->tm_sec;
            if (ora > 12)
                ora -= 12; 
    
            
         virtualDisp->fillScreen(virtualDisp->color565(0, 0, 0));  // clear the display
         
         virtualDisp->drawCircle(CENTRE_X, CENTRE_Y, 31, virtualDisp->color444(0, 255, 0));  // draw a greencircle

            // hour 3 point
            virtualDisp->drawLine(62,29, virtualDisp->width()-2, virtualDisp->height()-31, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(61,29, virtualDisp->width()-3, virtualDisp->height()-31, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(60,30, virtualDisp->width()-4, virtualDisp->height()-32, virtualDisp->color565(255,255,255));
            virtualDisp->drawPixelRGB888(59, 31, 255, 255, 255);

            // hour 6 point       
            virtualDisp->drawLine(29,62, virtualDisp->width()-31, virtualDisp->height()-2, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(29,61, virtualDisp->width()-31, virtualDisp->height()-3, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(30,60, virtualDisp->width()-32, virtualDisp->height()-4, virtualDisp->color565(255,255,255));
            virtualDisp->drawPixelRGB888(31, 59, 255, 255, 255);

            // hour 9 point
            virtualDisp->drawLine(0,29, virtualDisp->width()-64, virtualDisp->height()-31, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(1,29, virtualDisp->width()-63, virtualDisp->height()-31, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(2,30, virtualDisp->width()-62, virtualDisp->height()-32, virtualDisp->color565(255,255,255));
            virtualDisp->drawPixelRGB888(3, 31, 255, 255, 255);

            //12 hour point
            virtualDisp->drawLine(29,0, virtualDisp->width()-31, virtualDisp->height()-64, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(29,1, virtualDisp->width()-31, virtualDisp->height()-63, virtualDisp->color565(255,255,255));
            virtualDisp->drawLine(30,2, virtualDisp->width()-32, virtualDisp->height()-62, virtualDisp->color565(255,255,255));
            virtualDisp->drawPixelRGB888(31, 3, 255, 255, 255);

           virtualDisp->setTextColor(virtualDisp->color565(255, 0, 255));
           virtualDisp->setTextSize(1); 
            
           // hour 1 (text)
            virtualDisp->setCursor(43, virtualDisp->height()-57); // 63-x+3
            virtualDisp->print("1");

           // hour 2 (text)
            virtualDisp->setCursor(51, virtualDisp->height()-47); 
            virtualDisp->print("2");
            
           // hour 3 (text)
            virtualDisp->setCursor(54, virtualDisp->height()-36); 
            virtualDisp->print("3");
            
           // hour 4 (text)
            virtualDisp->setCursor(51, virtualDisp->height()-24); 
            virtualDisp->print("4");

           // hour 5 (text)
            virtualDisp->setCursor(42, virtualDisp->height()-15); 
            virtualDisp->print("5");
            
           // hour 6 (text)
            virtualDisp->setCursor(29, virtualDisp->height()-12); 
            virtualDisp->print("6");

           // hour 7 (text)
            virtualDisp->setCursor(16, virtualDisp->height()-15); 
            virtualDisp->print("7");
            
           // hour 8 (text)
            virtualDisp->setCursor(8, virtualDisp->height()-24); 
            virtualDisp->print("8");
            
           // hour 9 (text)
            virtualDisp->setCursor(4, virtualDisp->height()-36); 
            virtualDisp->print("9");

           // hour 10 (text)
            virtualDisp->setCursor(6, virtualDisp->height()-47); 
            virtualDisp->print("1");
            virtualDisp->setCursor(11, virtualDisp->height()-47); 
            virtualDisp->print("0"); 

           // hour 11 (text)
            virtualDisp->setCursor(16, virtualDisp->height()-58); 
            virtualDisp->print("1");
            virtualDisp->setCursor(21, virtualDisp->height()-58); 
            virtualDisp->print("1");

           // hour 12 (text)
            virtualDisp->setCursor(27, virtualDisp->height()-60); 
            virtualDisp->print("1");
            virtualDisp->setCursor(32, virtualDisp->height()-60); 
            virtualDisp->print("2");


           for (int i = 0; i < 60; i+=5) // hour points
           {
            radians = (90.0F - (i * 6.0F)) * M_PI / 180.0;
            x = round(CENTRE_X + 30 * cos(radians));
            y = round(CENTRE_Y - 30 * sin(radians));
            virtualDisp->drawCircle(x, y, 1, virtualDisp->color444(255, 255, 255));  // draw a greencircle
            virtualDisp->drawPixelRGB888(x, y, 255, 255, 255);        
           }
           
            // second
            radians = (90.0F - ((float)secunda * 6.0F)) * M_PI / 180.0F;
            x = round(CENTRE_X + R_SEC * cos(radians));
            y = round(CENTRE_Y - R_SEC * sin(radians));

   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y-1, virtualDisp->color565(0,0,255));  // blue line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y+1, virtualDisp->color565(0,0,255));  // blue line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X-1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(0,0,255));  // blue line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X+1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(0,0,255));  // blue line from center to hour           

            // minute
            radians = (90.0F - (minut * 6.0F)) * M_PI / 180.0;
            x = round(CENTRE_X + R_MIN * cos(radians));
            y = round(CENTRE_Y - R_MIN * sin(radians));
            
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y-1, virtualDisp->color565(255,255,0));  // yellow line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y+1, virtualDisp->color565(255,255,0));  // yellow line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X-1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(255,255,0));  // yellow line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X+1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(255,255,0));  // yellow line from center to hour

           // hour
            radians = (90.0F - ((ora * 30.0F) + ((minut * 29.0F) / 60.0F))) * M_PI / 180.0F;
            x = round(CENTRE_X + R_HOUR * cos(radians));
            y = round(CENTRE_Y - R_HOUR * sin(radians));
             
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y-1, virtualDisp->color565(255,0,0));  // red line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X, virtualDisp->height()-CENTRE_Y+1, virtualDisp->color565(255,0,0));  // red line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X-1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(255,0,0));  // red line from center to hour
   virtualDisp->drawLine(x,y, virtualDisp->width()-CENTRE_X+1, virtualDisp->height()-CENTRE_Y, virtualDisp->color565(255,0,0));  // red line from center to hour
}
