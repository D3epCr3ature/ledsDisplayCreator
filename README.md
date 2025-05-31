
# LEDs Display Creator

## Brief

This tool offers to create a custom LEDs display and to communicate with it, through a TCP socket.

- *How to use it*

  1. Launch an instance of the [**GUI program**](03b-Software/gui)

     - Configure the socket's IP & Port (TODO: yet to implement)

     - OR use the default:

       - IP&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: **127.0.0.1**

       - Port&nbsp;: **5000**

  2. Driver application
  
     You can either:
     
     - Create your application that instanciate a TCP socket to the GUI.
     
     - OR use one of the [**samples written**](03b-Software/cli) in C.



- [**Protocol implemented**](01-Doc/protocol/protocol.md)

- **

## Showcase

### X-Ray option

This option allows the user to see each LED's index. Very useful when drawing a design.

![7Seg: LEDs vs X-Ray](01-Doc/pics/7Seg-Views.png)

### Usage

- Using a design

  ![Sample with 7Seg's design](01-Doc/pics/)

- Creating, Saving & Loading a design

  ![Sample with custom design](01-Doc/pics/)

## Background

After reproducing the star from Ludens by Bring Me The Horizon, I was struck by the issue of not being able to develop animations on it without having the actual hardware.

This is how I came with the idea of creating this tool that allows to create a LEDs display and communicate with it to create animations, before exporting them to the real deal.

![]()

## ...

