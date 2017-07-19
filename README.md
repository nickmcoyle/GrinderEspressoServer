# GrinderEspressoServer

Grinder is an iOS 10 app I made to turn on my espresso machine from my phone and grind my coffee. Mostly I use it to turn on the espresso machine from bed in the morning so I don't have to wait for it to preheat. It also lets me grind coffee from my phone and even specify the amount of coffee I want ground.

Tab 1 of the app accesses a mobile friendly webpage that hosted on an Arduino microcontroller that is connected to the power switch on my espresso machine in my kitchen. Tab 2 connects using Bluetooth to another Arduino that is wired to my coffee grinder. I can specify a weight of ground coffee I would like, and the Arduino will stop the coffee grinder once this weight has been reached. 

To install, Grinder proto.ino goes to an arduino uno with a red bear labs ble shield connected and a load cell with hx711 amp and a 1602 lcd screen. GrinderEspressoServer.ino goes to a Wemos D1 arduino with a esp8266 wifi chip built in and connected to a mini oled screen.

Open the xcode project file and upload it to your apple device running ios 10 or earlier
