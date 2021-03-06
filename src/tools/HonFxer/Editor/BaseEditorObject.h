/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#ifndef BaseEditorObject_h__
#define BaseEditorObject_h__

#include "FxerPrerequisites.h"
#include "MyGUI_TreeControl.h"
#include "PropertyTable.h"

namespace Demi 
{
    typedef MyGUI::TreeControl::Node UINode;

    class DiBaseEditorObj : public DiBase
    {
    public:

        DiBaseEditorObj();

        virtual                     ~DiBaseEditorObj();

    public:

        template<typename T>
        T*                          CreateChild(const DiString& type)
        {
            return dynamic_cast<T*>(_CreateChild(type));
        }

        virtual DiBaseEditorObj*    _CreateChild(const DiString& type);
        
        virtual DiBaseEditorObj*    _CreateChildFrom(const DiString& type, const DiAny& param);

        virtual void                RemoveChild(const DiBaseEditorObj* child);
        
        virtual void                ClearChildren();

        DiBaseEditorObj*            GetParent() { return mParent; }
        
        DiBaseEditorObj*            LookForParent(const DiString& type);
        
        DiBaseEditorObj*            GetChild(size_t id) {return mChildren[id];}
        
        size_t                      GetNumChildren(){return mChildren.size();}
        
        void                        TraversalChildren(const std::function<void(size_t, DiBaseEditorObj*)> func);

    public:

        virtual void                OnMenuPopup(MyGUI::PopupMenu* menu, bool multiSelection);

        virtual void                OnCreate();
        
        virtual void                PostCreate(){}
        
        virtual void                OnCreate(const DiAny& param) {}

        virtual void                OnDestroy() {}

        virtual void                OnDestroyUI();

        virtual void                OnSelect();
        
        virtual void                OnSelectLost();

        virtual void                OnCreateUI();

        virtual DiString            GetUICaption() { return "Scene Root"; }

        virtual DiString            GetUINodeType() { return "Folder"; }

        virtual DiString            GetType() { return "Base"; }

        UINode*                     GetUINode() { return mUINode; }
        
        DiTransGizmo*               GetGizmo() { return mGizmo; }

        virtual void                Update(float dt);
        
        void                        Release();
        
        virtual void                NotifyMousePressed(int _left, int _top, MyGUI::MouseButton _id);
        
        virtual void                NotifyMouseReleased(int _left, int _top, MyGUI::MouseButton _id);
        
        virtual void                NotifyMouseMove(int _left, int _top);
        
        virtual void                NotifyMouseDrag(int _left, int _top, MyGUI::MouseButton _id);
        
        virtual void                NotifyTransfromUpdate();
        
        virtual void                SetPosition(const DiVec3& pos);
        
        virtual void                SetRotation(const DiQuat& rot){}
        
        virtual void                SetScale(const DiVec3& scale){}
        
        virtual DiVec3              GetPosition();
        
        virtual DiQuat              GetRotation(){ return DiQuat::ZERO; }
        
        virtual DiVec3              GetScale(){ return DiVec3::UNIT_SCALE; }
        
        DiCullNode*                 GetSceneNode(){ return mSceneNode; }
        
    protected:
        
        void                        SetPropertyTableVisible(bool visible);
        
        virtual void                InitPropertyTable(){}
        
        virtual void                DestroyPropertyTable();
        
        virtual void                UpdatePropertyUI(const DiString& caption);

    protected:

        DiVector<DiBaseEditorObj*>  mChildren;

        DiBaseEditorObj*            mParent{ nullptr };

        /// Tree control node
        UINode*                     mUINode{ nullptr };
        
        DiCullNode*                 mSceneNode{ nullptr };

        PropertyGroups              mPropGroups;

        DiTransGizmo*               mGizmo{ nullptr };
        
        /// Transformation properties
        DiVec3Property*             mPositionProp{ nullptr };
    };
}

#endif
