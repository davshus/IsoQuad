#%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%
#NOTE: ALL CURRENT WORK IS BEING DONE ON THE "opengl" BRANCH OR ONE OF ITS SUB-BRANCHES.
#%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%-%
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
<br>
# IsoQuad
A remote control for a quadcopter, written in C++ by David Shustin, 2016
<br>
Works via serial port connections.  Uses <a href="https://github.com/wjwwood/serial">William Woodall's "serial"</a> to initialize and communicate over COM Ports.
<br><br>
# Usage
1. Ensure that you are connected to the remote device(s).
    * If you are using a bluetooth connection, please ensure that your bluetooth connection is able to use a virtual serial connection.
2. Find and note your port name.
1. Open cmd and navigate to the folder where the release is located.
2. Enter the command "isoquad \<Primary COM Port\> [baud rate = 115200]" (\<required\> [optional]").
3. Follow the on-screen instructions.

To Do:
- [x] Secondary incoming COM connection
- [x] Separate function for connecting to Serial Ports
- [x] Optimization
- [x] Removing comments
- [x] Full Rewrite
- [ ] Add GUI
- [ ] Add 3D graphics and data visualizer using some sort of graphics API or engine (OpenGL, Direct3D, Irrlicht, etc.)
    - [ ] Convert all calls to MathFu to GLM(?)
