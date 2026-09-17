namespace hwk::shaders
{
    const char* const vertex = R"GLSL(
#version 150
in vec3 aPos;
in vec3 aNormal;
in vec2 aUV;

uniform mat4 uModel;
uniform mat4 uViewProj;

out vec3 vWorld;
out vec3 vNormal;
out vec2 vUV;
out vec3 vLocal;

void main()
{
    vec4 world = uModel * vec4 (aPos, 1.0);
    vWorld  = world.xyz;
    vNormal = mat3 (uModel) * aNormal;
    vUV     = aUV;
    vLocal  = aPos;
    gl_Position = uViewProj * world;
}
)GLSL";

    static const char* const header = R"GLSL(
#version 150
in vec3 vWorld;
in vec3 vNormal;
in vec2 vUV;
in vec3 vLocal;

out vec4 fragColor;

uniform vec3  uCamPos;
uniform vec3  uLightDir;
uniform float uTime;
uniform vec2  uViewport;
uniform float uVignette;

uniform vec3  uBaseColor;
uniform vec3  uEmissive;
uniform vec4  uParams;
uniform vec4  uParams2;
uniform vec3  uGlow;

uniform sampler2D uTex;
uniform sampler2D uTex2;

const vec3 skyCol    = vec3 (0.80, 0.74, 0.80);
const vec3 groundCol = vec3 (0.30, 0.22, 0.23);

vec3 envColor (vec3 r)
{
    float t = clamp (r.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 c = mix (groundCol, skyCol, smoothstep (0.30, 0.75, t));
    c += vec3 (0.95, 0.85, 0.92) * exp (-pow ((r.y - 0.05) * 5.0, 2.0)) * 0.35;
    c += vec3 (1.0, 0.93, 0.97) * pow (max (dot (r, normalize (vec3 (-0.15, 0.30, 1.0))), 0.0), 4.0) * 0.55;
    float wall = smoothstep (0.15, 0.85, r.z);
    float strips = pow (0.5 + 0.5 * cos (r.x * 9.0), 8.0);
    c += vec3 (0.88, 0.80, 0.86) * wall * (0.35 + 0.55 * strips);
    c += vec3 (1.0, 0.95, 0.97) * pow (max (dot (r, normalize (vec3 (-0.45, 0.75, 0.55))), 0.0), 24.0) * 1.1;
    c += vec3 (1.0, 0.82, 0.90) * pow (max (dot (r, normalize (vec3 (0.70, 0.35, 0.60))), 0.0), 40.0) * 0.5;
    return c;
}

float rrectSdf (vec2 p, vec2 halfSize, float r)
{
    vec2 q = abs (p) - halfSize + r;
    return length (max (q, 0.0)) + min (max (q.x, q.y), 0.0) - r;
}

float hash12 (vec2 p)
{
    vec3 p3 = fract (vec3 (p.xyx) * 0.1031);
    p3 += dot (p3, p3.yzx + 33.33);
    return fract ((p3.x + p3.y) * p3.z);
}
)GLSL";

    static const char* const prelude = R"GLSL(
void main()
{
    vec3 N = normalize (vNormal + vec3 (0.0, 1.0e-6, 0.0));
    vec3 V = normalize (uCamPos - vWorld);
    vec3 L = normalize (uLightDir);
    vec3 H = normalize (L + V);

    float ndl  = max (dot (N, L), 0.0);
    float wrap = max ((dot (N, L) + 0.4) / 1.4, 0.0);
    float ndv  = clamp (dot (N, V), 0.0, 1.0);
    float ndh  = max (dot (N, H), 0.0);
    float facing = 1.0 - ndv;

    vec3 lightCol = vec3 (1.0, 0.97, 0.95);
    vec3 amb  = mix (groundCol, skyCol, N.y * 0.5 + 0.5);
    float fill = max (dot (N, normalize (vec3 (0.75, 0.35, 0.9))), 0.0) * 0.22;
    vec3 R = reflect (-V, N);

    vec3 col = vec3 (0.0);
    float alpha = 1.0;
)GLSL";

    static const char* const postProcess = R"GLSL(
    col = col * (1.0 + col / 4.0) / (1.0 + col);
    vec2 sp = gl_FragCoord.xy / uViewport - 0.5;
    col *= 1.0 - uVignette * 0.40 * pow (length (sp) * 1.25, 2.4);
    col += (hash12 (gl_FragCoord.xy) - 0.5) / 255.0;
)GLSL";

    juce::String fragmentSource (const Material& m)
    {
        juce::String s;
        s << header << "\n// ---- " << m.name << " declarations\n" << m.declarations << "\n" << prelude
          << "\n// ---- " << m.name << "\n" << m.body << "\n";
        if (m.postProcess)
            s << postProcess;
        s << "    fragColor = vec4 (col, alpha);\n}\n";
        return s;
    }

    namespace library
    {
        const Material powderCoat { "powderCoat", R"GLSL(
    float grain = hash12 (floor (vWorld.xz * 900.0 + vWorld.y * 700.0)) - 0.5;
    col  = uBaseColor * (amb * 0.55 + wrap * lightCol * 0.70 + fill) * (1.0 + grain * 0.10);
    col += lightCol * (pow (ndh, 28.0) * 0.10 + pow (ndh, 120.0) * 0.18);
    col += envColor (R) * (0.02 + 0.18 * pow (facing, 4.0));
)GLSL" };

        const Material anodisedPanel { "anodisedPanel", R"GLSL(
    vec2 p = vLocal.xz;
    vec4 d = texture (uTex, (p - uParams.xy) / uParams.zw);
    float brush = hash12 (vec2 (floor (p.y * 2600.0), floor (p.x * 40.0))) - 0.5;
    vec3 panel = vec3 (0.085, 0.089, 0.098) * (1.0 + brush * 0.10);
    panel = mix (panel, vec3 (0.118, 0.123, 0.134), d.b * 0.85);
    vec3 albedo = mix (panel, vec3 (0.46, 0.48, 0.52), d.g);
    albedo = mix (albedo, vec3 (0.90, 0.91, 0.92), d.r);
    col  = albedo * (amb * 0.75 + wrap * lightCol * 0.85 + fill);
    float satin = pow (max (dot (R, normalize (vec3 (-0.15, 0.30, 1.0))), 0.0), 18.0);
    col += vec3 (0.85, 0.88, 0.95) * (satin * 0.10 + pow (ndh, 60.0) * 0.20 + pow (ndh, 8.0) * 0.03) * (1.0 - d.r * 0.5);
    col += envColor (R) * (0.015 + 0.20 * pow (facing, 5.0));
)GLSL" };

        const Material lacquerPanel { "lacquerPanel", R"GLSL(
    vec2 p = vLocal.xz;
    float print = texture (uTex, (p - uParams.xy) / uParams.zw).r;
    float flake = hash12 (floor (p * 1400.0)) - 0.5;
    vec3 albedo = mix (uBaseColor * (1.0 + flake * 0.06), vec3 (0.93, 0.92, 0.95), print);
    col  = albedo * (amb * 0.70 + wrap * lightCol * 0.85 + fill);
    float coat = 1.0 - print * 0.4;
    col += lightCol * (pow (ndh, 180.0) * 0.55 + pow (ndh, 24.0) * 0.06) * coat;
    col += envColor (R) * (0.04 + 0.32 * pow (facing, 5.0)) * coat;
    col += vec3 (0.55, 0.40, 0.95) * pow (facing, 3.0) * 0.10 * coat;
)GLSL" };

        const Material chrome { "chrome", R"GLSL(
    float polish = uParams.x;
    float fres = mix (0.55, 1.0, pow (facing, 5.0));
    col  = uBaseColor * (amb * 0.18 + ndl * lightCol * 0.25) * (1.0 - polish * 0.6);
    col += envColor (R) * mix (uBaseColor, vec3 (1.0), fres * 0.5) * mix (0.55, 0.95, polish) * fres;
    col += lightCol * pow (ndh, mix (40.0, 260.0, polish)) * mix (0.6, 2.2, polish);
    col += vec3 (0.80, 0.84, 0.92) * pow (facing, 3.0) * 0.08;
    if (uParams.y > 0.5)
    {
        float rings = length (vLocal.xz) * 700.0;
        col *= 1.0 + 0.07 * sin (rings) * clamp (1.0 - fwidth (rings) * 0.6, 0.0, 1.0);
    }
    col += uEmissive;
)GLSL" };

        const Material plastic { "plastic", R"GLSL(
    float shade = 1.0;
    if (uParams.x > 0.0 && vLocal.y < uParams.y && vLocal.y > 0.02)
        shade = 0.35 + 0.65 * smoothstep (-0.6, 0.6, sin (atan (vLocal.x, vLocal.z) * uParams.x));
    col  = uBaseColor * (amb * 0.60 + wrap * lightCol * 0.80 + fill);
    col += lightCol * (pow (ndh, 50.0) * 0.28 + pow (ndh, 8.0) * 0.05) * shade;
    col += envColor (R) * (0.04 + 0.20 * pow (facing, 4.0)) * shade;
    col += uEmissive;
)GLSL" };

        const Material woodTable { "woodTable", R"GLSL(
    vec2 p = vWorld.xz;
    float grain = sin (p.x * 6.0 + sin (p.y * 1.2 + p.x * 0.35) * 2.4 + sin (p.y * 0.29) * 4.0);
    float fine  = sin (p.x * 58.0 + sin (p.y * 2.9) * 3.0) * 0.5 + 0.5;
    vec3 albedo = mix (vec3 (0.16, 0.095, 0.075), vec3 (0.30, 0.18, 0.13), 0.5 + 0.3 * grain) * (0.90 + 0.10 * fine);
    col  = albedo * (amb * 0.5 + ndl * lightCol * 0.75);
    col += lightCol * pow (ndh, 24.0) * 0.08;
    col *= mix (1.0, 0.15, smoothstep (3.5, 10.0, length (p - vec2 (0.0, 0.2))));
)GLSL" };

        const Material emissive { "emissive", R"GLSL(
    col  = uBaseColor + uEmissive * (0.75 + 0.6 * pow (ndv, 3.0));
    col += uGlow * (1.6 + 0.5 * sin (vLocal.z * 14.0 - uTime * 2.4)) * uParams.x;
    col += vec3 (1.0) * pow (ndh, 60.0) * 0.45;
)GLSL" };

        const Material recess { "recess", R"GLSL(
    float depthT = clamp ((vLocal.y + uParams.x) / uParams.x, 0.0, 1.0);
    col  = uBaseColor * (amb * 0.55 + wrap * 0.45) * (0.30 + 0.70 * depthT);
    col += uGlow * (1.0 - depthT) * 1.5;
)GLSL" };

        const Material printRing { "printRing", R"GLSL(
    vec2 ringUv = vLocal.xz / (2.0 * uParams.x) + 0.5;
    float ink = mix (texture (uTex, ringUv).r, texture (uTex2, ringUv).r, uParams.z);
    vec3 inkCol = mix (vec3 (0.90, 0.91, 0.92), vec3 (1.0, 0.78, 0.30), uParams.w);
    col = inkCol * (amb * 0.75 + wrap * lightCol * 0.85 + fill);
    alpha = ink;
)GLSL" };

        const Material softShadow { "softShadow", R"GLSL(
    vec2 p = vLocal.xz * uParams.xy;
    float sd = rrectSdf (p, uParams.zw, uParams2.x);
    alpha = (1.0 - smoothstep (-uParams2.y, uParams2.y, sd)) * uParams2.z;
    col = vec3 (0.03, 0.0, 0.015);
)GLSL" };

        const Material screenOverlay { "screenOverlay", R"GLSL(
    if (uParams.x > 0.5)
    {
        vec4 t = texture (uTex, vUV);
        col = t.rgb;
        alpha = t.a * uParams.y;
    }
    else
    {
        col = uBaseColor;
        alpha = uParams.y;
        if (uParams.z > 0.5)
        {
            float r = length (vUV - 0.5);
            alpha *= 1.0 - smoothstep (0.36, 0.5, r);
            if (uParams.w > 0.5)          // ring: hollow, for backing something translucent
                alpha *= smoothstep (0.26, 0.42, r);
        }
    }
)GLSL", "", false };

        const Material glowSprite { "glowSprite", R"GLSL(
    float r2 = dot (vLocal.xz, vLocal.xz);
    col = uBaseColor;
    alpha = uParams.x * exp (-r2 * uParams.y) * (1.0 - smoothstep (0.8, 1.0, r2));
)GLSL", "", false };

        const Material valueArc { "valueArc", R"GLSL(
    // Angle clockwise from -z (the knob's zero direction)
    float a = atan (vLocal.x, -vLocal.z);
    float from = min (uParams.x, uParams.y), to = max (uParams.x, uParams.y);
    float aa = fwidth (a) * 1.5;
    float lit = smoothstep (from - aa, from + aa, a) * (1.0 - smoothstep (to - aa, to + aa, a));
    float track = step (-2.36, a) * step (a, 2.36);
    col = uBaseColor * (1.0 + 0.6 * lit);
    alpha = uParams.z * (lit + uParams.w * track * (1.0 - lit));
)GLSL", "", false };

        const Material magnifierLens { "magnifierLens", R"GLSL(
    // Screen-space loupe over a zoomed re-render of the scene. Fisheye: the centre is magnified most,
    // the edge compresses, with a little chromatic fringing like real glass.
    vec2 d = (gl_FragCoord.xy - uParams2.xy) / uParams2.z;
    float r = length (d);
    float k = uParams.x, chroma = uParams.y;
    vec2 dir = r > 1.0e-4 ? d / r : vec2 (0.0);
    float rs = r * ((1.0 - k) + k * r * r);
    vec2 uvG = 0.5 + 0.5 * dir * rs;
    vec2 uvR = 0.5 + 0.5 * dir * rs * (1.0 + chroma * r * r);
    vec2 uvB = 0.5 + 0.5 * dir * rs * (1.0 - chroma * r * r);
    col = vec3 (texture (uTex, uvR).r, texture (uTex, uvG).g, texture (uTex, uvB).b);

    // Glass: darker inner rim, bright bevel ring, specular glint at the top left, faint tint
    float rim = smoothstep (0.80, 1.0, r);
    col *= 1.0 - 0.35 * rim;
    col += vec3 (0.90, 0.92, 1.0) * smoothstep (0.93, 0.975, r) * (1.0 - smoothstep (0.975, 1.0, r)) * 0.55;
    vec2 glintDir = normalize (vec2 (-0.6, 0.8));
    float glint = pow (max (dot (dir, glintDir), 0.0), 6.0) * smoothstep (0.55, 0.9, r) * (1.0 - smoothstep (0.9, 0.99, r));
    col += vec3 (1.0) * glint * 0.35;
    col += vec3 (0.85, 0.92, 1.0) * exp (-pow ((d.x * 0.6 + d.y - 0.55) * 3.0, 2.0)) * 0.06;
    // uParams.z = glass opacity (0 = fully opaque, for callers that do not set it): the panel stays
    // faintly visible through the middle, while the rim and bevel stay solid
    float opacity = uParams.z > 0.0 ? uParams.z : 1.0;
    alpha = uParams.w * mix (opacity, 1.0, smoothstep (0.72, 0.99, r)) * (1.0 - smoothstep (0.985, 1.0, r));
)GLSL", "", false };
    }
}
