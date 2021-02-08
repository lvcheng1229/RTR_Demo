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
-[x]HBAO
