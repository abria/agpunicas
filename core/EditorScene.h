// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include "UIScene.h"
#include "graphicsUtils.h"

namespace agp
{
	class EditorScene;
	class GameScene;
	class RenderableObject;
	class EditableObject;
}

// EditorScene (or Map Editor) class
// - mouse-interaction-based object editing
// - load/save from/to json
class agp::EditorScene : public UIScene
{
	protected:

		GameScene* _gameScene;
		RectF _gameRect;

		bool _snapGrid;
		EditableObject* _currentCell;
		EditableObject* _currentObject;
		std::vector<EditableObject*> _editObjects;
		std::vector<RenderableObject*> _grid;
		int _currentCategory;
		std::vector<std::string> _categories;
		static const int MAX_CATEGORIES = 15;

		// camera controls
		Vec2Df _cameraTranslateVel;
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

		// helper functions
		virtual void fromJson();
		virtual void toJson();

	public:

		EditorScene(GameScene* gameScene);
		virtual ~EditorScene() {};

		void toggleSnapGrid() { _snapGrid = !_snapGrid; }

		// +syncronization with game scene
		virtual void update(float timeToSimulate) override;

		// +editor controls
		virtual void event(SDL_Event& evt) override;
};