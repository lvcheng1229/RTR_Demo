##### 各Demo的源码及纹理可以在Core文件下找到 
##### 运行结果可以在Blog Pic文件下找到
# RTR_Demo  

编译步骤：  
1.下载项目  
2.编译Assimp：  
使用Cmake时源码路径设置为：你的代码目录/RTR_Demo/assimp-4.0.0  
编译路径为(注意编译文件夹为build)：你的代码目录/RTR_Demo/assimp-4.0.0/**build**  
由于此版本的MMD和X3D格式载入存在Bug，请用CMake编译时取消勾选  
ASSIMP_BUILD_MMD_IMPORTER和ASSIMP_BUILD_X3D_IMPORTER  
3.编译整个项目  
源码路径为：你的代码目录/RTR_Demo  
编译路径随意  
将RTR_Demo\assimp-4.0.0\build\code\Debug下的assimp-vc140-mt.dll复制到RTR_Demo\build\Core\Debug

# Demo列表
 - [x] Plane Shadow(Projection Shadow)
 - [x] Shadow Volume
 - [x] basic shadow map
 - [x] Percentage-Closer Filter(PCF)
 - [x] Percentage-Closer Soft Shadow(PCSS)
 - [x] cascaded shaow map(ESM)
 - [x] variance shadow map(VSM)
 - [x] exponential shadow map(ESM)
 - [x] improved exponential shadow map
 - [x] convolution shadow map
 - [x] outline rendering normal edge method
 - [x] outline rendering procedural grometry method + z-bias
 - [x] outline rendering procedural grometry method + triangle fattening
 - [x] outline rendering image based method
 - [x] X-Toon highlight
 - [x] X-Toon backlight
 - [x] X-Toon Aerial Perpective
 - [x] solid "hard" lines used in CAD
 - [x] Hatching (tonal art maps TAMs)
 - [x] Bloom effect blur
 - [x] Bloom effect blur+downsample
 - [x] depth of field(DOF) + scatter as your gather method
 - [x] motion blur + scatter as your gather method
 - [x] basic physically based shading
 - [x] basic physically based shading with texture
 - [x] the normal distribution function(NDF) term of physically based shading
 - [x] the masking and shadowing function term(G) of physically based shading
 - [x] basic diffuse term of physically based shading
 - [x] the diffuse term of image based lighting(IBL)
 - [x] the specular term using split and sum metod for image based lighting(IBL)
 - [x] basic screen space ambient Occulusion(basic SSAO)
 - [x] Horizon-based Ambient Occlusion(HBAO)
 - [x] linearly trnasformed cosines(LTCs)
 - [x] Spherical Harmonics for irradiance environment Mapping
 - [x] basic Screen-Space Reflection(SSR)
 - [x] Clustered Shading + Forward Shading
 
