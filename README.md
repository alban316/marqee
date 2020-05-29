#### Scrolling Marquee

Target platform is Arduino UNO with a MAX7219-driven 8x8 LED Matrix.

The MAX7219 handles the LED display via SPI. I programmed the characters bit-by-bit and wrote the logic for bit-shifting the incoming and outgoing characters (and OR-ing them together) before rendering each frame. I also learned a bit about memory management on the Arduino UNO. Despite being conscious of the scope and duration of my variables, it seemed that iteratively creating and destroying variables in a loop was running the UNO out of memory. So, now I declare my variables once as globals and just re-initialize them with each iteration. Is this best practice?

The Python notebook helped me design the characters and convert them to a format that was useful for my Sketch.
