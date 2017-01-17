#pragma once

#include "TArc/Controls/PropertyPanel/ProxyComponentValue.h"
#include "TArc/Controls/PropertyPanel/PropertyModelExtensions.h"
#include "TArc/Controls/PropertyPanel/DefaultEditorDrawers.h"
#include "TArc/Controls/PropertyPanel/DefaultValueCompositors.h"

#include <QString>

namespace DAVA
{
namespace TArc
{
class TextComponentValue : public ProxyComponentValue<TextEditorDrawer, TextValueCompositor>
{
public:
    TextComponentValue() = default;

protected:
    virtual Any Convert(const DAVA::String& text) const;

    QWidget* AcquireEditorWidget(QWidget* parent, const QStyleOptionViewItem& option) override;
    void ReleaseEditorWidget(QWidget* editor) override;

private:
    String GetText() const;
    void SetText(const DAVA::String& text);

    bool IsReadOnly() const;
    bool IsEnabled() const;

private:
    DAVA_VIRTUAL_REFLECTION(TextComponentValue, ProxyComponentValue<TextEditorDrawer, TextValueCompositor>);
};

class FastNameComponentValue : public TextComponentValue
{
private:
    Any Convert(const DAVA::String& text) const override;

    DAVA_VIRTUAL_REFLECTION(FastNameComponentValue, TextComponentValue);
};
}
}
