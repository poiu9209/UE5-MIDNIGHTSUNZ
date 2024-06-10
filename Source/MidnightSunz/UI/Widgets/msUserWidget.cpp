// Fill out your copyright notice in the Description page of Project Settings.


#include "msUserWidget.h"

void UmsUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
