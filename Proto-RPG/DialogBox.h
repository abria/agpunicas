// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScene.h"
#include "RenderableObject.h"
#include <vector>
#include <functional>

namespace agp
{
	class DialogBox;
}

// DialogBox
// - implements interactive modal dialog box with
//   - on-new-row autoscroll
//   - one-by-one character display
//   - selectable options with associated actions
class agp::DialogBox : public UIScene
{
	public:

		struct Option
		{
			std::string name;
			std::function<void()> action;

			Option(const std::string& optName, std::function<void()> optAction)
				: name(optName), action(optAction) {}
		};

	protected:

		// data
		std::string _text;
		std::vector<Option> _options;

		// graphics
		std::vector < std::vector < RenderableObject*> > _chars;
		std::vector < RenderableObject* > _arrows;

		// parameters
		int _visibleLines;
		const float _charsPerSecond = 10;
		const float _scrollingRowTime = 0.5f;	// in seconds
		const Point _fontSize = { 7, 15 };		// in scene units
		const int _rowMargin = 3;				// in scene units
		const float _fasterScrollMult = 3;

		// state
		bool _scrollingRow;
		float _charIterator;
		int _rowIterator;
		int _currentOption;

		// helper functions
		void updateCurrentOption();

	public:

		DialogBox(
			const std::string& text,
			const std::vector<Option>& options = std::vector<Option>(),
			const PointF& pos = { 1 * 16, 10 * 16 },
			int visibleLines = 3,
			int wrapLength = 32);

		// extends update logic (+scroll, +one by one characters)
		virtual void update(float timeToSimulate) override;

		// extends event handler (+interaction)
		virtual void event(SDL_Event& evt) override;
};