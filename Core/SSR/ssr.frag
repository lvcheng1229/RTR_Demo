//https://lettier.github.io/3d-game-shaders-for-beginners/screen-space-reflection.html
//this blog maybe useful to you

//some uesful tips
//https://blog.csdn.net/hehemingsgc6/article/details/53888902
//using thickness
//binary search : what second pass do

//pass1 positon(view Pos?) normal color masktexture
//pass2 ssr
//pass3 reflect color
//pass4 blur
//pass5 color and blur color mix by roughness

#version 150

uniform mat4 lensProjection;

uniform sampler2D positionTexture;//camera Position
uniform sampler2D normalTexture;
uniform sampler2D maskTexture;

out vec4 fragColor;

float pow2(float a)
{
  return a*a*a*a*a;
}
void main() {
  float maxDistance = 8;
  float resolution  = 0.3;
  int   steps       = 5;
  float thickness   = 0.5;

  //[0,0]~[w,h] to [0,1]
  vec2 texSize  = textureSize(positionTexture, 0).xy;
  vec2 texCoord = gl_FragCoord.xy / texSize;

  vec4 uv = vec4(0.0);

  vec4 positionFrom = texture(positionTexture, texCoord);
  vec4 mask         = texture(maskTexture,     texCoord);

  if (  positionFrom.w <= 0.0
     || mask.r         <= 0.0
     ) { fragColor = uv; return; }

  vec3 unitPositionFrom = normalize(positionFrom.xyz);
  vec3 normal           = normalize(texture(normalTexture, texCoord).xyz);
  vec3 pivot            = normalize(reflect(unitPositionFrom, normal));

  vec4 positionTo = positionFrom;

  vec4 startView = vec4(positionFrom.xyz + (pivot *         0.0), 1.0);
  vec4 endView   = vec4(positionFrom.xyz + (pivot * maxDistance), 1.0);

  vec4 startFrag      = startView;
       //project to screen space
       startFrag      = lensProjection * startFrag;
       //perform the pespective divide
       startFrag.xyz /= startFrag.w;
       //Convert the screen-space XY coordinates to UV coordinates.
       startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       // Convert the UV coordinates to fragment/pixel coordnates.
       startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
       endFrag      = lensProjection * endFrag;
       endFrag.xyz /= endFrag.w;
       endFrag.xy   = endFrag.xy * 0.5 + 0.5;
       endFrag.xy  *= texSize;
  //convert [w,h]to[0,1]
  vec2 frag  = startFrag.xy;
       uv.xy = frag / texSize;

  float deltaX    = endFrag.x - startFrag.x;
  float deltaY    = endFrag.y - startFrag.y;
  //To handle all of the various different ways (vertical, horizontal, diagonal, etc.)
  float useX      = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
  float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
  vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

  float search0 = 0;
  float search1 = 0;

  //we use search1 to interpolation
  //and use search0 to recod the last position
  //current position x = (start x) * (1 - search1) + (end x) * search1;
  //current position y = (start y) * (1 - search1) + (end y) * search1;

  int hit0 = 0;//intersection during the first pass
  int hit1 = 0;//intersection during the second pass

  float viewDistance = startView.z;//panda3D opengl ???
  float depth        = thickness;

  float i = 0;
float aa=0;
  for (i = 0; i < int(delta); ++i) {
    frag      += increment;
    uv.xy      = frag / texSize;
    positionTo = texture(positionTexture, uv.xy);

    //Calculate the percentage or portion of the line the current fragment represents. 
    //If useX is zero, use the Y dimension of the line. If useX is one, use the X dimension of the line
    search1 =
      mix
        ( (frag.y - startFrag.y) / deltaY
        , (frag.x - startFrag.x) / deltaX
        , useX
        );

    search1 = clamp(search1, 0.0, 1.0);

    //this is wrong:viewDistance = mix(startView.y, endView.y, search1);
    //need to perform perspective-correct interpolation https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf
    viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);//z ?? y ??
    depth        = viewDistance - positionTo.z;//z ?? y

    if (depth < 0 && abs(depth) < thickness) {
      hit0 = 1;
      break;
    } else {
      search0 = search1;
    }
  }

  //Set the search1 position to be halfway between the position of the last miss and the position of the last hit
  //search1 = search0 + ((search1 - search0) / 2.0);

  //steps *= hit0;
  //If the reflection ray didn't hit anything in the first pass, skip the second pass
  //which means steps=0

  //binary search
  /*for (i = 0; i < steps; ++i) {
    frag       = mix(startFrag.xy, endFrag.xy, search1);
    uv.xy      = frag / texSize;
    positionTo = texture(positionTexture, uv.xy);

    viewDistance = (startView.z * endView.z) / mix(endView.z, startView.z, search1);
    depth        = viewDistance - positionTo.z;

    if (depth < 0 && abs(depth) < thickness) {
      hit1 = 1;
      search1 = search0 + ((search1 - search0) / 2);//left   
    } else {
      float temp = search1;
      search1 = search1 + ((search1 - search0) / 2);//right
      search0 = temp;
    }
  }*/

  //(dot(-unitPositionFrom, pivot)
  //the reflection ray points in the general direction of the camera
  //gradually fade out the reflection based on how much the reflection vector points to the camera's position

  //depth / thickness
  //Fade out the reflection the further it is from the intersection point you did find

  //length(positionTo - positionFrom)
  //Fade out the reflection based on how far way the reflected point is from the initial starting point
  float visibility =
      hit0
    * positionTo.w
    * ( 1
      - pow2(max
         ( dot(-unitPositionFrom, pivot)
         , 0
         ))
      )
    * ( 1
      - pow2(clamp
          ( depth / thickness
          , 0
          , 1
          ))
      )
    * ( 1
      - pow2(clamp
          (length(positionTo - positionFrom)
            / maxDistance
          , 0
          , 1
          ))
      )
    * (uv.x < 0 || uv.x > 1 ? 0 : 1)
    * (uv.y < 0 || uv.y > 1 ? 0 : 1);

  visibility = clamp(visibility, 0, 1);

  uv.ba = vec2(visibility);

  fragColor = uv;
}