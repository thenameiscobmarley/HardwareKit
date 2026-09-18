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
            case KnobStyle::skirted:     return "Skirted";
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
                // Flange, a shallow undercut where it meets the panel, the ridged flank, then a
                // chamfer and a slightly dished top - five changes of direction, so the light
                // breaks over it the way it does on a moulded knob.
                Part body { sweptRoundedRect (0, 0, r, 14,
                                              { { flange - r, 0.0f }, { flange - r, top * 0.10f },
                                                { flange - r - 0.004f, top * 0.13f }, { 0.0f, top * 0.19f },
                                                { -0.003f, top - 0.020f }, { -0.010f, top - 0.005f },
                                                { -0.022f, top }, { -0.030f, top - 0.004f } }, true),
                            Role::body, true, { 0.030f, 0.030f, 0.033f }, 28.0f, top - 0.024f };
                m.parts.push_back (std::move (body));
                m.parts.push_back ({ pointerLine (r, top - 0.002f, 0.0065f, 0.030f), Role::pointer, true, { 0.92f, 0.93f, 0.95f } });
                m.height = top;
                break;
            }

            case KnobStyle::fluted:
            {
                const float skirtR = r * 1.32f, top = r * 1.20f;
                m.footprintRadius = skirtR;
                m.parts.push_back ({ sweptRoundedRect (0, 0, skirtR, 12,
                                                       { { 0.0f, 0.0f }, { 0.0f, 0.010f }, { -0.003f, 0.014f },
                                                         { -0.010f, 0.022f }, { -skirtR + r + 0.004f, 0.026f },
                                                         { -skirtR + r, 0.030f } }, true),
                                     Role::body, true, { 0.025f, 0.025f, 0.028f } });
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 12,
                                                       { { 0.0f, 0.026f }, { -0.004f, 0.040f }, { -0.006f, top - 0.026f },
                                                         { -0.013f, top - 0.008f }, { -0.026f, top }, { -0.034f, top - 0.005f } }, true),
                                     Role::body, true, { 0.030f, 0.030f, 0.033f }, 16.0f, top - 0.030f });
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
                Part cap { sweptRoundedRect (0, 0, r, 14,
                                             { { 0.0f, 0.0f }, { 0.0f, 0.006f }, { -0.002f, 0.010f },
                                               { -0.002f, top - 0.016f }, { -0.008f, top - 0.004f },
                                               { -0.018f, top }, { -0.026f, top - 0.003f } }, true),
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
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 12,
                                                       { { 0.005f, 0.0f }, { 0.004f, 0.010f }, { 0.0f, top * 0.78f },
                                                         { -0.008f, top * 0.92f }, { -0.026f, top } }, true),
                                     Role::body, true, { 0.23f, 0.23f, 0.25f }, 36.0f, top * 0.76f });
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
            case KnobStyle::skirted:
            {
                // Machined skirt at the base, black cone above it, white line down the flank -
                // the knob you find on an outboard compressor.
                const float skirtR = r * 1.26f, top = r * 1.05f;
                m.footprintRadius = skirtR;

                // Machined skirt: a knurled flank, a step, and a polished chamfer on top
                Part skirt { sweptRoundedRect (0, 0, skirtR, 14,
                                               { { 0.0f, 0.0f }, { 0.0f, 0.014f }, { -0.002f, 0.018f },
                                                 { -0.002f, 0.024f }, { -0.008f, 0.030f } }, true),
                             Role::metal, true, { 0.74f, 0.74f, 0.77f } };
                skirt.polish = 0.42f;
                skirt.brushedRings = true;
                m.parts.push_back (std::move (skirt));

                // Body: tapers inward toward the top, with fine grip ridges on the flank
                m.parts.push_back ({ sweptRoundedRect (0, 0, r, 14,
                                                       { { 0.0f, 0.028f }, { -0.003f, 0.040f }, { -0.006f, 0.052f },
                                                         { -r * 0.30f, top - 0.012f }, { -r * 0.38f, top - 0.002f },
                                                         { -r * 0.48f, top - 0.008f } }, true),
                                     Role::body, true, { 0.035f, 0.035f, 0.038f }, 44.0f, top - 0.02f });

                // Pointer: a line down the sloping flank, reaching the skirt
                m.parts.push_back ({ geo::box ({ -0.0055f, 0.024f, -(skirtR - 0.004f) }, { 0.0055f, 0.030f, -(r - 0.012f) }),
                                     Role::pointer, true, { 0.96f, 0.96f, 0.98f } });
                m.parts.push_back ({ pointerLine (r * 0.78f, top - 0.004f, 0.0055f, r * 0.10f),
                                     Role::pointer, true, { 0.96f, 0.96f, 0.98f } });
                m.shadowRadius = skirtR;
                m.height = top;
                break;
            }
        }

        return m;
    }

    //==============================================================================
    Model vuMeter (float halfW, float halfH, float depth, Vec3 bezelColour)
    {
        Model m;
        m.footprintRadius = std::max (halfW, halfH);
        m.height = 0.0f;

        const geo::Rect window { 0.0f, 0.0f, halfW, halfH };
        const geo::Rect inner { 0.0f, 0.0f, halfW - 0.012f, halfH - 0.010f };

        // The case behind the face, and the walls of the recess
        m.parts.push_back ({ geo::wellWalls (window, 0.0f, depth), Role::body, false, { 0.05f, 0.05f, 0.055f } });

        // Printed face, recessed, uv 0..1 across the window
        m.parts.push_back ({ geo::horizontalQuad (inner, -depth), Role::screen, false, { 0.92f, 0.89f, 0.80f } });

        /*  Needle: hinged below the window so only its tip sweeps the dial. Built from the
            hinge outward along -z, tapering, then moved down to the hinge. The renderer turns
            it about that hinge (Model::pivotOffset). */
        const float pivotDrop = halfH * vuPivotDrop;
        const float reach = halfH * vuNeedleTip;
        MeshData needle;
        needle.append (geo::box ({ -0.0075f, -depth + 0.004f, -reach * 0.62f }, { 0.0075f, -depth + 0.009f, 0.012f }));
        needle.append (geo::box ({ -0.0040f, -depth + 0.004f, -reach }, { 0.0040f, -depth + 0.009f, -reach * 0.62f }));
        auto moved = MeshData();
        moved.append (needle, gfx::Mat4::translation ({ 0.0f, 0.0f, pivotDrop }));
        m.parts.push_back ({ std::move (moved), Role::pointer, true, { 0.10f, 0.10f, 0.12f } });
        m.pivotOffset = pivotDrop;

        // Pivot hub, sitting on the face at the bottom of the dial
        MeshData hub;
        hub.append (sweptRoundedRect (0, 0, halfH * 0.20f, 10,
                                      { { 0.0f, -depth + 0.004f }, { 0.0f, -depth + 0.012f }, { -halfH * 0.07f, -depth + 0.018f } }, true),
                    gfx::Mat4::translation ({ 0.0f, 0.0f, pivotDrop }));
        m.parts.push_back ({ std::move (hub), Role::body, false, { 0.13f, 0.13f, 0.15f } });

        // Bezel: a metal frame standing slightly proud of the panel
        constexpr float rc = 0.016f;
        MeshData bezel;
        bezel.append (sweptRoundedRect (halfW + 0.026f - rc, halfH + 0.024f - rc, rc, 4,
                                        { { 0.030f, 0.0f }, { 0.030f, 0.011f }, { 0.018f, 0.020f },
                                          { 0.004f, 0.020f }, { 0.0f, 0.008f }, { 0.0f, 0.0f } }, false));
        Part frame { std::move (bezel), Role::metal, false, bezelColour };
        frame.polish = 0.55f;
        m.parts.push_back (std::move (frame));

        // Four screws holding the bezel to the panel
        MeshData screws;
        const auto head = sweptRoundedRect (0, 0, 0.019f, 8, { { 0.0f, 0.020f }, { 0.0f, 0.026f }, { -0.008f, 0.030f } }, true);
        for (float sx : { -1.0f, 1.0f })
            for (float sz : { -1.0f, 1.0f })
                screws.append (head, gfx::Mat4::translation ({ sx * (halfW + 0.010f), 0.0f, sz * (halfH + 0.008f) }));
        Part screwPart { std::move (screws), Role::metal, false, { 0.70f, 0.70f, 0.73f } };
        screwPart.polish = 0.45f;
        m.parts.push_back (std::move (screwPart));

        // Cover glass, just inside the bezel
        m.parts.push_back ({ geo::horizontalQuad ({ 0.0f, 0.0f, halfW + 0.004f, halfH + 0.004f }, 0.012f),
                             Role::glass, false, { 0.60f, 0.64f, 0.70f } });
        return m;
    }

    //==============================================================================
    Model pushButton (float halfW, float halfD)
    {
        Model m;
        constexpr float rc = 0.016f, r = 0.010f;
        // Collar: a bezel standing off the panel with a chamfer into the well the cap sits in
        m.parts.push_back ({ sweptRoundedRect (halfW + 0.016f - rc, halfD + 0.016f - rc, rc, 6,
                                               { { 0.0f, 0.0f }, { 0.0f, 0.009f }, { -0.004f, 0.014f },
                                                 { -0.010f, 0.014f }, { -0.012f, 0.008f }, { -0.012f, 0.002f } }, false),
                             Role::body, false, { 0.018f, 0.018f, 0.020f } });

        // Cap: square-ish, chamfered all round, with a dished top that catches the light
        m.parts.push_back ({ sweptRoundedRect (halfW - r, halfD - r, r, 6,
                                               { { 0.0f, 0.004f }, { 0.0f, 0.040f }, { -0.005f, 0.050f },
                                                 { -0.014f, 0.054f }, { -0.022f, 0.051f } }, true),
                             Role::accent, true, { 0.40f, 0.41f, 0.43f } });   // "rotates" = moves with the press
        m.footprintRadius = std::max (halfW, halfD) + 0.014f;
        m.height = 0.054f;
        return m;
    }

    Model batToggleBase()
    {
        Model m;
        // Dress nut: hex, with a chamfer top and bottom like a real switch nut
        Part nut { geo::sweptPolygon (6, 0.042f, { { 0.0f, 0.0f }, { -0.003f, 0.004f }, { -0.003f, 0.018f },
                                                   { -0.007f, 0.024f }, { -0.013f, 0.026f } }, true),
                   Role::metal, false, { 0.80f, 0.79f, 0.83f } };
        nut.polish = 0.5f;
        m.parts.push_back (std::move (nut));

        // Threaded bushing above it, with a washer at its base
        Part washer { sweptRoundedRect (0, 0, 0.038f, 10, { { 0.0f, 0.026f }, { 0.0f, 0.030f }, { -0.004f, 0.032f } }, true),
                      Role::metal, false, { 0.74f, 0.73f, 0.76f } };
        washer.polish = 0.35f;
        m.parts.push_back (std::move (washer));

        Part bushing { sweptRoundedRect (0, 0, 0.025f, 12, { { 0.0f, 0.030f }, { 0.0f, 0.056f },
                                                             { -0.004f, 0.062f }, { -0.010f, 0.064f } }, true),
                       Role::metal, false, { 0.80f, 0.79f, 0.83f } };
        bushing.polish = 0.6f;
        m.parts.push_back (std::move (bushing));
        m.footprintRadius = 0.042f;
        return m;
    }

    Model batToggleLever()
    {
        Model m;
        MeshData lever;
        // Shaft: tapers as it rises, as a bat lever does
        lever.append (sweptRoundedRect (0, 0, 0.011f, 10, { { 0.002f, -0.010f }, { 0.002f, 0.020f },
                                                            { -0.001f, 0.080f }, { -0.004f, 0.150f } }, false));
        // Ball tip: a proper sphere-ish cap rather than a blob
        lever.append (sweptRoundedRect (0, 0, 0.019f, 12, { { -0.019f, 0.142f }, { -0.012f, 0.148f }, { -0.004f, 0.156f },
                                                            { 0.0f, 0.164f }, { -0.004f, 0.172f }, { -0.012f, 0.180f },
                                                            { -0.019f, 0.186f } }, true));
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
        /*  A moulded LED, not a hemisphere: a short cylindrical body with the flange that
            comes out of the mould, then the domed lens on top. Unit radius; scale when drawing. */
        MeshData m;
        m.append (sweptRoundedRect (0, 0, 1.06f, 14, { { 0.0f, 0.0f }, { 0.0f, 0.16f }, { -0.06f, 0.22f } }, false));
        m.append (sweptRoundedRect (0, 0, 1.0f, 14, { { 0.0f, 0.20f }, { 0.0f, 0.40f } }, false));
        m.append (geo::dome (1.0f, 0.62f, 14, 4), gfx::Mat4::translation ({ 0.0f, 0.40f, 0.0f }));
        return m;
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
