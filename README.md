
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

## Features

### Basic controls in the drawing area

- **Right click:** Place a LED

- **Shift + Right click:** Remove LED under cursor

  - When 2 LEDs are overlapping under the cursor, the latest placed is removed.

- **Left click:** Grab and move inside the drawing area

### Shortcuts

- **Ctrl+S:** Save current design

- **Ctrl+O:** Open/Load an existing design

- **Ctrl+Z:** Undo last action (TODO: Yet to implement)

- **Ctrl+Q:** Quit application

## Showcase

### X-Ray option

This option allows the user to see each LED's index. Very useful when drawing a design.

![7Seg: LEDs vs X-Ray](01-Doc/pics/7Seg-Views.png)

### Usage

- Using a design

  ![Sample with 7Seg's design](01-Doc/pics/usingDesign.gif)

- Creating, Saving & Loading a design

  ![Sample with custom design](01-Doc/pics/customDesign.gif)

  **Note:** When removing LEDs ("Empty design" or Shift+Right click on LED),
  there might be some remains of them. As seen, just moving it out of 
  the drawing area refreshes it.

## Background

After reproducing the star from Ludens by Bring Me The Horizon, I was struck by the issue of not being able to develop animations on it without having the actual hardware.

This is how I came with the idea of creating this tool that allows to create a LEDs display and communicate with it to create animations, before exporting them to the real deal.

![]()

## ...

