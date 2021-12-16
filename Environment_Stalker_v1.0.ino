#include <Wire.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h>
#include <Adafruit_BMP280.h>
#include <HTU21D.h>
#define TFT_RST   8
#define TFT_CS    10
#define TFT_DC    9
#define TFT_BLACK 0x0000
#define TFT_WHITE 0xFFFF
#define TFT_GREEN 0x2DE0
#define TFT_RED 0xF800
#define TFT_GREY 0xD69A
#define TFT_CYAN 0x1E1C
#define BMP280_I2C_ADDRESS  0x76
  float maxTemp = -999;
  float minTemp = 999;
  float maxPres = 10;
  float minPres = 40;
  float maxHumi = -95;
  float minHumi = 95;
//  float maxHic = -999;
//  float minHic = 999;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BMP280  bmp280;


void setlinecolour(int x, int y, int size, int colour1, int colour2, char * text)
{
    tft.setCursor(x, y);                              // move cursor to position (5, 76) pixel
    tft.setTextSize(size);
    tft.setTextColor(colour1, colour2);
    tft.print(text);
}

  char _buffer[16];

  HTU21D myHTU21D(HTU21D_RES_RH12_TEMP14);
  
void setup(void)
{
  Serial.begin(9600);
  delay(500);
  Serial.println("Environment Stalker v1.0 - Coded by paul@fackinel.com");
  Serial.println("Local environment sensor readings, with low to medium accuracy.");
  Serial.println();

  tft.initR(INITR_144GREENTAB);
  tft.fillScreen(TFT_WHITE);
  tft.drawFastHLine(0, 15, tft.width(), TFT_BLACK);
  tft.drawFastHLine(0, 16, tft.width(), TFT_GREY);
  setlinecolour(55,4,1,TFT_BLACK,TFT_WHITE,"ENV.STK v1.0");

 while (myHTU21D.begin() != true)
  {
    tft.setTextColor(TFT_RED, TFT_BLACK);         // set text color to red and black background
    tft.setTextSize(2);                                // setting text size to 2
    tft.setCursor(5, 45);                              // move cursor to position (5, 76) pixel
    tft.print("HTU device");
    tft.setCursor(10, 70);                            // move cursor to position (35, 100) pixel
    tft.print("not found");
    Serial.println(F("HTU sensor malfunction, not initialising, device stopped."));
    Serial.println();
  }
  Serial.println(F("HTU21D, SHT21 sensor is active"));

  if ( bmp280.begin(BMP280_I2C_ADDRESS) == 0 )   // call bmp using the specified address modifier
  { // connection error or device address wrong!
    tft.setTextColor(TFT_RED, TFT_WHITE);         // set text color to red and black background
    tft.setTextSize(2);                                // setting text size to 2
    tft.setCursor(5, 45);                              // move cursor to position (5, 76) pixel
    tft.print("BMP device");
    tft.setCursor(10, 70);                            // move cursor to position (35, 100) pixel
    tft.print("not found");
    Serial.println(F("BMP-280 malfunction, not initialising, device stopped."));
    Serial.println();
    while (1);  // stay here
  }

    bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     // Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      // Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time. */
                  
  tft.drawFastHLine(0,52,tft.width(), TFT_BLACK);  // draw horizontal seperation line at position (0, 55)pixel
  tft.drawFastHLine(0,53,tft.width(), TFT_GREY);  // draw horizontal seperation line at position (0, 55)pixel
  tft.drawFastHLine(0,91,tft.width(), TFT_BLACK);  // draw horizontal seperation line at position (0, 195)pixel
  tft.drawFastHLine(0,92,tft.width(), TFT_GREY);  // draw horizontal seperation line at position (0, 195)pixel
  tft.setTextColor(TFT_GREY, TFT_WHITE);                 // set text color to red and black background
  tft.setTextSize(4);
  tft.setCursor(1, 20);                                // move cursor to position (30, 20) pixel
  tft.print("T");                            // setting heading for first section
  tft.setCursor(1, 58);                                // move cursor to position (40, 60) pixel
  tft.print("H");                               // setting heading for second section
  tft.setCursor(1, 97);                               // move cursor to position (40, 100) pixel
  tft.print("P");                               // setting heading for third section
}
// main loop
void loop()
{

  // read temperature, humidity and pressure from the BMP280 sensor
    float tempdht = myHTU21D.readTemperature();
    float humi = myHTU21D.readHumidity();
    float compohumi = myHTU21D.readCompensatedHumidity();
    float temp = bmp280.readTemperature();                             // get temperature in °C
//  float hic = dht.computeHeatIndex(tempdht, humi, false);                      // Heat index temp in °c
    float pres = bmp280.readPressure() * (0.0295301 / 100);   // get pressure in inHg  taking into respect the change in temperature has on reading
    float alt = bmp280.readAltitude(1017.0);                             // get pressure in hPa

   //min-max calcs
  if (tempdht > maxTemp) maxTemp = tempdht;
  if (tempdht < minTemp) minTemp = tempdht;
  if (humi > maxHumi) maxHumi = humi;
  if (humi < minHumi) minHumi = humi;
  if (pres > maxPres) maxPres = pres;
  if (pres < minPres) minPres = pres;
//  if (hic > maxHic) maxHic = hic;
//  if (hic < minHic) minHic = hic;
  
  // print temperature (in °C)
  if(tempdht < 0)                              // if temperature < 0
    sprintf( _buffer, "-%02u.%02u", (int)abs(tempdht), (int)(abs(tempdht) * 100) % 100 );
  else                                      // if temperature >= 0
    sprintf( _buffer, " %02u.%02u", (int)tempdht, (int)(tempdht * 100) % 100 );// setting the value approximation
   setlinecolour(24,20,1,TFT_BLACK,TFT_WHITE,"Live:");
  if (tempdht < 0)
      tft.setTextColor(TFT_CYAN, TFT_WHITE);
  else
      tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.print(_buffer);                             // print temperature from BMP-280 sensor
  tft.print(" c");
  tft.drawCircle(93, 21, 1, TFT_BLACK);   
  //setlinecolour(24,31,1,TFT_BLACK,TFT_WHITE,"Humi: ");
  //tft.print(temp);
  //  if(hic < 0)                              // if temperature < 0
//  sprintf( _buffer, "-%02u.%02u", (int)abs(hic), (int)(abs(hic) * 100) % 100 );
//  else                                      // if temperature >= 0
//  sprintf( _buffer, " %02u.%02u", (int)hic, (int)(hic * 100) % 100 );// setting the value approximation
//  if(hic < 0)
//   tft.setTextColor(TFT_CYAN, TFT_WHITE);
//  else
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
//  tft.print(_buffer);                             // print temperature from BMP-280 sensor
//  tft.print(" c");
//  tft.drawCircle(93, 32, 1, TFT_BLACK);                // print the degree symbol ( ° )(can be omitted if * is used instead)
  tft.setTextColor(TFT_RED, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(18, 41);                          // move cursor to position (11,34) pixel
  if(minTemp < 0)                              // if temperature < 0
    sprintf( _buffer, "-%02u.%02u", (int)abs(minTemp), (int)(abs(minTemp) * 100) % 100 );
  else                                      // if temperature >= 0
    sprintf( _buffer, " %02u.%02u", (int)minTemp, (int)(minTemp * 100) % 100 );// setting the value approximation
  tft.print(_buffer);                             // print temperature from BMP-280 sensor
  tft.drawCircle(55, 42, 1, TFT_RED);                // print the degree symbol ( ° )(can be omitted if * is used instead)
  setlinecolour(57,41,1,TFT_RED,TFT_WHITE,"c");
  setlinecolour(68,41,1,TFT_GREY,TFT_WHITE,"->");
  tft.setTextColor(TFT_GREEN, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(82, 41);                          // move cursor to position (11,34) pixel
  if(maxTemp < 0)                              // if temperature < 0
    sprintf( _buffer, "-%02u.%02u", (int)abs(maxTemp), (int)(abs(maxTemp) * 100) % 100 );
  else                                      // if temperature >= 0
    sprintf( _buffer, " %02u.%02u", (int)maxTemp, (int)(maxTemp * 100) % 100 );// setting the value approximation
  tft.print(_buffer);                             // print temperature from BMP-280 sensor
  tft.drawCircle(119, 42, 1, TFT_GREEN);                // print the degree symbol ( ° )(can be omitted if * is used instead)
  setlinecolour(121,42,1,TFT_GREEN,TFT_WHITE,"c");
  // 2: print humidity (in %)
  dtostrf(humi, 4, 2, _buffer);
  //sprintf( _buffer, " %02u.%02u", (int)humi, (int)(humi * 100) % 100);    //, (int)(humi * 100) % 100 );          // setting the value approximation , (int)temp, (int)(temp * 100) % 100 );
  setlinecolour(24,58,1,TFT_BLACK,TFT_WHITE,"Live:");
  tft.print(_buffer);                                 // print humidity from DHT-11 sensor
  tft.print("%");                                  // print the percentage symbol
  setlinecolour(24,69,1,TFT_BLACK,TFT_WHITE,"Rel.:");
  tft.print(compohumi);
  tft.setTextColor(TFT_RED, TFT_WHITE);  // set text color to yellow and black background
//  if(minHic < 0)
//    sprintf( _buffer, "-%02u.%02u", (int)minHic, (int)(minHic) );
//  else                                      // if temperature >= 0
//    sprintf( _buffer, "%02u.%02u", (int)minHic, (int)(minHic) );// setting the value approximation
//  tft.print(_buffer);
  setlinecolour(86,69,1,TFT_GREY,TFT_WHITE,"% ");               
  tft.setTextColor(TFT_GREEN, TFT_WHITE);  // set text color to yellow and black background
//  tft.print(maxHic);               
  tft.setTextColor(TFT_RED, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(24, 79);                          // move cursor to position (11,34) pixel
  tft.print(minHumi);                             // print temperature from BMP-280 sensor
  tft.print("%");
  setlinecolour(68,79,1,TFT_GREY,TFT_WHITE,"->");
  tft.setTextColor(TFT_GREEN, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(84, 79);                          // move cursor to position (11,34) pixel
  tft.print(maxHumi);                             // print temperature from BMP-280 sensor
  tft.print("%");
  // 3: print pressure (in hPa)
  dtostrf(pres, 4, 2, _buffer);
  setlinecolour(24,97,1,TFT_BLACK,TFT_WHITE,"Live: ");
  tft.print(_buffer);                                 // readout the pressure
  tft.print(" inHg");                                    // print unit of atmospheric pressure as hecto pascal
  tft.setTextColor(TFT_RED, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(29,118);                          // move cursor to position (11,34) pixel
  tft.print(minPres);                             // print temperature from BMP-280 sensor
  setlinecolour(68,118,1,TFT_GREY,TFT_WHITE,"->");
  tft.setTextColor(TFT_GREEN, TFT_WHITE);  // set text color to yellow and black background
  tft.setCursor(88,118);                          // move cursor to position (11,34) pixel
  tft.print(maxPres);                             // print temperature from BMP-280 sensor
  //4: print altimeter reading - totally inaccurate unless specifically calibrated to day
  setlinecolour(24,107,1,TFT_GREY,TFT_WHITE,"Above sea:");
  tft.print(alt);               //
  tft.print("m");
    delay(1000);            // delay 20 seconds
 }
