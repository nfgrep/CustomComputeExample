# CustomComputeExample  
This is an example of how to write custom compute-shaders in Unreal Engine 4.27 (there is also a 4.26 branch)
The main goal of this project is to provide an example of how to use the RDG to transfer large amounts of data to the GPU.  
The [Wiki](https://github.com/nfgrep/CustomComputeExample/wiki) of this project contains an explaination of the code.  

The implementation and usage of the shader lies entirely in the plugin of this project.
To demonstrate the use of compute-shaders, the project just colours a texture with the position of the first vertex of a given mesh.  
