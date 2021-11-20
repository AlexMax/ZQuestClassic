#include "alertfunc.h"
#include <gui/builder.h>
#include <utility>
#include <cstdarg>
#include <gui/size.h>
extern int32_t zq_screen_w;

AlertFuncDialog::AlertFuncDialog(std::string title, std::string text, uint32_t numButtons, uint32_t focused_button, ...):
	InfoDialog(title,text)
{
	va_list args;
	va_start(args, focused_button);
	initButtons(args, numButtons, focused_button);
	va_end(args);
}

AlertFuncDialog::AlertFuncDialog(std::string title, std::vector<std::string_view> lines, uint32_t numButtons, uint32_t focused_button,  ...):
	InfoDialog(title,lines)
{
	va_list args;
	va_start(args, focused_button);
	initButtons(args, numButtons, focused_button);
	va_end(args);
}

std::shared_ptr<GUI::Widget> AlertFuncDialog::view()
{
	using namespace GUI::Builder;
	using namespace GUI::Props;

	std::shared_ptr<GUI::Window> window = Window(
		title = std::move(dlgTitle),
		onEnter = message::OK,
		onClose = message::OK,
		Column(
			Label(
				maxwidth = sized(
					GUI::Size::pixels(zq_screen_w)-(2*((2*DEFAULT_PADDING)+1_em)),
					30_em),
				hPadding = 1_em,
				maxLines = 30,
				textAlign = 1,
				text = std::move(dlgText)),
			buttonRow = Row(
				topPadding = 0.5_em,
				vAlign = 1.0,
				spacing = 2_em
			)
		)
	);
	for(std::shared_ptr<GUI::Button>& btn : buttons)
	{
		buttonRow->add(btn);
	}
	return window;
}

void AlertFuncDialog::initButtons(va_list args, uint32_t numButtons, uint32_t focused_button)
{
	using namespace GUI::Builder;
	using namespace GUI::Props;
	if(numButtons)
	{
		//even args only, as (, char*, void(*func)(),)
		for(uint32_t q = 0; q < numButtons; ++q)
		{
			std::string btntext(va_arg(args, char*));
			typedef void (*funcType)(void);
			std::function<void()> func = va_arg(args, funcType);
			if(func)
			{
				buttons.push_back(
					Button(
						text = btntext,
						onPressFunc = func,
						focused = (q==focused_button)
					));
			}
			else
			{
				buttons.push_back(
					Button(
						text = btntext,
						onClick = message::OK,
						focused = (q==focused_button)
					));
			}
		}
	}
	else
	{
		buttons.push_back(
			Button(
				text = "OK",
				onClick = message::OK,
				focused = true
			));
	}
}

bool AlertFuncDialog::handleMessage(const GUI::DialogMessage<int32_t>& msg)
{
	return true;
}