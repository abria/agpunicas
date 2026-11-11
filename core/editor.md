# Level editor appearance

The editor provides 30 categories (IDs 0–29). Older levels gain the missing
categories when opened; existing names, IDs and object properties are retained.
The extended list is written on the next normal save. Documents with more than
30 categories are not truncated (their accent palette repeats after 30).

- Two-finger touchpad scrolling (or mouse wheel): pan horizontally/vertically.
  Fractional deltas are preserved, at a constant screen-space speed at every zoom.
  Middle-button dragging still pans. Pan also works with a selected object.
- Command + scroll on macOS, Alt + scroll on Windows/Linux: smooth zoom.
  Plain `+` / `-` (also `=` and numeric keypad +/-) zoom without a scroll modifier,
  so they remain available when accessibility settings reserve a modifier.
  Zoom shortcuts are inactive while typing a category/object name.
- Shift + scroll with a selected object: rotate in 10-degree steps;
  Ctrl + Shift + scroll: rotate in 1-degree steps.
- Ctrl + scroll: grid size, from 1/16 to 64 scene units. Fractional touchpad
  input accumulates into whole grid/rotation steps.
- Space: next category while drawing; Shift+Space: previous category.
- The bottom bar shows the active/selected category ID and full name.
- Labels inside objects show only the category name; IDs remain in the bottom bar.
- Category strokes and text use an editor-specific palette in `EditorStyle.h`.
  All 30 accents exceed 7:1 contrast against the opaque label background.
- Objects keep translucent fills, opaque dark/colored outlines, a stronger
  outline on hover, and a white outer ring/corner markers when selected.
- Editor labels, coordinates and help use Roboto Condensed Medium (weight 500),
  without synthetic bold. Each game bundles the font and its license in `fonts/`.
- Names and categories occupy separate label bands and scale down with string
  length and object size to fit inside the object. Their background plates also
  stay within its bounds. Small objects may require zooming in to read the text.
- Cursor coordinates and help text use the same dark, high-contrast treatment.
- The help panel sizes itself to its content at a fixed readable screen size,
  with a compact inset background instead of full-width rows. Long help lines
  wrap when the window is narrow. H toggles auto-hide; hovering the panel reveals it.

Text rendering requires the existing `WITH_TTF=ON` build option. The optional
text backgrounds default to disabled outside the editor.
