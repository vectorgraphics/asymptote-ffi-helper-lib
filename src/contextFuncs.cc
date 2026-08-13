#include "asyffihelpers/contextFuncs.h"
#include "asyffihelpers/common.h"

namespace AsyFfiHelpers::Item
{

AsyContextHelper::AsyContextHelper(IAsyContext* ctx) : context(ctx)
{
}
IAsyPen* AsyContextHelper::createNewPen(Pen::PenCreationInfo const& penCreationInfo) const
{
    return context->createNewPen(
        Common::pointerOrNullConst(penCreationInfo.lineType), penCreationInfo.lineWidth,
        penCreationInfo.path,
        penCreationInfo.font.has_value() ? penCreationInfo.font->c_str() : nullptr,
        penCreationInfo.fontSize, penCreationInfo.lineSkip, penCreationInfo.colorSpace,
        penCreationInfo.color,
        penCreationInfo.pattern.has_value() ? penCreationInfo.pattern->c_str() : nullptr,
        penCreationInfo.fillRule, Common::pointerOrNullConst(penCreationInfo.transparency),
        penCreationInfo.baseLine, penCreationInfo.lineCap, penCreationInfo.lineJoin,
        penCreationInfo.miterLimit, penCreationInfo.overwriteType, penCreationInfo.transform
    );
}
std::string AsyContextHelper::createStringFromAsyString(THAsyString asyString) const
{
    size_t const len = context->getStringLength(asyString);
    std::vector<char> buffer(len + 1);
    context->copyString(asyString, buffer.data(), len + 1);
    return {buffer.data()};
}
} // namespace AsyFfiHelpers::Item
