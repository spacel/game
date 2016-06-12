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

#include "ProgressBar.h"

#include <Urho3D/Core/Context.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/UI/Slider.h>
#include <Urho3D/UI/UIEvents.h>

using namespace Urho3D;

namespace spacel {
namespace engine {
namespace ui {

const char *orientations[] =
{
	"Horizontal",
	"Vertical",
	0
};

ProgressBar::ProgressBar(Context * context) :
	BorderImage(context),
	orientation_(O_HORIZONTAL),
	range_(1.0f),
	value_(0.0f),
	loading_text_style_("Text")
{
	SetEnabled(false);
	SetEditable(false);
	SetFocus(false);
	knob_ = CreateChild<BorderImage>("S_Knob");
	knob_->SetInternal(true);

	loading_text_ = CreateChild<Text>("S_Text");
	loading_text_->SetInternal(true);

	UpdateLoadingBar();
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::RegisterObject(Context * context)
{
	context->RegisterFactory<ProgressBar>("UI");

	URHO3D_COPY_BASE_ATTRIBUTES(BorderImage);
	URHO3D_UPDATE_ATTRIBUTE_DEFAULT_VALUE("Is Enabled", true);
	URHO3D_ENUM_ACCESSOR_ATTRIBUTE("Orientation", GetOrientation, SetOrientation,
								   Orientation, orientations, O_HORIZONTAL, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Range", GetRange, SetRange, float, 1.0f, AM_FILE);
	URHO3D_ACCESSOR_ATTRIBUTE("Value", GetValue, SetValue, float, 0.0f, AM_FILE);
}

void ProgressBar::OnResize()
{
	UpdateLoadingBar();
}

void ProgressBar::SetOrientation(Orientation type)
{
	orientation_ = type;
	UpdateLoadingBar();
}

void ProgressBar::SetRange(float range)
{
	range = Max(range, 0.0f);
	if (range != range_) {
		range_ = range;
		UpdateLoadingBar();
	}
}

void ProgressBar::SetValue(float value)
{
	value = Clamp(value, 0.0f, range_);
	if (value != value_) {
		value_ = value;
		UpdateLoadingBar();

		using namespace SliderChanged;

		VariantMap &eventData = GetEventDataMap();
		eventData[P_ELEMENT] = this;
		eventData[P_VALUE] = value_;
		SendEvent(E_SLIDERCHANGED, eventData);
	}
}

void ProgressBar::ChangeValue(float delta)
{
	SetValue(value_ + delta);
}

bool ProgressBar::FilterImplicitAttributes(XMLElement &dest) const
{
	if (!BorderImage::FilterImplicitAttributes(dest))
		return false;

	XMLElement childElem = dest.GetChild("element");
	if (!childElem)
		return false;
	if (!RemoveChildXML(childElem, "Name", "S_Knob"))
		return false;
	if (!RemoveChildXML(childElem, "Name", "S_Text"))
		return false;
	if (!RemoveChildXML(childElem, "Position"))
		return false;
	if (!RemoveChildXML(childElem, "Size"))
		return false;

	return true;
}

void ProgressBar::UpdateLoadingBar()
{
	const IntRect &border = knob_->GetBorder();

	if (range_ > 0.0f) {
		if (orientation_ == O_HORIZONTAL) {
			int loadingBarLength = (int) Max((float) GetWidth() * value_ / range_,
				(float) (border.left_ + border.right_));
			knob_->SetSize(loadingBarLength, GetHeight());
			knob_->SetPosition(Clamp(0, 0, GetWidth() - knob_->GetWidth()), 0);
		}
		else {
			int loadingBarLength = (int) Max((float) GetHeight() * value_ / range_,
				(float) (border.top_ + border.bottom_));
			knob_->SetSize(GetWidth(), loadingBarLength);
			knob_->SetPosition(0, Clamp(0, 0, GetHeight() - knob_->GetHeight()));
		}
	}
	else {
		knob_->SetSize(GetSize());
		knob_->SetPosition(0, 0);
	}

	loading_text_->SetStyle(loading_text_style_);
	loading_text_->SetAlignment(HA_CENTER, VA_CENTER);
	loading_text_->SetText(Urho3D::ToString("%f %%", value_));
}

}
}
}
