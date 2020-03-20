#pragma once


class Widgets
{
public:
	static DialogInfoEx* GetWidgetUI(uint32_t FullIndex);
	static DialogInfoEx* IsInDialogWidget();
	static RSDialog GetDialogType();
};