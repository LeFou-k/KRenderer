# KRenderer

A physically-based renderer based on Nori.

My implementation of the educational ray tracer [Nori](https://wjakob.github.io/nori/).

## How to build

Build KRenderer is very simple. Just like other C++ programs, you need cmake to build it.

Take Linux/MacOS as example. Bash scripts are as follows:

```bash
cd path_to_KRenderer
mkdir build && cd build
cmake-gui ..
```

You can visit [here](https://wjakob.github.io/nori/) for details and other platforms.

## How to use

After building the project,  have a look at the folder `./examples`.

In KRenderer, we use a `.xml` file to describe the scene to be rendered. 

```xml
<scene>
    <!-- Independent sample generator, one sample per pixel -->
	<sampler type="independent">
		<integer name="sampleCount" value="1"/>
	</sampler>

    <!-- Render the visible surface normals -->
    <integrator type="normals"/>

    <!-- Load the Stanford bunny (https://graphics.stanford.edu/data/3Dscanrep/) -->
	<mesh type="obj">
		<string name="filename" value="bunny.obj"/>
		<bsdf type="diffuse"/>
	</mesh>

	<!-- Render the scene viewed by a perspective camera -->
	<camera type="perspective">
        <!-- 3D origin, target point, and 'up' vector -->
		<transform name="toWorld">
            <lookat target="-0.0123771, 0.0540913, -0.239922"
                    origin="-0.0315182, 0.284011, 0.7331"
                    up="0.00717446, 0.973206, -0.229822"/>
		</transform>

		<!-- Field of view: 30 degrees -->
		<float name="fov" value="16"/>
		<!-- 768 x 768 pixels -->
		<integer name="width" value="768"/>
		<integer name="height" value="768"/>
	</camera>
</scene>

```

In additional, there is a `bunny.obj` which cooperate with the `bunny.xml`. In xml file, you can define the scene with xml tags like scene, sampler, integrator, mesh, camera, etc.

After building, just use the xml file as argument.

```bas
./path_to_executable/nori examples/bunny.xml
```

## Function

In KRenderer, what I have accomplished are as follows:

1. Kd-Tree intersection acceleration with tbb and openMP
2. Monte Carlo sampling, such as uniform disk sampling, uniform sphere sampling, hemisphere sampling, hemisphere cosine sampling, BeckMann sampling, etc
3. Ambient Occlusion Integrator
4. A recursive version of path tracing called Whitted-style Ray tracing used Russian Roulette and importance sampling.
5. Diffuse material, mirror material, dielectric material and Microfacet material
6. A iterative version of path tracing using MIS(Multiple Importance Sampling)

All of the algorithms above pass the test in nori.

## Gallery

### Ambient Occlusion

![ajax-ao](https://lk-image-bed.oss-cn-beijing.aliyuncs.com/images/AO.png)

### Microfacet

![ajax-rough](https://lk-image-bed.oss-cn-beijing.aliyuncs.com/images/Microfacet.png)

### Cornell box with mirror and dielectric ball

![Cornell box](https://lk-image-bed.oss-cn-beijing.aliyuncs.com/images/cbox_mis.png)

### Veach

![veach_mis](https://lk-image-bed.oss-cn-beijing.aliyuncs.com/images/Veach.png)

### Table

![table_mis](https://lk-image-bed.oss-cn-beijing.aliyuncs.com/images/table_mis.png)

## Furthermore

Despite the features mentioned above, there are also other features to be accomplished.

- BVH acceleration using SAH method
- Add Intel Embree acceleration library
- Hierarchical sample warping and Image Based Lighting
- Specialized light source sampling despite mesh defined area light
- Extend BRDF to complete BSDF
- More awesome sceneries!
