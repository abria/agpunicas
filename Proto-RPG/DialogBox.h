// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScene.h"
#include "RenderableObject.h"
#include <vector>

namespace agp
{
	class DialogBox;
}

// DialogBox
// - implements scrollable interactive dialog boxes
class agp::DialogBox : public UIScene
{
	protected:

		std::vector < std::vector < RenderableObject*> > _charObjects;
		std::string _text;
		int _visibleLines;

	public:

		DialogBox(const std::string& text, const PointF& pos = {1*16, 10*16}, int visibleLines = 3, int wrapLength = 32);

		// extends update logic (+autoscroll)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+interaction)
		virtual void event(SDL_Event& evt) override;
};