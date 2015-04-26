# y4m.js
JavaScript Y4M decoder based on MJPEG Tools.

Before attempting to build y4m.js target, make sure Emscripten SDK has been installed and 'emcc' is in path.

Follow the next steps to build y4m.js:

1. Run './autogen.sh' to generate the proper Makefile.in file

2. Run './configure' to generate the proper Makefile

3. Run 'make y4m.js' to build y4m.js

Note that you do not need to build the mjpegtools package, the y4m.js build target is separated from the mjpegtools build targets.

A pre-built y4m.js is already under the 'html' directory for demo purpose. Open the index.html for the demo.
