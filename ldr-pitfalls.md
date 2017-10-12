### Novice pitfalls of working with LDR on RaspberryPi
#### Basics of a LDR sensor
Light Dependent Resistor, commonly known as LDR and used as a light sensor - is a device whose resistance increases with decreased light and the resistance decreases, whenever the light falling on the sensor increases.
<Graph illustrating the same>

### Basic circuit and Equations
Here, we have a LDR whose output is fed to the RaspberryPi.

##### Illustration
<Circuit Diagram>
We want to measure the voltage across LDR. So, if we supply a Vdd of 3.3 volts and measure the voltage across LDR, it would still be 3.3 and not the actual output of LDR. So, to overcome this problem, we add a 10Kohm resistor in series with the LDR. From the basics of electronics, we know that the same current flows through both the components, however the voltages will be divided between the two.
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
As simple as an LDR functioning principle may sound, working with LDR to get accurate values might get tricky and there could be unexpected pitfalls. Below,I have discussed two of such pitfalls:
1. An input voltage of 3.3V directly supplied to the LDR
   If we supply a 3.3 volts directly to the LDR,
   chances are that we might end up supplying a high voltage as an input to the GPIO of RPi.
   causing it to fail. This is because, the maximum voltage that we should input to RPi should not exceed 3 volts. So, to keep the voltage in check , we should add a potentiometer which will restrict the supply voltage to LDR.
   So, our updated circuit will look like the following:
<Circuit Diagram with potentiometer>
2. No Voltage divider circuit
   Now, since we know the working principle of LDR and its relationship with the Voltage and Resistance- as mentioned in table below,
<table>
    we can conclude that to be able to measure the actual output of LDR, we should introduce a voltage divider circuit. With this we can measure the voltage drop across LDR and deduce the resistance and thus the light intensity.To create the voltage divider circuit, a Resistor of say, 10Kohms is applied in series to the LDR.
