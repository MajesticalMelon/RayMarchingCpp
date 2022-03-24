# RayMarchingCpp
Making a 3D Ray Marcher in C++ using SFML and GLSL

Ray Marching CPP is a personal project of mine as I've always been intrigued by 3D rendering in real-time!  

Currently, the user can move the camera through the space only using the keyboard without touching any code.  
Controls:
- W -> Move Forward
- A -> Move Left
- S -> Move Down
- D -> Move Right
- SPACE -> Move Up
- LCTRL -> Move Down
  
- Left Arrow &nbsp;&nbsp;&nbsp; -> Look Left
- Right Arrow &nbsp;-> Look Right 
- Up Arrow &nbsp;&nbsp;&nbsp;&nbsp; -> Look Up
- Down Arrow  -> Look Down  
  
  
If someone wants to add to the scene, they can create a new RMShape object within main.cpp.
Once created, call RMShape.draw(&shader)

The easiest way to create one of the supported objects is to use the static function within RMShape.  
So far the only supported shapes are and the functions to create them are:  
- Sphere &nbsp; -> createSphere()
- Box &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -> createBox()
- Capsule -> createCapsule()

All of these will be colored upon creation and can even take in a transparent color!  
Furthermore, certain boolean operations can be performed on an RMShape.  
These operations and the functions to use them are:
- Union &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -> RMShape.combine(RMShape)
- Intersection -> RMShape.intersect(RMShape)
- Subtract &nbsp;&nbsp;&nbsp;&nbsp;&nbsp; -> RMShape.subtract(RMShape)

<img src="media/MarcherDemo.gif">
