## Possible additions
* Rendering Strategies
  * [ ] GPU Path Tracing with GPU denoising
  
* Denoising Strategies
  * [ ] [Mara, McGuire, Bitterli, Jarosz](https://cs.dartmouth.edu/wjarosz/publications/mara17towards.html)
  * [ ] [Spatiotemporal Variance-Guided Filtering](https://cg.ivd.kit.edu/svgf.php)
  
* Pipeline GUI
  * [ ] Pipeline flow graph
  * [ ] Framerate with per-pass details

* Scene
  * [ ] GUI Add / Edit / Disable lights
  * [ ] GUI Edit material properties and BSDF model
  
* Lights
  * [ ] Directional
  * [ ] Environment Map
  * [ ] Default lighting for scenes without emissive geometry

* Materials / BSDF models
  * [ ] Transmissive
  * [ ] Bump Map
  
* Other
  * [ ] Alternative sampling strategies (ex. Halton, Sobol)
  * [ ] Variable rendering resolution (currently locked 512x512)
  * [ ] Benchmarking (some sort of image quality metric. maybe compare to a reference image)
  * [ ] Asynchronous rendering (continuously render framebuffer and not block GUI)
