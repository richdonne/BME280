/*
EnvironmentCalculations.cpp

Copyright (C) 2016  Tyler Glenn

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Written: Dec 30 2015.
Last Updated: Jan 1 2016. - Happy New year!

This header must be included in any derived code or copies of the code.

Some unit conversations courtesy
of www.endmemo.com, altitude equation courtesy of NOAA, and dew point equation
courtesy of Brian McNoldy at http://andrew.rsmas.miami.edu.
 */

#include <Arduino.h>
#include <Math.h>

#include "EnvironmentCalculations.h"
/****************************************************************/
float EnvironmentCalculations::Altitude
(
  float pressure,
  AltitudeUnit altUnit,
  float referencePressure,
  float outdoorTemp,
  TempUnit tempUnit
)
{
  // Equation inverse to EquivalentSeaLevelPressure calculation
  float altitude = NAN;
  if (!isnan(pressure) && !isnan(referencePressure) && !isnan(outdoorTemp))
  {
      if(tempUnit != TempUnit_Celsius)
          outdoorTemp = (outdoorTemp - 32.0) * (5.0 / 9.0); /*conversion to [�C]*/

      altitude = pow(referencePressure / pressure, 0.190234) - 1;
      altitude *= ((outdoorTemp + 273.15) / 0.0065);
      if(altUnit != AltitudeUnit_Meters)  altitude *= 3.28084;
  }
  return altitude;
}

/****************************************************************/
int EnvironmentCalculations::Heatindex
(
  float temperature,
  float humidity,
  TempUnit tempUnit
)
{
  float heatindex(NAN);
  const static float hi[9] PROGMEM = {-8.784695,1.61139411,2.338549,-0.14611605,-1.2308094/100,-1.6424828/100,2.211732/1000,7.2546/10000,-3.582/1000000};

  if ( !isnan(temperature) && !isnan(humidity) ) { 
    if (tempUnit != TempUnit_Celsius) {
            temperature = (temperature - 32.0) * (5.0 / 9.0); /*conversion to [°C]*/
    }

    if ( humidity>40 && temperature>26.7 ) {
      //taken from https://de.wikipedia.org/wiki/Hitzeindex#Berechnung
      heatindex = pgm_read_float(hi);
      heatindex += pgm_read_float(hi+1) * temperature;
      heatindex += pgm_read_float(hi+2) * humidity;
      heatindex += pgm_read_float(hi+3) * temperature * humidity;
      heatindex += pgm_read_float(hi+4) * temperature * temperature;
      heatindex += pgm_read_float(hi+5) * humidity * humidity;
      heatindex += pgm_read_float(hi+6) * temperature * temperature * humidity;
      heatindex += pgm_read_float(hi+7) * temperature * humidity * humidity;
      heatindex += pgm_read_float(hi+8) * temperature * temperature * humidity * humidity;
      if (tempUnit != TempUnit_Celsius) {
              return (int)round(heatindex * (9.0 / 5.0) + 32.0); /*conversion back to [°F]*/
      }
      else {
	      return (int)round(heatindex);
      }
    }
    // logical fallback if the formula does not apply to the given values
    else {
      if (tempUnit != TempUnit_Celsius) {
              return (int)(temperature * (9.0 / 5.0) + 32.0); /*conversion back to [°F]*/
      }
      else {
	      return (int)temperature;
      }
    }
  }
  // fallback if the parameter are not useful
  else {
	  return -1;
  }
}

/****************************************************************/
float EnvironmentCalculations::EquivalentSeaLevelPressure
(
  float altitude,
  float temp,
  float pres,
  AltitudeUnit altUnit,
  TempUnit tempUnit
)
{
    float seaPress = NAN;
    if(!isnan(altitude) && !isnan(temp) && !isnan(pres))
    {
        if(tempUnit != TempUnit_Celsius)
            temp = (temp - 32.0) * (5.0 / 9.0); /*conversion to [�C]*/

        if(altUnit != AltitudeUnit_Meters)
            altitude *= 0.3048; /*conversion to meters*/

        seaPress = (pres / pow(1 - ((0.0065 *altitude) / (temp + (0.0065 *altitude) + 273.15)), 5.257));
    }
    return seaPress;
}


/****************************************************************/
float EnvironmentCalculations::DewPoint
(
  float temp,
  float hum,
  TempUnit tempUnit
)
{
  // Equations courtesy of Brian McNoldy from http://andrew.rsmas.miami.edu;
  float dewPoint = NAN;

  if(!isnan(temp) && !isnan(hum))
  {
     if (tempUnit == TempUnit_Celsius)
     {
       dewPoint = 243.04 * (log(hum/100.0) + ((17.625 * temp)/(243.04 + temp)))
       /(17.625 - log(hum/100.0) - ((17.625 * temp)/(243.04 + temp)));
     }
     else
     {
       float ctemp = (temp - 32.0) * 5.0/9.0;

       dewPoint = 243.04 * (log(hum/100.0) + ((17.625 * ctemp)/(243.04 + ctemp)))
       /(17.625 - log(hum/100.0) - ((17.625 * ctemp)/(243.04 + ctemp)));

       dewPoint = dewPoint * 9.0/5.0 + 32.0;
     }
  }

  return dewPoint;
}
