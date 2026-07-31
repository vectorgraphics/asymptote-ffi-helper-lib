#include "asyffihelpers/pen.h"

namespace AsyFfiHelpers::Pen
{

Asy::PenColor fromRgb(double const& red, double const& green, double const& blue)
{
    return {.red = red, .green = green, .blue = blue, .grey = 0.0};
}
PenCreationInfo::PenCreationInfo(double const& greyValue)
    : colorSpace(Asy::PenColorSpace::Grayscale),
      color {.red = 0.0, .green = 0.0, .blue = 0.0, .grey = greyValue}
{
}
PenCreationInfo::PenCreationInfo(Asy::PenColor const& color)
    : colorSpace(Asy::PenColorSpace::Rgb), color(color)
{
}
PenCreationInfo::PenCreationInfo(Asy::PenColor const& color, double const& opacity)
    : colorSpace(Asy::PenColorSpace::Rgb), color(color),
      transparency({.blendType = "", .opacity = opacity})
{
}
} // namespace AsyFfiHelpers::Pen
