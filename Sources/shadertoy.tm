void st_render P(( ));

:Begin:
:Function:       st_render
:Pattern:        RenderShadertoy[id_String, OptionsPattern[]]
:Arguments:      { id, OptionValue[Frame], OptionValue[Width], OptionValue[Height], OptionValue[Mouse], OptionValue[Format] }
:ArgumentTypes:  { Manual }
:ReturnType:     Manual
:End:

:Evaluate: RenderShadertoy::usage = "RenderShadertoy[id] renders a Shadertoy as an image";
:Evaluate: Options[RenderShadertoy] = { Frame -> Null, Width -> 640, Height -> 360, Mouse -> { 0, 0, 0, 0 }, Format -> "RGB" };

void st_reset P((const char *));

:Begin:
:Function:       st_reset
:Pattern:        ResetShadertoy[id_String]
:Arguments:      { id }
:ArgumentTypes:  { String }
:ReturnType:     Manual
:End:

:Evaluate: ResetShadertoy::usage = "ResetShadertoy[id] resets the rendering context of a Shadertoy";

void st_compile P((const char *));

:Begin:
:Function:      st_compile
:Pattern:       CompileShadertoy[source_String]
:Arguments:     { source }
:ArgumentTypes: { String }
:ReturnType:    Manual
:End:

:Evaluate: CompileShadertoy::usage = "CompileShadertoy[source] compiles source as a Shadertoy and returns its id";

:Evaluate: Shadertoy::glerr = "OpenGL error: `1`";
:Evaluate: Shadertoy::err = "Error: `1`";
