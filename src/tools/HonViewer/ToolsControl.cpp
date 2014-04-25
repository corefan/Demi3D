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

#include "ViewerPch.h"
#include "ToolsControl.h"
#include "FilesView.h"
#include "CommonToolControl.h"

namespace tools
{
	ToolsControl::ToolsControl(MyGUI::Widget* _parent) :
		wraps::BaseLayout("ToolsControl.layout", _parent)
        , mFilesView(nullptr)
        , mCommonTools(nullptr)
	{
        assignBase(mFilesView, "FilesControl");
        assignBase(mCommonTools, "CommonToolsControl");
	}

	ToolsControl::~ToolsControl()
	{
	}

} // namespace tools