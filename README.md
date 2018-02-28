# Triangles from Above
## A Physics Puzzle game combining Tangram with Stack-em-ups

Watch as the triangles fall down from heaven. Control Gravity with one click.
Match the background as closely as possible in this relaxing experimental sandbox game.

*The source code is located at*	**\oxygine-framework\examples\Triangles from Above**


## Premise
Triangles from Above was created for a homework assignment at the technical university of applied science in Dieburg.


## Features
- Real-time Physics using Box2D
- ECS Event Sourcing
- 5 Levels for testing

## Technology
- Oxygine: Cross-Platform 2D C++ Game Engine
- Box2D: A 2D Physics Engine for Games
- Linear Memory usage and Object Pooling
- Safe XML Resource Loading
- Bitmap fonts using BMFont

## Lessons Learned
I learned how valuable it can be to understand the systems working behind a game engine. I had been taking the Unity hierarchy and its component system for granted. Understanding the principles on which those are based really helped me to understand their power.
I am thankful for the opportunity to build something myself and I learned a lot about memory allocation and the limitations of different approaches.

## Plans for the Future
Box2D already uses linear memory to hold an array filled with all bodies and Oxygine already has a built-in object pool used for the actors.
I want to write my own game engine next which abstracts these two behaviors more so that there is only one pool of linear memory for both.
I was unable to place the triangle inside its own .cpp file because of the way smart pointers are declared within Oxygine. I believe there is a workaround for this but I was not able to find it. I will try to fix this in the future.

### Known Bugs
- Levels are not switched automatically
- Triangles sometimes become elastic when landing on a vertex

### References
1. [Understanding custom polygons in Box2D](http://www.emanueleferonato.com/2008/12/19/understanding-custom-polygons-in-box2d/)
2. [Tobias Stein The Entity-Component-System](https://www.gamasutra.com/blogs/TobiasStein/20171122/310172/The_EntityComponentSystem__An_awesome_gamedesign_pattern_in_C_Part_1.php)
3. [Fixtures in Box2D](http://www.iforce2d.net/b2dtut/fixtures)