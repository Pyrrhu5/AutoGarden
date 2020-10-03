# AutoGarden

This is an Arduino project.

Blink a LED when the soil is not moister enough using two metal rodes planted into the soil (as moister sensors have a shitty lifespan).

Meant to run on batteries, with the Arduino's power LED removed, it consumes around 1,35 mA.

## COMPONANTS
Schematics totally ripped off from [this article](
http://gardenbot.org/howTo/soilMoisture/#The_local_circuit_-_simple_voltage)

Two metal rodes (like screws, nails) consistently spaced by some non-conductive material (like packaging foam) and partially isolated up to the depth of the soil to measure (up to the point where to measure moisture, not the surface):
- S0
- S1

One resistor : between 47k to 100k Ohms (depends on observed measures)
- R0

One resistor: 100 Ohms
- R1

One resistor: 1 kOhms (lower it if the LED is not bright enough)
- R2

One LED
- L

Arduino Nano

## WIRING
Pin | Componant
--- | ---
A0 | S0 
D2 | R0 to S1
D3 | R1 to S0
D4 | R2 to L+
- |L- to GND

[Or just look at this](http://gardenbot.org/howTo/soilMoisture/soilMoisture_localCircuit_w-flip-flop_big.png)

If everything is well wired, the moisture level should be 0
when the rodes are not in contact with anything, and close to 1023 when in water.


## License
Unmodified [MIT license](https://opensource.org/licenses/MIT)

See `License.md`
