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
// - implements interactive modal dialog box with on-new-row autoscroll
//   and one-by-one character display
class agp::DialogBox : public UIScene
{
	protected:

		std::vector < std::vector < RenderableObject*> > _charObjects;
		std::string _text;
		std::vector<RenderableObject*> _optionObjects;
		std::string _name;
		std::vector<std::string> _optionList;

		// parameters
		int _visibleLines;
		const float _charsPerSecond = 10;
		const float _scrollingRowTime = 0.5f;	// in seconds
		const Point _fontSize = { 7, 15 };		// in scene units
		const int _rowMargin = 3;				// in scene units
		const float _fasterScrollMult = 3;

		// state attributes
		bool _scrollingRow;
		float _charIterator;
		int _rowIterator;
		int _currentOption;

		// helper function
		void updateCurrentOption();

	public:

		DialogBox(
			const std::string& name,
			const std::string& text,
			const std::string& options = "",	// separated by comma
			const PointF& pos = { 1 * 16, 10 * 16 },
			int visibleLines = 3,
			int wrapLength = 32);

		// extends update logic (+scroll, +one by one characters)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+interaction)
		virtual void event(SDL_Event& evt) override;
};