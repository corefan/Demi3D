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

#ifndef DiTransGizmoHelper_h__
#define DiTransGizmoHelper_h__

#include "FxerPrerequisites.h"
#include "MyGUI_MouseButton.h"

namespace Demi
{
    /** Transform gizmo class
     */
    class DiTransGizmo
    {
    public:
        
        DiTransGizmo(void);

        ~DiTransGizmo(void);

        enum GizmoMode
        {
            GIZMO_HIDE,
            GIZMO_SELECT,
            GIZMO_MOVE,
            GIZMO_ROTATE,
            GIZMO_SCALE,
        };
        
        enum RotatePick
        {
            PICK_NONE = 0,
            PICK_ROT_X,
            PICK_ROT_Y,
            PICK_ROT_Z,
            PICK_ROT_XYZ,
        };
        
    public:

        void            Update();
        
        void            SetGizmoMode(GizmoMode mode);
        
        GizmoMode       GetGizmoMode() { return mMode; }
        
        void            OnMouseMove(int _left, int _top);
        
        void            OnMouseDown(int _left, int _top, MyGUI::MouseButton _id);
        
        void            OnMouseUp(int _left, int _top, MyGUI::MouseButton _id);
        
        void            Show(bool visible);
        
        bool            IsActive(){return mActive;}
        
        DiCullNode*     GetNode(){ return mBaseNode; }
        
        DiRay           GetMouseRay(int _left, int _top);
        
        DiAABB          GetWorldAABB();
        
    protected:
        
        void            Create();
        
        void            HideAll();
        
        void            GenerateRotRingVerts();
        
        RotatePick      PickRotRings(const DiRay& ray);
        
        void            HightlightRotRings(RotatePick pickret);
    
    protected:
        
        DiCullNode*     mBaseNode{ nullptr };
        
        DiCullNode*     mAxesNode{ nullptr };
        
        DiCullNode*     mRotateCircleNode{ nullptr };
        
        DiCullNode*     mScaleNode{ nullptr };
        
        GizmoMode       mMode{GIZMO_MOVE};
        
        DiDebugHelperPtr mCircles;
        
        DiTransAxesPtr  mAxes;
        
        DiSimpleShapePtr mRotateRings[3];
        
        DiVector<DiVec3> mRotatingRingsVerts;
        
        DiMaterialPtr    mRotatingRingsMat[3];
        
        DiCullNode*     mRotateRingNode[3]{nullptr,nullptr,nullptr};
        
        bool            mPicking{ false };
        
        bool            mActive{ true };
    };
}

#endif
