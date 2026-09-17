#pragma once

/*  HardwareKit geometry primitives. Everything is procedural and built once (usually when the GL
    context is created). Convention used by the models: y is "out of the panel" (up for a part lying
    on the panel), x/z span the panel surface.
*/
namespace hwk::geo
{
    using gfx::MeshData;
    using gfx::Vec3;
    using gfx::Mat4;

    inline constexpr float kPi = 3.14159265f;

    /** Axis-aligned rectangle on the x/z plane: centre and half sizes. */
    struct Rect
    {
        float cx, cz, hw, hd;
        constexpr float minX() const noexcept { return cx - hw; }
        constexpr float maxX() const noexcept { return cx + hw; }
        constexpr float minZ() const noexcept { return cz - hd; }
        constexpr float maxZ() const noexcept { return cz + hd; }
        constexpr bool contains (float x, float z) const noexcept { return x >= minX() && x <= maxX() && z >= minZ() && z <= maxZ(); }
    };

    /** (outset from the base outline, y) */
    struct ProfilePoint { float outset, y; };

    /** Sweeps a profile around a rounded rectangle (halfW/halfD = 0 gives a circle of `radius`).
        Each profile segment gets its own normal, so bevels stay crisp. */
    MeshData sweptRoundedRect (float halfW, float halfD, float radius, int cornerSegments,
                               const std::vector<ProfilePoint>& profile, bool capTop);

    /** Regular prism (e.g. a hex nut): the profile swept around a `sides`-gon with flat faces. */
    MeshData sweptPolygon (int sides, float radius, const std::vector<ProfilePoint>& profile, bool capTop);

    /** Flat rectangle at height y with axis-aligned rectangular holes. */
    MeshData plateWithHoles (const Rect& outer, float y, const std::vector<Rect>& holes);

    /** Inner walls of a rectangular cutout (normals pointing inward). */
    MeshData wellWalls (const Rect& hole, float topY, float depth);

    /** Flat quad with uv 0..1 (u along +x, v along +z). */
    MeshData horizontalQuad (const Rect& r, float y);

    MeshData box (Vec3 minCorner, Vec3 maxCorner);
    MeshData flatAnnulus (float innerRadius, float outerRadius, int segments);
    MeshData dome (float radius, float height, int segments, int rings);
    MeshData triangularBlade (float baseHalfWidth, float height, float halfThickness);

    /** Circle-ish outline extruded along y: a pointer plate for chicken-head style knobs.
        The point is at -z (the knob's zero direction). */
    MeshData pointerPlate (float length, float tail, float halfWidth, float y0, float y1);

    /** [-1, 1] x/z quad at y = 0, for shadows, decals and screen-space quads. */
    MeshData unitQuad();
}
