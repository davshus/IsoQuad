<h1>NOTE</h1>
This project is currently undergoing a COMPLETE REWRITE because I had no clue what I was doing the first time.<br>
My memory leaked more than a 10 year old faucet in a public bathroom.<br>
Don't do malloc(), kids.<br>
# IsoQuad
A remote control for a quadcopter, written in C++ by David Shustin, 2016
<br>
Works via COM Connections.  Uses <a href="http://playground.arduino.cc/Interfacing/CPPWindows">Arduino's SerialClass.cpp</a> to initialize and communicate over COM Ports.
<br><br>
<h1>Usage</h1>
1. Open cmd and navigate to the folder where the release is located.
2. Enter the command "isoquad <Primary COM Port> [Secondary COM Port]" (<required> [optional]").
3. Follow the on-screen instructions.

To Do:
- [x] Secondary incoming COM connection
- [ ] Unlimited quantity of incoming and outgoing COM connections.
- [ ] Separate function for connecting to Serial Ports
- [ ] Optimization
- [ ] Removing comments
