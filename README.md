<h1 id='head'>Computer Graphics using OpenGL</h1>
<b>Author: Toby Surtees</b>
<br>
<p>This Project was designed to be a demonstration of different graphical techniques that I have implemented into a single scene. I wrote this project in C++ and GLSL as an exercise to learn about OpenGL and the different things you can build when using it, as well as for developing a better understanding of how computer graphics work.</p>

<h3 id='contents'>Contents:</h3>
<ol>
	<li><a href='/setup.md'>Setup Instructions</a></li>
	<li><a href='/Graphics_Main/Assessment2/README.txt'>Controls</a></li>
	<li><a href='#desc'>Scene Description</a></li>
	<li><a href='#demo'>Graphics Demo</a></li>
    <li><a href='#3d_modelling'>3D Modelling</a></li>
    <li><a href='#3d_transformation'>3D Object Transformation</a></li>
    <li><a href='#3d_animation'>3D Animation</a></li>
    <li><a href='#3d_modelling'>3D Modelling</a></li>
    <li><a href='#camera'>Camera</a></li>
    <li><a href='#texture'>Texture</a></li>
    <li><a href='#lighting'>Lighting</a></li>
    <li><a href='#shadow'>Shadows</a></li>
    <li><a href='#interact'>Interactivity</a></li>
    <li><a href='#curves'>Curves</a></li>
    <li><a href='#transparency'>Transparency</a></li>
    <li><a href='#research'>Researched Techniques & Sources</a></li>
    <li><a href='/asset_credits.md'>Asset Credits</a></li>
    <li><a href='#credits'>Tool Credits</a></li>

</ol>

<h3 id='desc'>Description</h3>

<p>
I implemented this scene to act as a unique way to demonstrate several simple to advanced computer graphics techniques using different types of objects. I utilised a dimly lit setting to better demonstrate shiny surfaces in simulated moonlight, while showcasing different glowing objects for interactions between light sources and transparent objects. I have also included interactive objects which can alter how the scene is lit and falls in shadow.
</p>

<p>
This scene shows procedurally generated desert dunes in a starry night sky. On the dunes, there is a 3D pyramid surrounded by a transparent cone of light leading up to a large, spinning UFO. The UFO is producing a glowing green colour while projecting 4 red spinning spotlights on the ground around the pyramid. Circling the UFO is a jet plane. In the background, there are some Egyptian ruins showcasing different transparent layers, aimed to represent stained glass, and sitting on those ruins is a large ancient vase. Looking up at the sky, we see procedurally generated, animated shooting stars.
</p>

<p>
<i>The inspiration for this scene comes mainly from Egypt and the pyramids, as well as folklore regarding how the pyramids were created, specifically the theory that they were built by aliens, to put a more interesting spin on a typical desert scene. This also helped to showcase more lighting elements with an interactive UFO.</i>
</p>


<h3 id='demo'>Graphics Demonstration</h3>
<h4 id='3d_modelling'>3D Modelling</h4>
<i>Procedurally Generated:</i>
<p>The first object is a Cone / Cylinder shape, representing a beam emitting from the UFO. This is generated in cylinder.h using two radius values to determine the size of each end of a cylinder, forming a cone shape. The shape is made of two circles at each end, surrounded by a specified number of strips which make up the sides of the shape. Also implemented is a gradient colour, from green to blue, to represent the travel distance of the beam, as well as a gradient of transparency which decreases from top to bottom.
</p>

<p>
The second object is a plane of desert dunes, generated using a grid of triangles which form together to create a flat plane. I then manipulate this plane using Sine waves to change the height of each triangle, creating the dune effect. I also implemented roughness to make the waves appear more interesting and random.
</p>

<div>
<img src='/image_res/modelling1.png'>
<img src='/image_res/modelling2.png'>
</div>

<i>Generated using TinyObjectLoader OBJ Parser:</i>
<p>The first object shown is a Jet Plane rendered using an .OBJ file and .mtl for texture mapping and colours. The parser I have designed supports tangent space calculations, textures and colours as well as automatic translations into vectors of floats rather than triangle objects for use with OpenGL buffers.</p>

<p>Loaded objects are rendered with Shadow and react to lighting, as seen by the green reflections from the positional light in the image with the plane. Objects also support transparency as shown in the image with the UFO. The Ruins object is used to showcase my PBR shader as a matte colour object, while the vase is used to showcase a shiny PBR object.</p>

<div>
<img src='/image_res/modelling3.png'>
<img src='/image_res/modelling4.png'>
<img src='/image_res/modelling5.png'>
<img src='/image_res/modelling6.png'>
<div>


<h4 id='3d_transformation'>3D Object Transformations</h4>
<p>Here we can see the cone (beam) portrayed as leaving the base of the UFO. It has been scaled, translated and rotated in order to properly fit under the UFO and cover the pyramid. When the UFO is clicked, the cone is then transformed in an animation to reduce in scale first in the Y and then X and Z to represent shrinking.</p>

Additionally, The vase has been scaled down to fit on the pedestal in the ruins object, rotated to face straight on the pedestal and translated to the correct position.

<img src='/image_res/modelling1.png'>


<h4 id='3d_animation'>3D Animation</h4>
<p>The plane object has been animated to circle the UFO in a consistent radius by translating it over time. The plane is automatically rotated to bank towards the UFO to simulate a real plane in flight.</p>

<p>The UFO also rotates in place, as do its projected spot-lights.</p>

<img src='/image_res/modelling3.png'>

<h4 id='camera'>Camera</h4>
<p>There are a total of 7 cameras implemented in this scene. <br>
The first is a simple Model-Viewer camera that follows a central point in the scene and can be manipulated using the arrow keys and R / F for zoom. <br>
The second (Pictured) is a first-person fly-through camera that can be controlled using the mouse to change view direction, and WASD for movement. <br>
Next is an animated rotating camera that circles the scene, its speed can be changed with the square bracket keys '[]'. <br>
Finally, are 4 fixed cameras that are set at each corner of the scene.</p>

<img src='/image_res/camera1.png'>

<h4 id='texture'>Texture</h4>
<p>The first image shows the sand dunes with a sand texture. This texture uses an array of mipmaps from 1x1 to 1024x1024 mapped to texture coordinates.</p>

<p>The second image shows a sky-box using cube mapped textures to create a realistic and seamless sky effect. This is achieved by binding 6 separate textures to a cube object and applying depth properties to ensure the cube stays in position in the camera, so it always looks far away.</p>

<p>The third image shows a texture applied to the UFO object, which uses a normal map for raised details, a glow map for enhanced glowing areas such as the green lights, a specular map for shiny areas and a bump map, again for smaller details. This creates the detailed effect you can see in the image.</p>

<div>
<img src='/image_res/texture1.png'>
<img src='/image_res/texture2.png'>
<img src='/image_res/texture3.png'>
</div>

<h4 id='lighting'>Lighting</h4>
<i>Blinn-Phong:</i>
<p>
The first and most obvious is the green positional light, which is positioned directly above the pyramid in the image, casting a green glow. </p>
<p>The second are the red spotlights that circle the pyramid, which cast a red spot on the ground, calculated using a cutoff angle for a sharper shape and a faded colour as distance increases from the source. The red glow can also be seen cast down the side of the pyramid, reflecting off the parallax mapping. </p>
<p>Finally, is a directional light positioned at a distance from the scene which casts a yellow shine over the entire scene, contributing to reflections and overall colour depending on where the light is positioned. </p>

<i>Physically Based Rendering:</i>
<p>The vase object is implemented using Physically Based Rendering (PBR) which more accurately simulates the properties of different materials. The directional light can be seen producing a nice shine on the side of the object, while the positional light contributes to this shine by adding a green tinge. The lights can be seen reflecting off small imperfections on the object, as well as realistically wrapping round the handles. In this screenshot, we can also see a matte PBR object reacting differently to the light. Normal mapping is also used here to improve depth perception.</p>

<div>
<img src='/image_res/lighting1.png'>
<img src='/image_res/lighting2.png'>
</div>

<h4 id='shadow'>Shadow</h4>
<p>By moving the directional light to a low-down position, we can see the shadows cast by the procedurally generated dunes. This is achieved using shadow mapping to determine whether an object is in shadow. This effect is enhanced by using Percentage Closer Filtering (PCF) to soften the edges of the shadows by sampling neighbouring texels and getting an average colour result.</p>

<p>This image shows the shadows projected from the UFO and Plane objects, produced by the directional light. These shadows are projected onto the ground and stretched by the unlevel surface, producing a more realistic effect.</p>

<div>
<img src='/image_res/shadow1.png'>
<img src='/image_res/shadow2.png'>
</div>

<h4 id='interact'>Interactivity</h4>
<p>The interactive object shown is the UFO, light beam (cone) and associated lighting elements (positional and spotlights). By simulating a sphere around a central point, set at the middle of the UFO, I can then check if the user’s mouse is within that radius when a mouse click is registered.</p>

<p>If this is detected, the lighting elements mentioned are removed from the scene by changing the associated uniforms. As well as this, a shrinking animation is played on the light beam, making it disappear into a point. The UFO is then translated away into the distance, while reducing its size to simulate a further distance. Size is then reduced to 0 to remove it from the scene.</p>

<p>We can see the difference without the extra lighting in the scene and observe differences in colour contribution at each object.</p>

<p>The scene can be reset by pressing E, returning the ship, beam and lighting to the scene.</p>

<div>
<img src='/image_res/interact1.png'>
<img src='/image_res/interact2.png'>
<img src='/image_res/interact3.png'>
</div>

<h4 id='curves'>Curves</h4>
<p>This implementation of Bezier curves is used to simulate shooting stars moving across the sky. Each shooting star's path is generated procedurally using four randomized control points, forming a smooth, curved trajectory. The curve is evaluated into points and rendered as a GL line strip to create streaks in the sky. This is animated by progressively drawing segments of the line, creating the illusion of motion. Once the animation completes, a new path is generated, making each shooting star appear unique and dynamic.</p>

<div>
<img src='/image_res/curves1.png'>
<img src='/image_res/curves2.png'>
<img src='/image_res/curves3.gif'>
</div>

<h4 id='transparency'>Transparency</h4>
<p>The first example of transparency is the three panes in the ruins. Each pane has a different colour, and by looking through the first pane we can see the colours combining as each pane contributes to the final colour. This was achieved by defining three simple squares with alpha values, and disabling depth masking while rendering, so that other objects appear correctly, as well as rendering opaque objects first, then rendering transparent object back-to-front to enable multiple transparent objects.</p>

<p>The second example is the light beam (cone) which uses a gradient of alpha values to change its transparency from top to bottom, simulating strength declining over distance.</p>

<p>Finally, the UFO uses transparent textures so you can see into the cockpit and into the ship through the side windows.</p>

<div>
<img src='/image_res/transparent1.png'>
<img src='/image_res/transparent2.png'>
<img src='/image_res/interact1.png'>
</div>

<h4 id='research'>Researched Techniques & Sources</h4>
<p><h5 id='pcf'>Percentage Closer Filtering:</h5>
<p>
<i>Sources:</i> <br>
<i><a href='https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-11-shadow-map-antialiasing'>https://developer.nvidia.com/gpugems/gpugems/part-ii-lighting-and-shadows/chapter-11-shadow-map-antialiasing</a></i>
<br><i><a href='https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping'>https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping</a></i>

PCF improves shadow mapping by sampling nearby texels and forming an average value for softer, more realistic looking edges.
</p>

<img src='/image_res/pcf.png'>

<h5 id='pbr'>Physically Based Rendering:</h5>
<p>
<i>Sources:</i><br>
<i><a href='https://learnopengl.com/PBR/Theory'>https://learnopengl.com/PBR/Theory</i>
<br><i><a href='https://learnopengl.com/PBR/Lighting'>https://learnopengl.com/PBR/Lighting
</i>

PBR aims to replicate how light interacts with real-world surfaces by using physical properties like albedo, metallics, roughness, and ambient occlusion.

I implemented a Cook-Torrance BRDF model to simulate surface reflectance, simulating roughness, shadowing and specular highlights.

Material appearance is influenced by metallicity, roughness, and ambient occlusion, allowing for realistic variation between shiny metals and rough materials. I also used HDR tone mapping and gamma correction for more control over the scene.
</p>

<img src='/image_res/pbr.png'>

<h5 id='pom'>Parallax Occlusion Mapping:</h5>
<p>
<i>Sources:</i><br>
<i><a href='https://learnopengl.com/Advanced-Lighting/Parallax-Mapping'>https://learnopengl.com/Advanced-Lighting/Parallax-Mapping</a></i>
<br><i><a href='https://rdmilligan.wordpress.com/2018/03/09/parallax-mapping-with-opengl-and-c/'>https://rdmilligan.wordpress.com/2018/03/09/parallax-mapping-with-opengl-and-c/</a></i>

POM is used to simulate depth and surface detail by offsetting texture coordinates based on a height map and the viewer's perspective, adding visual complexity without increasing mesh detail.

I used ray marching and a height map in tangent space to find where the view ray intersects the virtual surface layers. Once the intersection is found, I perform linear interpolation between the last two steps for smoother results. These texture coordinates are then used to sample a normal map, which is transformed back into world space for realistic lighting interaction.
</p>

<img src='/image_res/pom.png'>

<h5 id='bfl'>Blinn-Phong Lighting:</h5>
<p>
<i>Source:</i><br>
<i><a href = 'https://learnopengl.com/Advanced-Lighting/Advanced-Lighting'>https://learnopengl.com/Advanced-Lighting/Advanced-Lighting</a></i>

Blinn-Phong improves on Phong lighting by computing specular highlights using a halfway vector between the light and view directions. This results in more realistic looking lighting with better specular highlights and shininess.
</p>

<div>
<img src='/image_res/interact1.png'>
<img src='/image_res/texture3.png'>
</div>

<h4 id='credits'>Credits</h4>
<ul>
<li>TinyObjectLoader: <a href = 'https://github.com/tinyobjloader/tinyobjloader'>https://github.com/tinyobjloader/tinyobjloader</a></li>
</ul>
