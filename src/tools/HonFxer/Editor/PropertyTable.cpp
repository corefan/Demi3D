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

#include "FxerPch.h"
#include "PropertyTable.h"
#include "MainWorkspaceControl.h"
#include "MainPaneControl.h"
#include "ToolsControl.h"
#include "PropertiesControl.h"
#include "HonFxerApp.h"
#include "PanelView.h"

namespace Demi
{
    DiEditProperty::DiEditProperty(const DiString& caption, DiPropertyBase* prop) : mCaption(caption)
        , mProperty(prop)
    {
    }

    DiEditProperty::~DiEditProperty()
    {
        DI_DELETE mProperty;
    }

    DiPropertyGroup::DiPropertyGroup(const DiString& name) :mGroupName(name)
    {

    }

    DiPropertyGroup::~DiPropertyGroup()
    {
        for (auto p : mProperties)
        {
            DI_DELETE p;
        }
    }

    void DiPropertyGroup::AddProperty(const DiString& caption, DiPropertyBase* prop)
    {
        mProperties.push_back(DI_NEW DiEditProperty(caption, prop));
    }

    void DiPropertyGroup::CreateUI()
    {
        auto toolsCtrl = HonFxerApp::GetFxApp()->GetMainPane()->GetWorkspaceControl()->GetToolsControl();
        auto propCtrl = toolsCtrl->getPropertiesCtrl();
        auto group = propCtrl->getPanelView()->AddGroup();
        group->SetCaption(mGroupName);
        
        for (auto p : mProperties)
            group->AddItem(p->mCaption, p->mProperty);
    }
}