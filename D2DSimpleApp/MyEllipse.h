#include "headers.h"

#pragma once

struct MyEllipse
{
	MyEllipse(D2D1_ELLIPSE ellipse, D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Red))
		: ellipse(ellipse)
		, color(color)
	{
	}

	D2D1_ELLIPSE ellipse;
	D2D1_COLOR_F color;

	const void Draw(ID2D1RenderTarget* render_target, ID2D1SolidColorBrush* stroke) const;
	const BOOL HitTest(FLOAT x, FLOAT y) const;
};