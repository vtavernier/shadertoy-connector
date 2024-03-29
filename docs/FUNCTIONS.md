# shadertoy-connector functions

<!-- MDTOC maxdepth:1 firsth1:0 numbering:0 flatten:0 bullets:1 updateOnSave:0 -->

- [Introduction](#introduction)   
- [Initialization](#initialization)   
- [st_compile: GLSL Compilation](#st_compile-glsl-compilation)   
- [st_render: Context rendering](#st_render-context-rendering)   
- [st_set_input: Set input texture](#st_set_input-set-input-texture)   
- [st_set_input_filter: Set input texture filter](#st_set_input_filter-set-input-texture-filter)   
- [st_reset_input: Reset input texture](#st_reset_input-reset-input-texture)   
- [st_reset: Reset context](#st_reset-reset-context)   
- [st_set_renderer: Set target renderer](#st_set_renderer-set-target-renderer)   

<!-- /MDTOC -->

## Introduction

Most functions in this toolkit operate on a *context*. A context defines the
fragment shaders, buffers and inputs that are used to render frames for a given
Shadertoy. It is identified by a string, which either refers to a local context
(ie. a context you have manually provided the sources for) or a shadertoy.com
context (ie. a context which has been/will be loaded from the shadertoy.com
API).

The function [st_compile/CompileShadertoy](#st_compile-glsl-compilation) can be
used to compile GLSL code to create a local context.
[st_set_input/SetShadertoyInput](#st_set_input-set-input-texture) can later be
used to set the inputs of the resulting context buffers.

If you use a Shadertoy id (from shadertoy.com) as a context id, and it is
accessible through the API (access level: *public + API*), it will be loaded
using the shadertoy.com API before rendering.

If the context id does not exist, or if it cannot be loaded from the
shadertoy.com API, an error message will be output to the standard output
(Octave) or as error messages (Mathematica).

## Initialization

### Synopsis

```
(* Mathematica *)
<<Shadertoy`

% Octave
shadertoy_octave();
```

### Description

Initializes the Shadertoy Connector library.

### Arguments

None

### Return value

None

## st_compile: GLSL Compilation

### Synopsis

```
(* Mathematica *)
code = "void mainImage(out vec4 O, in vec2 U){O = U.xyxy;}";
ctxta = CompileShadertoy[code];

codeImage = "void mainImage(out vec4 O, in vec2 U){O = texelFetch(iChannel0, ivec2(U-.5), 0);}";
ctxtb = CompileShadertoy[codeImage, "a" -> code];

% Octave
code = 'void mainImage(out vec4 O, in vec2 U){O = U.xyxy;}';
ctxta = st_compile(code);

code_image = "void mainImage(out vec4 O, in vec2 U){O = texelFetch(iChannel0, ivec2(U-.5), 0);}";
ctxtb = st_compile(code_image, 'a', code);
```

### Description

Compiles one or more fragment shaders as if they were the code for the buffers of
a Shadertoy context. The return value is the identifier for the built rendering
context.

Only the code for the image buffer is required. Other buffer definitions will be
rendered before the image buffer, in the order they were defined in the call to
`st_compile/CompileShadertoy`.

Any GLSL compilation errors will be output to the standard output (Octave) or as
error messages (Mathematica).

### Arguments

* `code`: GLSL fragment shader to compile, with mainImage as its entry point
* *(may occur 0 or more times)* `BufferName -> Source` (Mathematica) or
`BufferName, Source` (Octave): GLSL fragment shader to compile and render
as an extra buffer named `BufferName`. The buffer can be used as an input
using the [st_set_input/SetShadertoyInput](#st_set_input-set-input-texture)
function.

### Additional notes

The final fragment shader which is compiled by OpenGL for rendering is created
from a template, which is itself made of template parts. The additional source
name in the sources may either be a simple buffer name, or a template part
name.  If the additional source name is a template part name, the corresponding
template part will be replaced by the specified sources.

The default parts of the buffer template are as follows:
  * `glsl:header`: Fragment shader header

```glsl
#version 330
```
  * `glsl:defines`: List of pre-processor defines

```glsl
// Generated on the fly depending on its value
// Example:
#define MY_VALUE 10
```
  * `shadertoy:header`: Header for Shadertoy compatibility

```glsl
precision highp float;
precision highp int;
precision highp sampler2D;

// Input texture coordinate
in vec2 vtexCoord;
// Output fragment color
out vec4 fragColor;
```
  * `shadertoy:uniforms`: Uniform variables defined by the render context

```glsl
// Generated on the fly from the definitions in uniform_state.hpp
uniform vec3 iResolution;
uniform vec4 iMouse;
// etc.
```
  * `buffer:inputs`: Sampler uniforms defined by the buffer being compiled

```glsl
// Generated on the fly from the input definitions
uniform sampler2D myTexture;
uniform sampler3D my3DTexture;
```
  * `buffer:sources`: Sources provided by the buffer being compiled

```glsl
// Should define mainImage, as in a Shadertoy
void mainImage(out vec4 O, in vec2 U) { O = vec4(1.); }
```
  * `shadertoy:footer`: Footer for Shadertoy compatibility

```glsl
// GLSL entry point
void main(void) {
    fragColor = vec4(0.,0.,0.,1.);
    mainImage(fragColor, vtexCoord.xy * iResolution.xy);
}
```

These parts may be overriden in order to fully control how the resulting
shaders are built.  Note that the `buffer:*` parts are filled in from the
sources specified in the st_compile method call so they should not be replaced.
Other parts are not mandatory and can be replaced.

### Return value

* `ctxt`: String that identifies the rendering context based on this fragment
shader

## st_render: Context rendering

### Synopsis

```
(* Mathematica *)
img = RenderShadertoy[ctxt, Frame -> Null, Size -> { 640, 360 }, Mouse ->
	Format -> "RGB", { 0, 0, 0, 0 }, FrameTiming -> False];

% Octave
img = st_render(ctxt, -1, 640, 360, 'RGB', [0 0 0 0], false);
```

### Description

Renders a single frame of the given context `ctxt`.

### Arguments

* `ctxt`: String that identifies the context to render
* *(optional)* `Frame` (Mathematica) or 2nd arg (Octave): Number of the frame to
render (`iFrame/iTime` uniforms). Use `Null` (Mathematica) or `-1` (Octave) to
render the next frame following the previous render call.
* *(optional)* `Size` (Mathematica) or 3rd (width) and 4th (height) args
(Octave): Size of the rendering viewport. If `Size` is a single number, a square
viewport will be assumed. Use `Null` (Mathematica) or `-1` (Octave) for the
default size.
* *(optional)* `Format` (Mathematica) or 5th arg (Octave): Format of the
rendering. Can either be `'Luminance'` (one-channel), `'RGB'` (three-channel) or
`'RGBA'` (four-channel). Defines the number of channels in the returned value.
* *(optional)* `Mouse` (Mathematica) or 6th arg (Octave): Value of the `iMouse`
uniform, as a 2 or 4 component vector of floats.
* *(optional)* `FrameTiming` (Mathematica) or 7th arg (Octave): set to `True` to
return a list containing the running time of the shader, queried using
glBeginQuery(GL_TIMESTAMP), and the rendered image. Defaults to `False`
(only return the rendered image).

### Return value

In Octave, calling this function either returns a HxWxD matrix, where H, W are
the requested height and width of the rendering, and D is the number of channels
of the requested format (1, 3 or 4).

In Mathematica, calling this function returns a floating-point image object of
size HxW, with D channels, where D is the number of channels of the requested
format (1, 3 or 4).

In Mathematica, if `FrameTiming` is set to `True`, a list will be returned
instead. The first element will be the runtime of the image buffer fragment
shader invocation, in seconds. The second element will be the rendered image.

## st_set_input: Set input texture

### Synopsis

```
(* Mathematica *)
input1 = ExampleData[{"TestImage", "Lena"}];
SetShadertoyInput[ctxt, "image.0" -> "a", "a.0" -> input1];

% Octave
input1 = imread("lena512.png");
st_set_input(ctxt, "image.0", "a", "a.0", input1);
```

### Description

Sets the textures that will be used to render frames from the context `ctxt`. If
`ctxt` is the identifier of a remote context, this function overrides the inputs
specified on the shadertoy.com website.

An error will be raised if the input identifier is invalid, either because it is
malformed, or because the named buffer does not exist.

### Arguments

* `ctxt`: String that identifies the context.
* *(may occur many times)* `InputName -> InputImage` (Mathematica) or
`InputName, InputMatrix` (Octave): Sets the texture to use for the input named
`InputName`. `InputName` has the form `bufferName.channel` where `bufferName` is
one of the buffers defined in the context named `ctxt`, and `channel` is in the
inclusive range 0-3. `InputImage` must be a gray-level, RGB or RGBA image
object. `InputMatrix` must be a HxWxD matrix representing either a gray-level,
RGB or RGBA image.
* *(may occur many times)* `InputName -> BufferName` (Mathematica) or
`InputName, BufferName` (Octave): Sets the texture to use for the input named
`InputName`. `InputName` is defined as above. `BufferName` is the name of one
of the buffers defined in the context `ctxt`.

### Return value

None.

### Additional notes

*libshadertoy* uses GL_FLOAT/GL_RGBA32F textures internally, which means aside
from the conversion of matrix elements to single-precision floating point
numbers, no other changes are made to the inputs before being passed to the
driver. If you want 8-bit textures to be scaled between 0 and 1, this has to
be done manually, before calling `st_set_input/SetShadertoyInput`.

## st_set_input_filter: Set input texture filter

### Synopsis

```
(* Mathematica *)
SetShadertoyInputFilter[ctxt, "image.0" -> "Linear", "a.0" -> "Mipmap"];

% Octave
st_set_input_filter(ctxt, 'image.0', 'Linear', 'a.0', 'Mipmap');
```

### Description

Sets the texture unit filter to be used when rendering frames from the context
`ctxt`. If `ctxt` is the identifier of a remote context, this function overrides
the input filters specified on the shadertoy.com website.

An error will be raised if the input identifier is invalid, either because it is
malformed, or because the named buffer does not exist.

### Arguments

* `ctxt`: String that identifies the context.
* *(may occur many times)* `InputName -> InputFilter` (Mathematica) or
`InputName, InputFilter` (Octave): Sets the texture filter to use for the input
named `InputName`. `InputName` has the form `bufferName.channel` where
`bufferName` is one of the buffers defined in the context named `ctxt`, and
`channel` is in the inclusive range 0-3. `InputImage` must be a string
identifying the filtering mode to use, either `'Nearest'`, `'Linear'`, or
`'Mipmap'`.

### Return value

None.

## st_reset_input: Reset input texture

### Synopsis

```
(* Mathematica *)
ResetShadertoyInput[ctxt, "image.0", "a.0"];

% Octave
st_reset_input(ctxt, 'image.0', 'a.0');
```

### Description

When used with a remote context, resets the input properties (texture and
filter, as set by [st_set_input](#st_set_input-set-input-texture) and
[st_set_input_filter](#st_set_input_filter-set-input-texture-filter)) to the
defaults specified on the shadertoy.com website.

### Arguments

* `ctxt`: String that identifies the remote context.
* *(may occur many times)* `InputName`: Name of the buffer input to reset. See
[st_set_input/SetShadertoyInput](#st_set_input-set-input-texture) for details.

### Return value

None.

## st_reset: Reset context

### Synopsis

```
(* Mathematica *)
ResetShadertoy[ctxt];

% Octave
st_reset(ctxt);
```

### Description

When used with a remote context, clears all cached state. The next call
using this context id will reload the context from shadertoy.com.

### Arguments

* `ctxt`: String that identifies the remote context.

### Return value

None.

## st_set_renderer: Set target renderer

### Synopsis

```
(* Mathematica *)
SetShadertoyRenderer["tcp://server:13710"];
SetShadertoyRenderer["local"];

% Octave
st_set_renderer('tcp://server:13710');
st_set_renderer('local');
```

### Description

Sets the name of the target that should be used to render the created contexts.
The target can either be `local`, which renders using the currently running X
server, or a `[tcp://]hostname[:port]` specification that targets a running
instance of shadertoy_server on the network.

Note that this methods allows switching between renderers repeatedly, however
the context identifiers are per-renderer, which means you may have to call
st_compile multiple times if you try to do distributed rendering.

This feature is experimental and may change at any time.

### Arguments

* `host`: Either `local` to render using the local X server, or
`[tcp://]hostname[:port]` to connect to a TCP socket opened by the
`shadertoy_server` program. Note that the connection will only be
attempted when a rendering command is issued, not when this method
is called.


### Return value

None.

