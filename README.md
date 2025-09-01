<h1 id='head'>Computer Graphics using OpenGL</h1>
<b>Author: Toby Surtees</b>

<h3 id='contents'>Contents:</h3>
<ol>
	<li><a href='#desc'>Scene Description</a></li>
	<li><a href='#demo'>Graphics Demo</a></li>
</ol>

<h3 id='desc'>Description</h3>
<p>
I implemented this scene to act as a unique way to demonstrate several, simple to advanced, computer graphics techniques using different types of objects. This scene utilities a dimly lit setting to better demonstrate shiny surfaces in simulated moon light, while encompassing different glowing objects to showcase interactions between light sources and effect on transparency, which interactive elements to remove said light sources for a different effect.
</p>

<p>
This scene shows procedurally generated desert dunes in a starry night sky. On the dunes there is a 3D pyramid surrounded by a transparent cone of light leading up to a large, spinning UFO. The UFO is producing a glowing green colour, while projecting 4 red spinning spotlights on the ground around the pyramid. Circling the UFO is a jet plane. In the background there are some Egyptian ruins showcasing different transparent layers, aimed to represent stained glass, and sitting on those ruins is a large ancient vase. Looking up at the sky we see procedurally generated, animated shooting stars.
</p>

<p>
<i>The inspiration for this scene comes mainly from Egypt and the pyramids, as well as folklore regarding how the pyramids were created, specifically the theory that they were built by aliens, to put a more interesting spin on a typical desert scene. This also helped to showcase more lighting elements with an interactive UFO.</i>
</p>


<h3 id='demo'>Graphics Demonstration</h3>
<h4>3D Modelling</h4>
<p>The first object is a Cone / Cylinder shape, representing a beam emitting from the UFO. This is generated in cylinder.h using two radius values to determine the size of each end of a cylinder, forming a cone shape. The shape is made of two circles at each end, surrounded by a specified number of strips which make up the sides of the shape. Also implemented is a gradient colour, from green to blue, to represent the travel distance of the beam, as well as a gradient of transparency which decreases from top to bottom.
</p>

<p>
The second object is a plane of desert dunes, generated using a grid of triangles which form together to create a flat plane. I then manipulate this plane using Sine waves to change the height of each triangle, creating the dune effect. I also implemented roughness to make the waves appear more interesting and random.
</p>




























