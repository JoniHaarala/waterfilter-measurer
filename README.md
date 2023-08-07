# waterfilter-measurer
<br/><br/>
## Water flow velocity and total volume control for a UV wather filter dev in Arduino.

  This program drives a unipolar water flow sensor model YF-B1.
  At the same time, store the volume data to a SD card using a Datalogger drive.
  This is to prevent loose volume data if light go off.
  
<br/>

## Water-flow measure specs:
  - Model: YF-SB1
  
  - Sensor Type: Hall effect
  
  - Workflow tension: 5V - 18V. (better 12v)
  
  -  Máx functional current: 15mA (5V)
  
  - Outpue type: 5V TTL
  
  - insulation resistance: > 100MΩ
  
  - Working liquid flow: 1 L/m - 30 L/m.
  
  - Max water pressure: 1.75MPa
  
  - Functional temperature: -40ºC a +80ºC.
  
  - Max liquid temperature: <= 120°C
  
  - Output pulse: 477 pulses/L (16 a 357Hz)
  
  - Flow rate pulse: Frecuency (Hz) = 11 * Q ± 5% *flow (L / min)

<br/>

## LIBRARIES
- SPI.h
- SD.h
- LiquidCrystal_I2C.h
- Wire.h
<br/>

  ## Credits
Created 17 May 2023

Author: HAARALA, Jonatan A.
