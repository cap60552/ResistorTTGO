#include <TFT_eSPI.h> 
#include "resistor.h"

/*
 * Screen Size: 240x320
 * 
 * Modify User_Setup.h
 * 
 * #define ILI9341_DRIVER
 * 
 * #define TFT_MISO 25
 * #define TFT_MOSI 23
 * #define TFT_SCLK 26
 * #define TFT_CS   19  // Chip select control pin
 * #define TFT_DC   22  // Data Command control pin
 * #define TFT_RST  21  // Reset pin
 * 
 * 
 * 
 */

const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmLedChannelTFT = 0;

const int valueBox_x = 86;
const int valueBox_y = 110;
const int toleranceSelector_x = 86;
const int bandSelectorLabel_w = 16;

TFT_eSPI tft = TFT_eSPI(); 
int tt[4]={1,2,5,10};
int cT[4]={TFT_BROWN,TFT_RED,TFT_GOLD,TFT_SILVER};
int colors[12]={TFT_BLACK,TFT_BROWN,TFT_RED,TFT_ORANGE,TFT_YELLOW,TFT_GREEN,TFT_BLUE,TFT_VIOLET,TFT_LIGHTGREY, TFT_WHITE, TFT_GOLD,TFT_SILVER };
double multis[12]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000,0.1,0.01};
String mm[12]={"x1","x10","x100","x1K","x10K","x100K","x1M","x10M","x100M","x1G","x0.1","x0.01"};
int band=0;
int color=0;

int first=1;
int secon=2;
int multiplier=1;
int tolerance=2;
int deb1=0;
int deb2=0;

int x=86;
int y=32;
int w=126;
int h=20;

int bPos[4]={x+45,x+57,x+69,x+97};

int buttonPressed = false;
unsigned long debounce;

void setup() {
  debounce = millis();
  touchAttachInterrupt(14, upbutton, 40);  // UP - threshold 40
  touchAttachInterrupt(13, downbutton, 40);  // DOWN - threshold 40
  touchAttachInterrupt(12, rightbutton, 40);  // RIGHT - threshold 40
  touchAttachInterrupt(27, leftbutton, 40);  // LEFT - threshold 40
  touchAttachInterrupt(15, triggerbutton, 40);  // TRIGGER - threshold 40

  pinMode(0,INPUT_PULLUP);
  pinMode(35,INPUT_PULLUP);

  //ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
  //ledcAttachPin(TFT_BL, pwmLedChannelTFT);
  //ledcWrite(pwmLedChannelTFT, 100);
  
  tft.init();
  tft.setSwapBytes(true);
  tft.setRotation(3); // rotate 3*(pi/2)
  
  tft.fillScreen(TFT_BLACK);
  tft.drawString("Resistor CALCULATOR",x,0,2);

  /* Draw resistor image */
  tft.pushImage(x,y,148,60,resistor);

  /* Initial Resistor Bands */
  tft.fillRect(bPos[0],y+10,6,40,colors[first]);
  tft.fillRect(bPos[1],y+10,6,40,colors[secon]);
  tft.fillRect(bPos[2],y+10,6,40,colors[multiplier]);
  tft.fillRect(bPos[3],y+10,6,40,cT[tolerance]);

  /* Initial band selection indicator */
  tft.fillCircle(bPos[band]+2,y + 60,4,TFT_WHITE); 

  
   for(int i=0;i<12;i++)
   {
    
    
    if(i<10){
    //tft.fillRect(0,i*13,11,13,colors[i]); // Band 1 Color Box
    //tft.fillRect(22,i*13,11,13,colors[i]); // Band 2 Color Box
    tft.fillRect(0,i*24,bandSelectorLabel_w,24,colors[i]); // Band 1 Color Box
    tft.fillRect(22+bandSelectorLabel_w,i*24,bandSelectorLabel_w,24,colors[i]); // Band 2 Color Box
    if(i==0)
    
    tft.setTextColor(TFT_WHITE,colors[i]);
    else
    tft.setTextColor(TFT_BLACK,colors[i]);
    
    tft.drawString(String(i),5,(i*24)+2,2);
    tft.drawString(String(i),27 + bandSelectorLabel_w,(i*24)+2,2);

    // Draw Multiplier
    tft.setTextColor(TFT_WHITE,TFT_BLACK);
    }
    if(i==0)
    tft.drawCircle(310,(i*19) + bandSelectorLabel_w,8,TFT_WHITE);
    else
    tft.fillCircle(310,(i*19)+ bandSelectorLabel_w,8,colors[i]);

    tft.drawString(mm[i],265,(i*19)+12);
    
   }

    /*
    tft.drawString("<",14,(first*13)+2);
    tft.drawString("<",36,(secon*13)+2);
     */

     tft.fillCircle(20,(first*24)+12,3,TFT_WHITE); // Band 1 indicator
     tft.fillCircle(42 + bandSelectorLabel_w,(secon*24)+12,3,TFT_WHITE); // Band 2 indicator
     tft.fillCircle(252,(multiplier*19)+15,3,TFT_WHITE); // Initial Multiplier indicator
    
    // draw tolerance % boxes
    for(int i=0;i<4;i++){
    tft.fillRect(toleranceSelector_x + (i*37),217,37,11,cT[i]);
    tft.setTextColor(TFT_BLACK,cT[i]);
    tft.drawString(String(tt[i])+"%",toleranceSelector_x + 10 +(i*37),219);
    }
    tft.fillCircle(toleranceSelector_x + 17 +(tolerance*37),233,2,TFT_WHITE);
   tft.setTextColor(TFT_WHITE,TFT_BLACK); 
}

void loop() {


  if (buttonPressed) {
       double res=((first*10)+secon)*multis[multiplier];
       tft.fillRect(valueBox_x,valueBox_y,148,96,0x00A5);
       double maxx=res+(res*(tt[tolerance]/100.00));
       double minn=res-(res*(tt[tolerance]/100.00));
       tft.setTextColor(TFT_WHITE,0x00A5); 
  
       if(res>=1000 && res<1000000){
       tft.drawString(String(res/1000)+" K",valueBox_x + 2,valueBox_y + 6, 4);
       tft.drawString(String(first)+String(secon)+"x10",valueBox_x + 2,valueBox_y + 50, 2);
       tft.drawString(String(multiplier),valueBox_x + 37,valueBox_y + 45); // Super script
       tft.drawString("MAX:"+String(maxx),valueBox_x + 2,valueBox_y + 70);
       tft.drawString("MIN:"+String(minn),valueBox_x + 2,valueBox_y + 82);
       }
       
       else if(res>=1000000 && res<1000000000)
       {
       tft.drawString(String(res/1000000)+" M",valueBox_x + 2,valueBox_y + 6, 4);
       tft.drawString(String(first)+String(secon)+"x10",valueBox_x + 2,valueBox_y + 50, 2);
       tft.drawString(String(multiplier),valueBox_x + 37,valueBox_y + 45); // Super script
       tft.drawString("MAX:"+String(maxx),valueBox_x + 2,valueBox_y + 70);
       tft.drawString("MIN:"+String(minn),valueBox_x + 2,valueBox_y + 82);
       }

       else if(res>=1000000000)
       {
       tft.drawString(String(res/1000000000)+" G",valueBox_x + 2,valueBox_y + 6, 4);
       tft.drawString(String(first)+String(secon)+"x10",valueBox_x + 2,valueBox_y + 50, 2);
       tft.drawString(String(multiplier),valueBox_x + 37,valueBox_y + 45); // Super script
       tft.drawString("MAX:"+String(maxx),valueBox_x + 2,valueBox_y + 70);
       tft.drawString("MIN:"+String(minn),valueBox_x + 2,valueBox_y + 82);
       }

       
       else
       {
       tft.drawString(String(res)+" ohm",valueBox_x + 2,valueBox_y + 6, 4);
       tft.drawString(String(first)+String(secon)+"x10",valueBox_x + 2,valueBox_y + 50, 2);
       tft.drawString(String(multiplier),valueBox_x + 37,valueBox_y + 45); // Super script
       tft.drawString("MAX:"+String(maxx),valueBox_x + 2,valueBox_y + 70);
       tft.drawString("MIN:"+String(minn),valueBox_x + 2,valueBox_y + 82);
       }
  
       tft.setTextColor(TFT_WHITE,TFT_BLACK); 
  
       buttonPressed = false;
  
  }

}

void upbutton() {
  if ((millis() - debounce) > 200)
  {
    debounce = millis();
  
    buttonPressed = true;
    if(band==0)
    {
      tft.fillCircle(20,(first*24)+12,3,TFT_BLACK);
      first--;
      if(first<0) {
        first=9;
      }
      tft.fillRect(bPos[band],y+10,6,40,colors[first]);
      tft.fillCircle(20,(first*24)+12,3,TFT_WHITE);
    }  

    if(band==1)
    {
      tft.fillCircle(42 + bandSelectorLabel_w,(secon*24)+12,3,TFT_BLACK);
      secon--;
      if(secon<0){
        secon=9;
      }
      tft.fillRect(bPos[band],y+10,6,40,colors[secon]);
      tft.fillCircle(42 + bandSelectorLabel_w,(secon*24)+12,3,TFT_WHITE);
    }

    if(band==2)
    {
      tft.fillCircle(252,(multiplier*19)+15,3,TFT_BLACK);
      multiplier--;
      if(multiplier<0){
        multiplier=11;
      }  
      tft.fillRect(bPos[band],y+10,6,40,colors[multiplier]);      
      tft.fillCircle(252,(multiplier*19)+15,3,TFT_WHITE);
    }

    if(band==3)
    {
      tft.fillCircle(toleranceSelector_x + 17 +(tolerance*37),233,2,TFT_BLACK);
      tolerance++;
      if(tolerance>3) {
        tolerance=0;
      }
      tft.fillRect(bPos[band],y+10,6,40,cT[tolerance]);
      tft.fillCircle(toleranceSelector_x + 17 +(tolerance*37),233,2,TFT_WHITE);
      
    }


    
  }  
}

void downbutton() {
  if ((millis() - debounce) > 200)
  {
    debounce = millis();
  
    buttonPressed = true;
    if(band==0)
    {
      tft.fillCircle(20,(first*24)+12,3,TFT_BLACK);
      first++;
      if(first>9) {
        first=0;
      }
      tft.fillCircle(20,(first*24)+12,3,TFT_WHITE);
      tft.fillRect(x+45,y+10,6,40,colors[first]);      
    }  

    if(band==1)
    {
      tft.fillCircle(42 + bandSelectorLabel_w,(secon*24)+12,3,TFT_BLACK);
      secon++;
      if(secon>9){
        secon=0;
      }
      tft.fillCircle(42 + bandSelectorLabel_w,(secon*24)+12,3,TFT_WHITE);
      tft.fillRect(x+57,y+10,6,40,colors[secon]);
    }

    if(band==2)
    {
      tft.fillCircle(252,(multiplier*19)+15,3,TFT_BLACK);
      multiplier++;
      if(multiplier>11){
        multiplier=0;
      }  
      tft.fillCircle(252,(multiplier*19)+15,3,TFT_WHITE);
      tft.fillRect(x+69,y+10,6,40,colors[multiplier]);      
    }

    if(band==3)
    {
      tft.fillCircle(toleranceSelector_x + 17 +(tolerance*37),233,2,TFT_BLACK);
      tolerance--;
      if(tolerance<0) {
        tolerance=3;
      }
      tft.fillCircle(toleranceSelector_x + 17 +(tolerance*37),233,2,TFT_WHITE);
      tft.fillRect(x+97,y+10,6,40,cT[tolerance]);
    }
    
  }
}

void leftbutton() {
  buttonPressed = true;

  if ((millis() - debounce) > 200)
  {
    debounce = millis();

    buttonPressed = true;
    tft.fillCircle(bPos[band]+2,y + 60,4,TFT_BLACK);
    deb1=1;
    band--;
    if(band<0)
    band=3; 
    
    tft.fillCircle(bPos[band]+2,y + 60,4,TFT_WHITE);

  }  
  
}

void rightbutton() {
  buttonPressed = true;

  if ((millis() - debounce) > 200)
  {
    debounce = millis();

    buttonPressed = true;
    tft.fillCircle(bPos[band]+2,y + 60,4,TFT_BLACK);
    deb1=1;
    band++;
    if(band>3)
    band=0; 
    tft.fillCircle(bPos[band]+2,y + 60,4,TFT_WHITE);

  }  
  
}

void triggerbutton() {


}
