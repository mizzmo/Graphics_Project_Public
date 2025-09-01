<h1>Setup Instructions</h1>h1>

<p>This is a very basic project, intended to only demonstrate different graphical techniques, rather than offer an easily playable scene. You can, however, run the scene with some fiddling on your own machine.</p>

<p>Firstly, you need Visual Studio. You must then import the project .sln file 'Assessment2.sln' as a project.</p>

<p>From here, you must add the include files and lib files to the project. The specific steps to do this are as follows: </p>
<p> 
  VC++:
  Include Directories: ../../include;
  Library Directories: ../../lib/glfw-3.4.bin.WIN64/lib-vc2022;
  
  Linker/Input:
  Additional Dependencies: glfw3.lib;
  
  Add gl3w.c to source files.
</p>

<p>From here, you should be able to launch the scene in <b>Debug Mode<b> using the IDE.</p>

<a href='../Graphics_Project_Public'>Back to README</a>
