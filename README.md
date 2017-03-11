# A 3D dynamic fairy scene, using OpenGL

### Description ###
The project herein presents a scene from a fairytale, focusing on a fairy in her natural environment, in dim lit foggy woods, at twilight. 
She is surrounded by her fairy house, her garden with mushrooms and flowers,
whilst being accompanied by lighting fire flies.
The scene also presents a "magical cube" that transforms the fairy giving her a red glow after she dives in it.

### Running the program ###
In order to open the program, one must clone the repository and open the Microsoft Visual Studio 
executable file, with the ".sln" extension. 
Afterwards, click Ctrl + F5 to run the release version.

### Functionalities: ###
* 'O' key: turns the directional light on/off
* 'P' key: turns the point light on/off (along with the cube which represents it)
* key 1 (directional light)/ 2 (point light) &
 * X/C- move the light on the X axis
 * Y/V- move the light on the Y axis
 * Z/B- move the light on the Z axis
* 'M' key: changes the viewing mode: solid, wireframe, point.
* keys which move the camera:
 * 'W': forward;
 * 'S': backwards;
 * 'A': to the left;
 * 'D': to the right;
* 'E' & 'Q' keys rotate the camera on a circle arround the central point of the scene; this move is for presentation purposes, the camera returning in its previous position after releasing the keys;
* 'Enter' key: starts the presentation demo:
 * the camera will start a presentation rotation around the scene
 * the camera will return to the starting point
 * the fairy will move towards the magical cube
 * once there, the fairy will dive in the cube
 * upon ascending from the cube, the fairy will self-rotate a couple of times
 * when finishing the rotations, the fairy will be surrounded by a red light (generated cube)
 * after holding the posture for a couple of seconds, the fairy will return to the original place

The _mouse_ will allow the user to rotate the camera dynamically around the scene, in order to better observe it.
The point light sources- the fire flies, will gradually fade in and fade out.

### Algorithms used: ###
* Blinn-Phong illumination algorithm
* Lighting Maps for texture coloring the objects
* Shadow Mapping for shadow representation
* Skyboxing
* Fog generation
* Color Blending

### Results: ###

_Fading Lights:_

![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17264873_726937494151001_9222063461118300049_n.jpg?oh=1678e12b2011008de23a6e3a5a93c43f&oe=5965A807)
![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17191439_726937584150992_236697689748883671_n.jpg?oh=6f8eac7f0168f67fbb9eb9118b7a3fc6&oe=592998F7)
![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17202898_726937594150991_7209313727616257028_n.jpg?oh=2d398f109e373e36ffb6e5575011bb6a&oe=59284FC7)

_With Directional Light - clearer shadows:_

![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17264387_726939524150798_5415735021025600985_n.jpg?oh=0f1bd1aaedd435e5fe08a5afcbea0da6&oe=5932C21C)

_Rear view:_

![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17264408_726937484151002_5636214755703326908_n.jpg?oh=e85d0d3a8e6987f722ac793b1252ae84&oe=596ED129)

_Rotation & Transformation:_

![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17202829_726937567484327_8916531692140047486_n.jpg?oh=c2fc12f5e51bc97dfac19b2c9a7f1495&oe=593304D5)
![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17155246_726937577484326_838099798483227909_n.jpg?oh=e8f3f787c398ac9b1a382c6ef11d554c&oe=59264642)
![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/14717149_726937490817668_354807066821206055_n.jpg?oh=5c134c1da65445bc83ef3648632726d1&oe=59724A4E)

_Fog:_

![alt tag](https://scontent.fomr1-1.fna.fbcdn.net/v/t1.0-9/17203165_726937477484336_730422407652615924_n.jpg?oh=fcae6d42fed1844c762c36f8f62ca6c3&oe=5926521F)

