//
//  BaseProperty.h
//  UIEditor
//
//  Created by Dmitry Belsky on 12.9.14.
//
//

#ifndef __UI_EDITOR_BASE_PROPERTY_H__
#define __UI_EDITOR_BASE_PROPERTY_H__

#include "DAVAEngine.h"

class BaseProperty : public DAVA::BaseObject
{
public:
    enum ePropertyType
    {
        TYPE_NONE,
        TYPE_HEADER,
        TYPE_VARIANT,
        TYPE_ENUM,
        TYPE_FLAGS,
    };

    enum eEditFrags
    {
        EF_NONE = 0,
        EF_CAN_RESET = 1 << 0,
        EF_ADD_REMOVE = 1 << 1,
    };
    
public:
    BaseProperty();
    virtual ~BaseProperty();

    BaseProperty *GetParent() const;
    void AddProperty(BaseProperty *property);
    virtual int GetCount() const;
    virtual BaseProperty *GetProperty(int index) const;
    virtual int GetIndex(BaseProperty *property) const;

    virtual DAVA::String GetName() const = 0;
    virtual ePropertyType GetType() const = 0;
    virtual eEditFrags GetEditFlag() const { return EF_NONE; };

    virtual bool CanRemove() const {return false; }
    virtual bool CanCreate() const {return false; }

    virtual DAVA::VariantType GetValue() const;
    virtual void SetValue(const DAVA::VariantType &newValue);
    virtual const EnumMap *GetEnumMap() const;
    virtual void ResetValue();
    virtual bool IsReplaced() const;
    
    virtual void PrepareToEdit();

private:
    BaseProperty *parent;
    DAVA::Vector<BaseProperty*> children;
};

#endif // __UI_EDITOR_BASE_PROPERTY_H__
