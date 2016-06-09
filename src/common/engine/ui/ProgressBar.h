//
// Copyright (c) 2008-2015 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/Math/Vector2.h>
#include <Urho3D/UI/Text.h>

namespace spacel {
namespace engine {
namespace ui {

/// %ProgressBar bar %UI element.
class URHO3D_API ProgressBar : public Urho3D::BorderImage
{
URHO3D_OBJECT(ProgressBar, BorderImage);

public:
/// Construct.
	ProgressBar(Urho3D::Context *context);

/// Destruct.
	virtual ~ProgressBar();

/// Register object factory.
	static void RegisterObject(Urho3D::Context *context);

	/// React to resize.
	virtual void OnResize();

	/// Set orientation type.
	void SetOrientation(Urho3D::Orientation orientation);

	/// Set ProgressBar range maximum value (minimum value is always 0.)
	void SetRange(float range);

	/// Set ProgressBar current value.
	void SetValue(float value);

	/// Change value by a delta.
	void ChangeValue(float delta);

	/// Return orientation type.
	Urho3D::Orientation GetOrientation() const
	{ return orientation_; }

	/// Return ProgressBar range.
	float GetRange() const
	{ return range_; }

	/// Return ProgressBar current value.
	float GetValue() const
	{ return value_; }

	/// Return knob element.
	Urho3D::BorderImage *GetKnob() const
	{ return knob_; }

	void SetLoadingPercentStyle(const Urho3D::String &style)
	{ loading_text_style_ = style; }

protected:
	/// Filter implicit attributes in serialization process.
	virtual bool FilterImplicitAttributes(Urho3D::XMLElement &dest) const;

	/// Update ProgressBar knob position & size.
	void UpdateLoadingBar();

	/// ProgressBar knob.
	Urho3D::SharedPtr <Urho3D::BorderImage> knob_;
	/// ProgressBar text
	Urho3D::SharedPtr <Urho3D::Text> loading_text_;
	/// Orientation.
	Urho3D::Orientation orientation_;
	/// ProgressBar range.
	float range_;
	/// ProgressBar current value.
	float value_;
	/// ProgressBar text style
	Urho3D::String loading_text_style_;
};

}
}
}
