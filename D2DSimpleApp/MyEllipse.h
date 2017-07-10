#include "headers.h"

#pragma once

struct MyEllipse
{
	D2D1_ELLIPSE ellipse;
	D2D1_COLOR_F color;

	const void Draw(ID2D1RenderTarget* render_target, ID2D1SolidColorBrush* brush) const;
	const BOOL HitTest(FLOAT x, FLOAT y) const;
};