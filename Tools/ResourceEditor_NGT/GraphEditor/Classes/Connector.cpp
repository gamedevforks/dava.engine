/*==================================================================================
    Copyright (c) 2008, binaryzebra
    All rights reserved.
 
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
 
    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    * Neither the name of the binaryzebra nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.
 
    THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=====================================================================================*/

#include "Connector.h"
#include "Metadata/Connector.mpp"

#include "GraphNode.h"
#include "ConnectionSlot.h"
#include "ConnectionManager.h"

#include <core_reflection/i_definition_manager.hpp>
#include <core_dependency_system/i_interface.hpp>

#include <assert.h>

Connector::Connector()
{
    signalConnections[0] = signalConnections[1] = 0;
}

Connector::~Connector()
{
    ObjectHandleT<GraphNode> outputNode = GetOutputSlot()->GetParentNode();
    outputNode->NodeMoved.disconnect(signalConnections[0]);

    ObjectHandleT<GraphNode> inputNode = GetInputSlot()->GetParentNode();
    inputNode->NodeMoved.disconnect(signalConnections[1]);
}

void Connector::Init(size_t outputSlotID_, size_t intputSlotID_)
{
    outputSlotID = outputSlotID_;
    intputSlotID = intputSlotID_;

    signalConnections[0] = GetOutputSlot()->GetParentNode()->NodeMoved.connect(std::bind(&Connector::Repaint, this));
    signalConnections[1] = GetInputSlot()->GetParentNode()->NodeMoved.connect(std::bind(&Connector::Repaint, this));
}

ObjectHandleT<ConnectionSlot> Connector::GetOutputSlot() const
{
    assert(ConnectionManager::Instance().GetSlot(outputSlotID) != nullptr);
    return ConnectionManager::Instance().GetSlot(outputSlotID);
}

ObjectHandleT<ConnectionSlot> Connector::GetInputSlot() const
{
    assert(ConnectionManager::Instance().GetSlot(intputSlotID) != nullptr);
    return ConnectionManager::Instance().GetSlot(intputSlotID);
}

void Connector::Repaint()
{
    IDefinitionManager* defMng = Context::queryInterface<IDefinitionManager>();
    assert(defMng != nullptr);

    IClassDefinition* definition = defMng->getDefinition<Connector>();
    assert(definition != nullptr);

    definition->bindProperty("outputSlot", this).setValue(0);
}

size_t Connector::GetInputSlotId() const
{
    return intputSlotID;
}

size_t Connector::GetOutputSlotId() const
{
    return outputSlotID;
}

void Connector::DummySetSlotId(size_t const&)
{
    // we don't do anything here, because you can not change slot id in runtime.
    // You need recreate connector with this purpose
    // This method need to notify qml that connector must be repainted.
    // And i need setter for this. NGT will notify qml only if i call setValue through property
}
