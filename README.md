# esp8266-voltmeter
Voltmeter sender and remote display

This project started as a way to keep tabs on the battery of a rarely used old car parked in the garage.
There are two parts:
  - The sender measure voltage and responds to http request via the home wifi OR its own AP.  This is read with any wifi capable browser, phone etc.
  - The (optional) receiver that uses a 1" OLED display to show current voltage and then go back to sleep.
  
  Configuration is needed to set the matching wifi networks.
  Options are available to set the Name of the voltmeter being read.
  
