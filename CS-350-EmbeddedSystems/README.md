# CS-350: Emerging Systems Architecture and Technologies

For this course, I worked with a Raspberry Pi to develop several embedded systems projects using Python. The two projects I chose for my portfolio are the temperature sensor integration lab and the final thermostat project.

The temperature sensor project used an AHT20 sensor to read temperature and humidity data and display it on an LCD. It also included a button to switch between Celsius and Fahrenheit. The final project built on a lot of the same concepts but added more functionality, including heating and cooling modes, buttons to adjust the set temperature, LED indicators, and serial communication with a server simulator. The idea was to create a working prototype of a thermostat that could monitor temperature and respond to user input.

### What did you do particularly well?

I think I did pretty well with troubleshooting throughout this course. I ran into quite a few problems getting everything connected and working correctly, especially with the LCD and temperature sensor. There were times when the hardware appeared to be working, but the software wasn't getting the results I expected. I got better at checking individual components and figuring out whether the problem was with my code, the wiring, or something else.

### Where could you improve?

I definitely could have managed my time better, especially with the final project. I had everything working at one point, but when I went to record my final demonstration, the LCD suddenly started displaying random characters and eventually stopped displaying anything at all. The buttons and LEDs were still working, but I couldn't get the display working again before the deadline, so I wasn't able to fully complete the final assignment. It was pretty frustrating to have everything fall apart right at the end, especially after spending so much time getting it working in the first place. I should have started earlier and left myself more time to deal with something unexpected like that.

### What tools and resources are you adding to your support network?

I got more experience working with the Raspberry Pi, Linux, Python, GPIO, and I2C communication. I also used SSH and VS Code to work on the Pi remotely, which made development a lot easier than having to work directly on the device. The Raspberry Pi documentation, hardware datasheets, and Python library documentation were useful when I needed to figure out how something was supposed to work or troubleshoot a problem.

### What skills will transfer to other projects or coursework?

I think the biggest thing I'll take away from this course is a better understanding of how hardware and software work together. Before this course, most of my programming experience involved software running on a computer, where I didn't really have to worry about physical connections or whether a sensor was communicating correctly. Working with the Raspberry Pi showed me how many things can go wrong outside of the code itself. The troubleshooting experience and working with different hardware interfaces are things I can see being useful in other projects.

### How did you make your project maintainable, readable, and adaptable?

I used separate functions for different parts of the program and a state machine to manage the thermostat's operating modes. This made it easier to keep track of what the program was doing instead of putting everything into one large block of code. It also made troubleshooting easier because I could focus on a specific part of the program when something wasn't working. The state machine would also make it easier to add more features or change how the thermostat behaves without having to rewrite the entire program.
