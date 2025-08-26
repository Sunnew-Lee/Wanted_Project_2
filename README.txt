a. Complete name of the student (first name and last name)
- Sunwoo Lee
-----------------------------------------------------------------------------------------------------------------
b. DigiPen Id
- sunwoo.lee
-----------------------------------------------------------------------------------------------------------------
c. How to use parts of your user interface that is NOT specified in the assignment description.
- Use the ImGui to 
	- Model
		- Show BV CheckBox
			- change BV
			- change BV Hierarchy
		- Show Octree CheckBox
			- Show Octree AABBs CheckBox
		- Show BSP tree CheckBox
		- Show vertex/face normals CheckBox.
	- Material
		- change model Ambient, Diffuse, Specular, Emissive
	- Light
		- change individual light Ambient, Diffuse, Specular and light type
		- change lighting scenarios
		- change # of lights to visualize
		- toggle to pause rotation
	- Toggle Show Debug to use depth copy
-----------------------------------------------------------------------------------------------------------------
d. Any assumption that you make on how to use the application that, if violated, might cause 
the application to fail.
- Open the CS300_3Dgraphics.sln on VS2019 and run it.
-----------------------------------------------------------------------------------------------------------------
e. Which part of the assignment has been completed? 
- Scene setup with one of the PowerPlant sections.(Section4)
- Created Adaptive Octree and each level has different color
- Created BSP tree and each leaf node has different color 
- Camera movement(WASD to move cam position, QR to Yaw, Period&Slash to Pitch)
- ImGui features to toggle depth copying
-----------------------------------------------------------------------------------------------------------------
f. Which part of the assignment has NOT been completed (not done, not working, etc.) and 
explanation on why those parts are not completed?
- None
-----------------------------------------------------------------------------------------------------------------
g. Where the relevant source codes (both C++ and shaders) for the assignment are located. 
Specify the file path (folder name), file name, function name (or line number). 
- Header files -> CS300_3Dgraphics\include folder & lib\include & assimp/assimp
- Source files -> CS300_3Dgraphics\src folder
- Shader files -> shaders folder
- OBJ files -> 3Dmodels folder
- Material files -> textures folder
-----------------------------------------------------------------------------------------------------------------
h. Which machine did you test your application on. 
- OS -> Microsoft Windows 11 Pro
- GPU -> NVIDIA GeForce RTX 3060TI/PCI
- OpenGL Driver version -> 4.6.0 NVIDIA 528.02
-----------------------------------------------------------------------------------------------------------------
i. The average number of hours you spent on the assignment, on a weekly basis
- 18h
-----------------------------------------------------------------------------------------------------------------
j. Any other useful information pertaining to the application
- Try building the project with release mode for fast build.
- The color is random, so if the color difference is unsure, try running the project again.
-----------------------------------------------------------------------------------------------------------------