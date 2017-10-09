### Novice pitfalls of working with LDR on RaspberryPi
#### Basics of a LDR sensor
Light Dependent Resistor, commonly known as LDR and used as a light sensor - is a device whose resistance increases with decreased light and the resistance decreases, whenever the light falling on the sensor increases.

### Basic circuit and Equations
Here, we have a LDR whose output is fed to the RasperryPi. Below is the circuit diagram.
##### Illustration

LDR and R are connected in series. So, from the basics of electronics, we know that the same current flows though both the components, however the voltages will be divided between the two.
Following equations explain the phenomenon in theory:
##### Equations
```
Vdd = 3.3v
Vdd = Vl + Vr
Vdd = Il*Rl + Il*R
Il = (Vdd )/ (Rl + R)
or
Vl/Rl = (Vdd )/ (Rl + R)
Finally, Vl , the voltage across LDR is equated as
Vl = (Vdd * Rl)/(Rl +R)
```

### Pitfalls to avoid
