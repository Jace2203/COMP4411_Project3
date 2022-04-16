# COMP4411_Project3

## Required

- [x] Box Intersection
- [x] Phong Specular-Reflection Model
- [x] Directional Lights
- [x] Point Lights
- [x] Shadow Attenuation
- [x] Reflection
- [x] Refraction

## TO-DO List
|BW|Task|Done|
|-|-|-|
|W|adaptive termination|✔|
|W|antialiasing by supersampling|✔|
|+W|jittering|✔|
|B|spot lights|✔|
|B|background image|✔|
|B|accelerate shadow atteuation|✔|
|B|overlapping tranparent objects||
|BW|texture mapping (diffuse and transparent)|✔|
|BW|antialiasing by adaptive supersampling|✔|
|BW|Warn model|✔|
|2B|bump mapping|✔|
|2B|solid/procedural texture|✔|
|2B|new geometry (quadrics)|✔|
|2B|height fields|✔|
|2B|first|✔|
|1.5B|next for each Dof, soft shadows,|✔✔✔|
|    |motion blur, glossy reflection||
|3B|high lever geometry (e.g. metaballs)||
|3B|ray-intersection optimization|✔|
|4B|realistic shading model||
|4B|CSG|✔|
|4B|caustics||

## Documentation

### Requirement:
---
### Box Intersection
load Box.ray file by File > Load Scene.

### Phong Specular-Reflection Model
it is implemented in every scene.  
Adjust the scale of Ambient Light in the UI.

### Distance Attenuation
load box_dist_atten.ray file by File > Load Scene.  
Adjust the scale of Constant, Linear and Quadric coefficient for point light in the UI.

### Shadow Attenuation
load box_cyl_opaque_shadow.ray file by File > Load Scene.

### Reflection
load box_cyl_reflect.ray file by File > Load Scene.  
Adjust the depth value to >= 2 in the UI

### Refraction
load cyl_cyl_refract.ray file by File > Load Scene.  
Adjust the depth value to >= 2 in the UI

### BW:
---
### Adaptive termination
adjust the threshold value bar to set the threshold value

### Antialiasing by Supersampling
Press the `Super Samp` button to enable this feature.  
Use the `Number of sup-pixel` slider to adjust the detail of supersampling.

### Antialiasing by Jittering
Press the `Super Samp` button and `Jitter` button to enable this feature.

### Spot lights
load spotlight.ray file by File > Load Scene.

### Background Image
load any .ray file by File > Load Scene and load background.bmp by File > Load Background image.

### Accelerate shadow attenuation
use BVH

### Texture Mapping
In .ray file, diffues will be set to a file name.  
That file will be located in `texture` folder.  
  
load wall_tex.ray

### Antialiasing by Adaptive Supersampling
Press the `Adapt Supper` button to enable this feature.  
Use the `Number of sup-pixel` slider to adjust the detail of supersampling.

### Warn model
load warnlight.ray file by File > Load Scene.  
Triangle, rectangle and circle are implemented.  
flap_0 and flap_1 are used to restrict the ranges of lit points in the world coordinates.  
The light source is defined similarly as spot light.

### Bump Mapping
load wall_tex.ray and press the `Bump mapping` button

### Solid/Procedural texture
load wall_tex.ray and press the `Generate Noise` button once  
press the `Noise Texture` button to apply the generated texture  
`Noise Input` and `Noise Scale` are parameter that affect the generation of the texture  
if the slider has changed, press `Generate Noise` again to generate a new texture with the given parameters

### New Geometry (quadrics)
load torus.ray to render a torus  
load paraboloid.ray to render a paraboloid  
load paraboloid_cut.ray to render a paraboloid with the bottom cut away

### Height-fields
load blank.ray by File > Load Scene and load 1.bmp by File > Load HField Map.  
1_grey.bmp is automatically loaded as the heights.

### Depth of field
load depthOfField.ray by File > Load Scene.  
Enable DOF and adjust the Focal Length and Aperture Size in the UI.

### Soft Shadows
load softShadow.ray by File > Load Scene.  
Enable Soft Shadow

### Motion Blur
load softShadow.ray by File > Load Scene.  
Enable Motion

### Glossy Reflection
load glossyReflection.ray by File > Load Scene.  
Enable Glossy

### Ray-intersection Optimization
this is done by using Axis-aligned bounding box for bounding volume hierarchy
press `BVH` to enable this feature

### CSG
load file named csg_*.ray  
each operation is demonstrated by each file