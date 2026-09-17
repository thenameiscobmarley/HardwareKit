#pragma once

/*  HardwareKit model library: ready-made hardware parts built from the geometry primitives.

    A model is a list of parts. Each part carries a *role* telling the renderer which kind of material
    to use and whether it turns with the control. Sizes are in "panel units"; the default knob radius
    matches a small studio knob on a 19" rack face that is 5 units wide.

        Role::body     main plastic / rubber body     (plastic material, `colour`, optional grip ridges)
        Role::metal    chrome / brushed / aluminium   (chrome material, `polish`, optional brushed rings)
        Role::pointer  indicator line / dot / inlay   (emissive, bright)
        Role::accent   coloured cap or insert         (plastic material in the style's accent colour)
*/
namespace hwk::models
{
    using gfx::MeshData;
    using gfx::Vec3;

    enum class Role { body, metal, pointer, accent };

    struct Part
    {
        MeshData mesh;
        Role role = Role::body;
        bool rotates = true;          // turns with the control
        Vec3 colour { 0.03f, 0.03f, 0.033f };
        float ridges = 0.0f;          // grip ridges drawn by the plastic material (0 = smooth)
        float ridgesBelowY = 0.0f;    // ridges only below this height
        float polish = 0.6f;          // metal: 0 satin .. 1 mirror
        bool brushedRings = false;    // metal: concentric machining rings
    };

    struct Model
    {
        std::vector<Part> parts;
        float footprintRadius = 0.12f;   // for picking: covers everything the model can sweep
        float height = 0.13f;

        /** Contact shadow: the round body, plus an optional pointer ("beak") that turns with the
            knob. shadowRadius 0 means "use footprintRadius" - right for knobs that are round. */
        float shadowRadius = 0.0f;
        float beakLength = 0.0f, beakHalfWidth = 0.0f;

        float bodyShadowRadius() const noexcept { return shadowRadius > 0.0f ? shadowRadius : footprintRadius; }
    };

    //==============================================================================
    /** Knob styles. All point at -z when their value is centred (angle 0) and turn about +y. */
    enum class KnobStyle
    {
        proXl,        // black ridged cylinder with a white line (modern rack processor)
        fluted,       // Davies-1900-type: fluted body on a wide skirt, white pointer line
        chickenHead,  // bakelite pointer knob with a white inlay, for selectors
        aluminium,    // machined aluminium cap with knurled flank and an engraved dot
        softTouch,    // grey soft-touch rubber with a coloured cap (console style)
        jewelCap      // black body with a polished metal cap and a coloured jewel centre
    };

    inline constexpr std::array<KnobStyle, 6> allKnobStyles { KnobStyle::proXl, KnobStyle::fluted, KnobStyle::chickenHead,
                                                             KnobStyle::aluminium, KnobStyle::softTouch, KnobStyle::jewelCap };

    const char* knobStyleName (KnobStyle) noexcept;

    /** radius = body radius; accent colours the cap / insert where the style has one. */
    Model knob (KnobStyle, float radius = 0.114f, Vec3 accent = { 0.42f, 0.28f, 0.86f });

    //==============================================================================
    /** Latching square push button: collar (fixed) + cap (moves, does not rotate). */
    Model pushButton (float halfW = 0.060f, float halfD = 0.042f);

    /** Bat toggle: hex nut + bushing (fixed) and a lever part that pivots about x at `pivotY`. */
    Model batToggleBase();
    Model batToggleLever();
    inline constexpr float batTogglePivotY = 0.05f;

    /** Faceted jewel lamp in a chrome bezel. */
    Model jewelLamp (float radius = 0.072f);

    /** Round LED lens (unit radius; scale when drawing). */
    MeshData ledLens();

    /** Rack screw with cross recess (head + recess as separate parts). */
    Model rackScrew (float radius = 0.05f);

    /** Rounded rack chassis body between two heights, front at chassisFrontZ, depth toward -z. */
    MeshData rackChassis (float halfW, float bottomY, float topY, float frontZ, float depth, float seamY = -1.0f);

    /** Faceplate outer bevel (panel-local, face at y = 0, thickness toward -y). */
    MeshData faceplateEdge (float halfW, float halfH, float thickness);
}
