# BTN415 - Term Project, Winter 2018
<h2>About</h2>
A multi-threaded client application that is using a reliable TCP/IP communications link and is able to communicate with a mobile robotic device.
Client application is controlling the robot and receiving its telemetry sensor data.
<h3>Commands available:<h3>
  <h4>Drive: </h4> once the packet is validated, the robot will respond with an ACK packet and then execute the command. The robot can drive left, right, forward or backward, depending on the parameters. Upon finishing t he command the robot returns to the “Waiting” mode and waits for the next command.
  <h4>Sleep: </h4> The robot transitions to “Sleep” mode when it receives a sleep command.  Once the packet is validated, the robot will respond with an ACK packet and then cease all communications with your client application and wait for another request to connect. 

<h2>Learning outcomes</h2>
<p>- Interpretion of an application layer protocol</p>
<p>- Implementation of a protocol definition within a client environment</p>
<p>- Design of a multi-threaded application</p>
<p>- Encapsulation of an application protocol with TCP/IP reliable communications</p>

<h2>Team members:</h2>
<p>1. Inna Zhogova</p>
<p>2. Rachael Scotchmer</p>
<p>3. Paul Liakhov</p>
<p>4. Mahnoor Siddiqui</p>
