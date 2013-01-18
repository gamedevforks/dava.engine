//
//  BaseParticleEditorNode.h
//  ResourceEditorQt
//
//  Created by Yuri Coder on 11/26/12.
//
//

#ifndef __ResourceEditorQt__BaseParticleEditorNode__
#define __ResourceEditorQt__BaseParticleEditorNode__

#include "Scene3D/ParticleEffectNode.h"
#include "Main/ExtraUserData.h"

namespace DAVA {

class ParticleEffectComponent;
// Base Particle Editor node - common for all inner nodes.
class BaseParticleEditorNode : public ExtraUserData
{
public:
    BaseParticleEditorNode(SceneNode* root);
    virtual ~BaseParticleEditorNode();
    
    // Add/remove child node.
    void AddChildNode(BaseParticleEditorNode* childNode);
    void RemoveChildNode(BaseParticleEditorNode* childNode);

    // Access to the root node.
    SceneNode* GetRootNode() const {return rootNode;};

	ParticleEffectComponent* GetParticleEffectComponent() const;
    
	// Node name.
    void SetNodeName(const QString& nodeName) {this->nodeName = nodeName;};
    virtual QString GetName() const {return this->nodeName;};

    // Mark for selection logic.
    bool IsMarkedForSelection() const {return this->isMarkedForSelection;};
    void SetMarkedToSelection(bool value) {this->isMarkedForSelection = value;};

    // Access to children.
    typedef List<BaseParticleEditorNode*> PARTICLEEDITORNODESLIST;
    const PARTICLEEDITORNODESLIST& GetChildren() const {return childNodes;};

protected:
    // Cleanup the node's children.
    void Cleanup();

    // Root effect node.
    SceneNode* rootNode;

    // Current node name.
    QString nodeName;

    // Child nodes.
    PARTICLEEDITORNODESLIST childNodes;
    
    // Whether the node is marked for selection.
    bool isMarkedForSelection;
};

}

#endif /* defined(__ResourceEditorQt__BaseParticleEditorNode__) */
