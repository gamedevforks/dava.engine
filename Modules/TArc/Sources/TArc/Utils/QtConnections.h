#pragma once

#include "Base/BaseTypes.h"

#include <QMetaObject>
#include <QPointer>
#include <QObject>

namespace DAVA
{
namespace TArc
{
class QtConnections final : public QObject
{
public:
    QtConnections() = default;
    ~QtConnections() = default;

    template <typename Func1, typename Func2>
    void AddConnection(const typename QtPrivate::FunctionPointer<Func1>::Object* sender, Func1 signal, Func2 slot, Qt::ConnectionType connectionType = Qt::AutoConnection)
    {
        QObject::connect(sender, signal, this, slot, connectionType);
    }
};
} // namespace TArc
} // namespace DAVA
