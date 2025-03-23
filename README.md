# Interactive Computer Graphics Application: Presenting a recreation area that includes tennis courts, a river and cars
## Description 
This project reprezents an interactive computer graphics application realized using OpenGL library. 

The application aims to provide a photorealistic rendering of a 3D scene by implementing various functionalities, including scene illumination using different types of light 
(directional and positional), shadow generation, rain simulation, and fragment discarding.
It allows the user to explore the scene through a first-person perspective and interact with certain objects, such as simulating driving a car.
    ![image](https://github.com/user-attachments/assets/3cad95ef-e631-4afe-8cfa-327b927f95c3)

## Features
### Functionalities
- Scene exploration using "W", "A", "S", "D" keys for moving and the mouse for changing orientation
  - This is done using a "camera" described by its position, its front, right and up directions (representing an orthogonal and the target (the point to which it looks)
- Car driving simulation using "UP", "DOWN", "RIGHT", "LEFT" keys
  - Car movements is implemented almost the same as the camera movement, the only difference is that when pressing "RIGHT" and "LEFT", the car shouldn't move, it should just rotate in that direction
  - ![image](https://github.com/user-attachments/assets/ab0b4697-6b7b-4aa8-9176-f593b27f0ece)

- Rain simulation
  - This is done using multiple instances of a single object representing a raindrop
  - For efficiency, i used glDrawElementsInstanced, so I wouldn't have to tell the GPU multiple times to draw the raindrop and where. This way I only told GPU to draw multiple instances of the raindrop, using different positions (the positions are computed in vertex shader)
  - ![image](https://github.com/user-attachments/assets/a2a50377-a869-4f13-b20b-a5247fbd789a)

- Waves simulation
  - For simulating waves on the river I used a grid of points that change their position in time after function
- Fog simulation
  - ![image](https://github.com/user-attachments/assets/f2ee57b1-c966-41db-96db-bfaf3fc4850b)

- Illumination
  - Used Blinn-Phong model
- Shadows
  - Used Shadow Mapping technique

