#pragma once

/*  HardwareKit materials.

    One shared vertex shader; each material is a GLSL snippet placed inside a common fragment main()
    (lighting terms already computed: N V L H R ndl wrap ndv ndh facing amb fill lightCol envColor()).
    The snippet writes `col` (and `alpha`). Materials are compiled once each, so the GPU never branches
    per fragment - important on integrated graphics.

    Common uniforms: uModel uViewProj uCamPos uLightDir uTime uViewport uVignette
                     uBaseColor uEmissive uParams uParams2 uGlow uTex uTex2
    A material may declare its own extra uniforms (`declarations`).
*/
namespace hwk::shaders
{
    struct Material
    {
        const char* name = "";
        const char* body = "";              // GLSL statements writing col / alpha
        const char* declarations = "";      // extra uniforms / helper functions
        bool postProcess = true;            // highlight roll-off, vignette (uVignette), dither
    };

    extern const char* const vertex;

    /** Full fragment shader source ("#version 150" included). */
    juce::String fragmentSource (const Material&);

    /** Built-in materials. */
    namespace library
    {
        extern const Material powderCoat;     // textured powder-coated steel. uBaseColor
        extern const Material anodisedPanel;  // brushed panel + RGBA decal (r white print, g outlines, b fields). uParams = decal rect
        extern const Material lacquerPanel;   // glossy painted panel + R8 print. uParams = decal rect, uBaseColor = paint
        extern const Material chrome;         // uParams.x polish, uParams.y > 0.5 brushed rings
        extern const Material plastic;        // uParams.x ridge count, uParams.y ridges below local y; uEmissive = hover lift
        extern const Material woodTable;
        extern const Material emissive;       // LEDs / lamps / vents. uEmissive, uGlow * uParams.x
        extern const Material recess;         // cavity walls. uParams.x depth, uGlow
        extern const Material printRing;      // alpha-blended printed ring. uParams = (outer radius, _, blend uTex->uTex2, accent)
        extern const Material softShadow;     // analytic rounded-rect shadow on a unit quad
        extern const Material screenOverlay;  // screen-space quads: uParams = (textured, alpha, round, _)
        extern const Material glowSprite;     // additive halo on a unit quad: uBaseColor * uParams.x, uParams.y falloff
        extern const Material valueArc;       // knob value arc on an annulus: uParams = (from angle, to angle, alpha, track alpha)
        extern const Material brushedFace;
        extern const Material meterFace;
        extern const Material coverGlass;
        extern const Material windowLight;
        extern const Material magnifierLens;  // fisheye loupe: uTex = zoomed view; uParams = (fisheye, chroma, _, alpha), uParams2 = (centre px, radius px, _)
    }
}
