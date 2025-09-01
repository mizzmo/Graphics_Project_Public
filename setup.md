
<h1>Setup Instructions</h1>

<p>This is a very basic project, intended to only demonstrate different graphical techniques, rather than offer an easily playable scene. You can, however, run the scene with some fiddling on your own machine.</p>

<p>Firstly, you need Visual Studio. You must then import the project .sln file 'Assessment2.sln' as a project.</p>

<p>From here, you must add the include files and lib files to the project. The specific lines that need to be added, and where to add them are as follows: </p>
<p> 
VC++:<br>
Include Directories: ../../include;<br>
Library Directories: ../../lib/glfw-3.4.bin.WIN64/lib-vc2022;<br>

Linker/Input:<br>
Additional Dependencies: glfw3.lib;<br>

Add gl3w.c to source files.<br>
</p>

<p>From here, you should be able to launch the scene in <b>Debug Mode</b> using the IDE.</p>

