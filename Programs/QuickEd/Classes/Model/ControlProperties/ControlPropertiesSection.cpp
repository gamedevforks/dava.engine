#include "ControlPropertiesSection.h"

#include "PropertyVisitor.h"
#include "ValueProperty.h"
#include "IntrospectionProperty.h"

#include "UI/UIControl.h"

using namespace DAVA;

ControlPropertiesSection::ControlPropertiesSection(DAVA::UIControl* control_, const ControlPropertiesSection* sourceSection, eCloneType cloneType)
    : SectionProperty(control_->GetClassName())
    , control(SafeRetain(control_))
{
    Reflection controlRef = Reflection::Create(&control);
    Vector<Reflection::Field> fields = controlRef.GetFields();
    
    for (const Reflection::Field &field : fields)
    {
        String name = field.key.Get<String>();
        IntrospectionProperty* sourceProperty = nullptr == sourceSection ? nullptr : sourceSection->FindChildPropertyByName(name);
        IntrospectionProperty* prop = IntrospectionProperty::Create(control, name, field.ref, sourceProperty, cloneType);
        AddProperty(prop);
        SafeRelease(prop);
    }
}

ControlPropertiesSection::~ControlPropertiesSection()
{
    SafeRelease(control);
}

void ControlPropertiesSection::Accept(PropertyVisitor* visitor)
{
    visitor->VisitControlSection(this);
}
