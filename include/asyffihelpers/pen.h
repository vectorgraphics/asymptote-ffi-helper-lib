#pragma once
#include <asyffi.h>
#include <optional>
#include <string>

namespace AsyFfiHelpers::Pen
{

/** Creates a new Asy::PenColor instance with specified red, green and blue values */
Asy::PenColor fromRgb(double const& red, double const& green, double const& blue);

/**
 * Struct containing pen creation information.
 * For more information, see <a href=https://asymptote.sourceforge.io/doc/Pens.html>here</a>
 */
struct PenCreationInfo
{
    /** Type of line. For example, whether the line is solid, dashed, dotted, etc. */
    std::optional<Asy::PenLineType> lineType = std::nullopt;

    /** Width of the line */
    double lineWidth = ASY_PEN_DEFAULT_WIDTH;

    /** TODO: Write docs for path */
    IAsyPath const* path = nullptr;

    /** String containing which font to use. If font is nullopt, the pen is created with
     * Asymptote's default font */
    std::optional<std::string> font = std::nullopt;

    /** Font size in TeX points (1pt = 1/72.27 inches) */
    double fontSize = 0.0;

    /** Line skip amount */
    double lineSkip = 0.0;

    /** Which color space (e.g. RGB, CMYK, etc) should the pen be in */
    Asy::PenColorSpace colorSpace = Asy::PenColorSpace::Default;

    /** The color of the pen */
    Asy::PenColor color = {.red = 0.0, .green = 0.0, .blue = 0.0, .grey = 0.0};

    /** Pattern of the pen. If nullopt is given, the created pen will have no pattern.
     * These are postscript patterns, for example "tile, "brick" or "checker" */
    std::optional<std::string> pattern = std::nullopt;

    /** Pen fill rule per postscript */
    Asy::PenFillRule fillRule = Asy::PenFillRule::Default;

    /** Transparency information. If nullopt is given, the pen is opaque */
    std::optional<Asy::PenTransparencyInfo> transparency = std::nullopt;

    /** Base align values per postscript */
    Asy::PenBaseLine baseLine = Asy::PenBaseLine::Default;

    /** Line end caps. These are postscript line caps */
    Asy::PenLineCap lineCap = Asy::PenLineCap::Default;

    /** Line join values. These are postscript join styles */
    Asy::PenLineJoin lineJoin = Asy::PenLineJoin::Default;

    /** Postscript miter limit */
    double miterLimit = 0.0;

    /** Overwrite options. This is useful for preventing labels from overriding one another */
    Asy::PenOverwrites overwriteType = Asy::PenOverwrites::Default;

    /** TODO: Write docs for this */
    IAsyTransform const* transform = nullptr;

    /** Creates a pen with a specified greyscale value */
    PenCreationInfo(double const& greyValue);

    /** Creates a pen with RGB color */
    PenCreationInfo(Asy::PenColor const& color);

    /** Creates a pen with RGB color and specified opacity */
    PenCreationInfo(Asy::PenColor const& color, double const& opacity);
};

} // namespace AsyFfiHelpers::Pen
