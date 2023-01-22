# TalkingFish

Documentation of the project:
----
The idea was to have the billy bass fish integrated in the CO2 monitoring system so that it can trigger an accoustic alarm everytime the air quality gets into a critical state. Since the fish contained other elements like a button, a photodiode and a pieze alement it was also a goal to reuse as much as possible:    
![Fish](images/fish.png)   

Used Parts:
----
Arduino Pro Mini 168 16MHz 5V    
DF Player Mini    
Motor drive module DAYPOWER Motor-9110    
CO2 Sensor: TFA Dostmann AirControl Mini CO2 Meter TFA 31.5006    


Understand the original setup
----

The fish uses two motors to move. One is opening and closing the mouth. The other one is moving the head and the tail.    
![Cables](images/cablesOriginal.jpg)   

----

Motor 1:    
Open mouth: White cable  (+) Red cable    (-)   
Motor 2:    
Move head:  Orange cable (+) Black cable  (-)    
Move back:  Black cable  (+) Orange cable (-)

----
