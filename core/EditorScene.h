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
	class EditorUI;
	class GameScene;
	class RenderableObject;
	class EditableObject;
}

// EditorScene (or Map Editor) class
// - 
// - mouse interactions (pan/zoom, selection, drag-and-drop)
// - text input (objects' categories and names)
// - load/save from/to json
class agp::EditorScene : public UIScene
{
	public:

		enum class State { 
			DEFAULT, 
			DRAW_RECT, DRAW_LINE, 
			RENAME_CATEGORY, RENAME_OBJECT, 
			SELECT, SAVING };

	protected:

		GameScene* _gameScene;
		RectF _gameRect;
		EditorUI* _ui;

		// mouse tracking
		PointF _mouseCoordsF;
		PointF _mouseCoordsSnap;
		PointF _mousePointCurr;

		// pan & zoom
		bool _isPanning;
		PointF _panningDelta;
		PointF _lastMousePositionPanning;
		float _cameraZoomVel;		// camera zoom velocity (in [0,1] relative scale units)

		// drag and drop
		bool _isDragging;
		PointF _dragStartMousePosition;
		PointF _dragStartObjectPosition;
		EditableObject* _draggedObject;

		// resizing
		bool _isResizing;
		EditableObject* _resizingObject;

		// renderables
		std::vector<EditableObject*> _editObjects;
		std::vector<RenderableObject*> _grid;

		// state attributes
		float _gridCellSize;
		bool _snapGrid;
		int _currentCategory;
		EditableObject* _currentCell;
		EditableObject* _currentObject;
		std::string _textInput;
		std::vector<std::string> _categories;
		State _state;
		State _prevState;

		// constants
		static constexpr int MAX_CATEGORIES = 15;
		const Color GRID_COLOR = { 100,100,100 };
		const std::string DEFAULT_SAVE_FILENAME = "EditorScene.json";

		// helper functions
		void fromJson();
		void toJson();
		void updateState(State newState);
		void generateGrid();
		EditableObject* editableUnderMouse();
		void checkResizing();

	public:

		EditorScene(GameScene* gameScene, EditorUI* ui);
		virtual ~EditorScene() {};

		void toggleSnapGrid() { _snapGrid = !_snapGrid; }
		void toggleGrid();

		// +panning, +syncronization with game scene
		virtual void update(float timeToSimulate) override;

		// +editor controls
		virtual void event(SDL_Event& evt) override;
};