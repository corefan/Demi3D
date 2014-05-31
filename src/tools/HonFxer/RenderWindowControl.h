
#ifndef __FXER_RENDER_WINDOW_CONTROL_H__
#define __FXER_RENDER_WINDOW_CONTROL_H__

#include "FxerPrerequisites.h"
#include "BaseLayout.h"

namespace tools
{
	class RenderWindowControl : public wraps::BaseLayout
	{
	public:
        RenderWindowControl(MyGUI::Widget* _parent);
        virtual ~RenderWindowControl();

    public:
        void onUpdateCanvas(MyGUI::Canvas*, MyGUI::Canvas::Event evt);

        virtual void update();

        bool mouseInCanvas();

        void updateInfo();

    protected:

        MyGUI::Canvas* mCanvas{ nullptr };

        MyGUI::TextBox* mInfo{ nullptr };

        MyGUI::TextBox* mInfoStatic{ nullptr };
	};

} // namespace tools

#endif // __RENDER_WINDOW_CONTROL_H__