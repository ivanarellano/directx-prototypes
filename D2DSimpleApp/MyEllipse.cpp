#include "MyEllipse.h"

const void MyEllipse::Draw(ID2D1RenderTarget* render_target, ID2D1SolidColorBrush* stroke) const
{
	stroke->SetColor(color);
	render_target->FillEllipse(ellipse, stroke);

	stroke->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
	render_target->DrawEllipse(ellipse, stroke, 1.0f);
}

const BOOL MyEllipse::HitTest(FLOAT x, FLOAT y) const
{
	const FLOAT a{ ellipse.radiusX };
	const FLOAT b{ ellipse.radiusY };
	const FLOAT x1{ x - ellipse.point.x };
	const FLOAT y1{ y - ellipse.point.y };
	const float d{ (x1 * x1) / (a * a) + (y1 * y1) / (b * b) };
	return d <= 1.0f;
}