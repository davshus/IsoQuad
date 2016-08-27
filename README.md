# IsoQuad
A remote control for a quadcopter, written in C++ by David Shustin, 2016
<br>
Works via COM Connections.  Uses <a href="https://github.com/wjwwood/serial">William Woodall's "serial"</a> to initialize and communicate over COM Ports.
<br><br>
<h1>Usage</h1>
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
