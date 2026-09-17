namespace hwk::models
{
    using geo::ProfilePoint;
    using geo::sweptRoundedRect;
    using gfx::Mat4;

    const char* knobStyleName (KnobStyle s) noexcept
    {
        switch (s)
        {
            case KnobStyle::proXl:       return "Pro XL";
            case KnobStyle::fluted:      return "Fluted";
            case KnobStyle::chickenHead: return "Chicken head";
            case KnobStyle::aluminium:   return "Aluminium";
            case KnobStyle::softTouch:   return "Soft touch";
            case KnobStyle::jewelCap:    return "Jewel cap";
        }
        return "Knob";
    }

    static MeshData pointerLine (float radius, float y, float halfWidth, float inner)
    {
        return geo::box ({ -halfWidth, y - 0.0005f, -(radius - 0.018f) }, { halfWidth, y + 0.003f, -inner });
    }

    Model knob (KnobStyle style, float r, Vec3 accent)
    {
        Model m;
        m.footprintRadius = r * 1.12f;

        switch (style)
        {
            case KnobStyle::proXl:
            {
                const float top = r * 1.18f, flange = r * 1.11f;
                Part body { sweptRoundedRect (0, 0, r, 10,
                                              { { flange - r, 0.0f }, { flange - r, top * 0.12f }, { 0.0f, top * 0.18f },
                                                { -0.004f, top - 0.014f }, { -0.016f, top } }, true),
                            Role::body, true, { 0.030f, 0.030f, 0.033f }, 28.0f, top - 0.018f };
                m.parts.push_back (std::move (body));
                m.parts.push_back ({ pointerLine (r, top, 0.0065f, 0.030f), Role::pointer, true, { 0.92f, 0.93f, 0.95f } });
                m.height = top;
                break;
            }

            case KnobStyle::fluted:
            {
                const float skirtR = r * 1.32f, top = r * 1.20f;
                m.footprintRadius = skirtR;
                m.parts.push_back ({ sweptRoundedRect (0, 0, skirtR, 12, { { 0.0f, 0.0f }, { 0.0f, 0.012f }, { -0.010f, 0.022f }, { -skirtR + r, 0.026f } }, true),
                                     Role::body, true, { 0.025f, 0.025f, 0.028f } });
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 12, { { 0.0f, 0.02f }, { -0.006f, top - 0.022f }, { -0.020f, top } }, true),
                                     Role::body, true, { 0.030f, 0.030f, 0.033f }, 16.0f, top - 0.024f });
                m.parts.push_back ({ pointerLine (r, top, 0.006f, 0.0f), Role::pointer, true, { 0.95f, 0.95f, 0.97f } });
                // pointer continues down the skirt
                m.parts.push_back ({ geo::box ({ -0.005f, 0.021f, -(skirtR - 0.006f) }, { 0.005f, 0.027f, -(r - 0.004f) }), Role::pointer, true, { 0.95f, 0.95f, 0.97f } });
                m.height = top;
                break;
            }

            case KnobStyle::chickenHead:
            {
                const float top = r * 0.95f;
                m.parts.push_back ({ sweptRoundedRect (0, 0, r * 0.95f, 10, { { 0.0f, 0.0f }, { 0.0f, 0.03f }, { -0.012f, 0.045f } }, true),
                                     Role::body, true, { 0.035f, 0.030f, 0.028f } });
                m.parts.push_back ({ geo::pointerPlate (r * 1.55f, r * 0.55f, r * 0.42f, 0.04f, top), Role::body, true, { 0.035f, 0.030f, 0.028f } });
                m.parts.push_back ({ geo::box ({ -0.006f, top - 0.0005f, -(r * 1.45f) }, { 0.006f, top + 0.003f, -r * 0.1f }), Role::pointer, true, { 0.96f, 0.94f, 0.88f } });
                m.footprintRadius = r * 1.6f;
                m.shadowRadius = r * 0.98f;        // the body; the beak casts its own, turning shadow
                m.beakLength = r * 1.50f;
                m.beakHalfWidth = r * 0.42f;
                m.height = top;
                break;
            }

            case KnobStyle::aluminium:
            {
                const float top = r * 1.05f;
                Part cap { sweptRoundedRect (0, 0, r, 12, { { 0.0f, 0.0f }, { 0.0f, top - 0.012f }, { -0.012f, top } }, true),
                           Role::metal, true, { 0.80f, 0.80f, 0.82f } };
                cap.polish = 0.45f;
                cap.brushedRings = true;
                m.parts.push_back (std::move (cap));
                // engraved indicator dot near the edge
                MeshData dot;
                dot.append (sweptRoundedRect (0, 0, r * 0.12f, 4, { { 0.0f, top - 0.0005f }, { 0.0f, top + 0.0025f } }, true),
                            Mat4::translation ({ 0.0f, 0.0f, -r * 0.68f }));
                m.parts.push_back ({ std::move (dot), Role::body, true, { 0.02f, 0.02f, 0.02f } });
                m.height = top;
                break;
            }

            case KnobStyle::softTouch:
            {
                const float top = r * 1.15f;
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 12, { { 0.004f, 0.0f }, { 0.0f, top * 0.8f }, { -0.02f, top } }, true),
                                     Role::body, true, { 0.23f, 0.23f, 0.25f }, 36.0f, top * 0.78f });
                m.parts.push_back ({ sweptRoundedRect (0, 0, r * 0.62f, 10, { { 0.0f, top - 0.004f }, { -0.006f, top + 0.006f } }, true),
                                     Role::accent, true, accent });
                m.parts.push_back ({ geo::box ({ -0.005f, top + 0.006f, -(r * 0.60f) }, { 0.005f, top + 0.0085f, -r * 0.12f }), Role::pointer, true, { 0.97f, 0.97f, 0.99f } });
                m.height = top + 0.006f;
                break;
            }

            case KnobStyle::jewelCap:
            {
                const float top = r * 1.2f;
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 12, { { 0.008f, 0.0f }, { 0.0f, 0.03f }, { 0.0f, top - 0.03f } }, false),
                                     Role::body, true, { 0.03f, 0.03f, 0.034f }, 40.0f, top - 0.03f });
                Part cap { sweptRoundedRect (0, 0, r, 12, { { 0.0f, top - 0.03f }, { 0.0f, top - 0.012f }, { -0.014f, top } }, true),
                           Role::metal, true, { 0.86f, 0.85f, 0.88f } };
                cap.polish = 0.9f;
                m.parts.push_back (std::move (cap));
                m.parts.push_back ({ sweptRoundedRect (0, 0, r * 0.30f, 8, { { 0.0f, top }, { -r * 0.12f, top + 0.018f } }, true),
                                     Role::accent, true, accent });
                m.parts.push_back ({ pointerLine (r, top + 0.001f, 0.005f, r * 0.42f), Role::pointer, true, { 0.97f, 0.97f, 0.99f } });
                m.height = top + 0.018f;
                break;
            }
        }

        return m;
    }

    //==============================================================================
    Model pushButton (float halfW, float halfD)
    {
        Model m;
        constexpr float rc = 0.016f, r = 0.010f;
        m.parts.push_back ({ sweptRoundedRect (halfW + 0.014f - rc, halfD + 0.014f - rc, rc, 3,
                                               { { 0.0f, 0.0f }, { 0.0f, 0.010f }, { -0.006f, 0.014f } }, true),
                             Role::body, false, { 0.018f, 0.018f, 0.020f } });
        m.parts.push_back ({ sweptRoundedRect (halfW - r, halfD - r, r, 3, { { 0.0f, 0.004f }, { 0.0f, 0.046f }, { -0.008f, 0.054f } }, true),
                             Role::accent, true, { 0.40f, 0.41f, 0.43f } });   // "rotates" = moves with the press
        m.footprintRadius = std::max (halfW, halfD) + 0.014f;
        m.height = 0.054f;
        return m;
    }

    Model batToggleBase()
    {
        Model m;
        Part nut { geo::sweptPolygon (6, 0.040f, { { 0.0f, 0.0f }, { 0.0f, 0.020f }, { -0.006f, 0.026f } }, true), Role::metal, false, { 0.80f, 0.79f, 0.83f } };
        m.parts.push_back (std::move (nut));
        Part bushing { sweptRoundedRect (0, 0, 0.024f, 6, { { 0.0f, 0.024f }, { 0.0f, 0.058f }, { -0.005f, 0.062f } }, true), Role::metal, false, { 0.80f, 0.79f, 0.83f } };
        m.parts.push_back (std::move (bushing));
        m.footprintRadius = 0.042f;
        return m;
    }

    Model batToggleLever()
    {
        Model m;
        MeshData lever;
        lever.append (sweptRoundedRect (0, 0, 0.010f, 5, { { 0.002f, -0.010f }, { 0.002f, 0.010f }, { -0.003f, 0.150f } }, false));
        lever.append (sweptRoundedRect (0, 0, 0.018f, 6, { { -0.018f, 0.140f }, { -0.006f, 0.146f }, { 0.0f, 0.158f },
                                                           { -0.006f, 0.172f }, { -0.018f, 0.178f } }, true));
        Part p { std::move (lever), Role::metal, true, { 0.92f, 0.90f, 0.93f } };
        p.polish = 0.9f;
        m.parts.push_back (std::move (p));
        return m;
    }

    Model jewelLamp (float radius)
    {
        Model m;
        Part bezel { sweptRoundedRect (0, 0, radius, 8, { { 0.0f, 0.0f }, { 0.0f, 0.014f }, { -0.010f, 0.026f }, { -0.022f, 0.026f }, { -0.026f, 0.020f } }, false),
                     Role::metal, false, { 0.85f, 0.84f, 0.88f } };
        bezel.polish = 0.75f;
        m.parts.push_back (std::move (bezel));
        const float j = radius * 0.69f;
        m.parts.push_back ({ geo::sweptPolygon (8, j, { { 0.0f, 0.010f }, { 0.0f, 0.030f }, { -j * 0.44f, 0.058f }, { -j * 0.8f, 0.066f } }, true),
                             Role::pointer, false, { 1.3f, 0.10f, 0.05f } });
        m.footprintRadius = radius;
        return m;
    }

    MeshData ledLens()
    {
        return geo::dome (1.0f, 0.55f, 12, 3);
    }

    Model rackScrew (float radius)
    {
        Model m;
        Part head { sweptRoundedRect (0, 0, radius, 3, { { 0.0f, 0.0f }, { 0.0f, 0.012f }, { -0.02f, 0.03f } }, true), Role::metal, false, { 0.75f, 0.74f, 0.78f } };
        head.polish = 0.5f;
        m.parts.push_back (std::move (head));
        MeshData recess;
        const float a = radius * 0.44f, t = radius * 0.09f;
        recess.append (geo::box ({ -a, 0.026f, -t }, { a, 0.0315f, t }));
        recess.append (geo::box ({ -t, 0.026f, -a }, { t, 0.0315f, a }));
        m.parts.push_back ({ std::move (recess), Role::body, false, { 0.02f, 0.02f, 0.025f } });
        m.footprintRadius = radius;
        return m;
    }

    MeshData rackChassis (float halfW, float bottomY, float topY, float frontZ, float depth, float seamY)
    {
        constexpr float r = 0.05f;
        const float halfD = 0.5f * depth;
        std::vector<ProfilePoint> profile { { 0.0f, bottomY } };
        if (seamY > bottomY && seamY < topY - 0.05f)
        {
            profile.push_back ({ 0.0f, seamY - 0.012f });
            profile.push_back ({ -0.010f, seamY - 0.004f });
            profile.push_back ({ -0.010f, seamY + 0.004f });
            profile.push_back ({ 0.0f, seamY + 0.012f });
        }
        profile.push_back ({ 0.0f, topY - 0.03f });
        profile.push_back ({ -0.03f, topY });

        MeshData mesh;
        mesh.append (sweptRoundedRect (halfW - r, halfD - r, r, 3, profile, false), Mat4::translation ({ 0.0f, 0.0f, frontZ - halfD }));
        return mesh;
    }

    MeshData faceplateEdge (float halfW, float halfH, float thickness)
    {
        constexpr float r = 0.022f;
        return sweptRoundedRect (halfW - r, halfH - r, r, 2, { { 0.0f, -thickness }, { 0.0f, -0.016f }, { -0.016f, 0.0f } }, false);
    }
}
