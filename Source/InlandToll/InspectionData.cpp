// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionData.h"

FInspectionData::FInspectionData()
	: InspectionName(FText::GetEmpty())
	, InspectionDescription(FText::GetEmpty())
	, InspectionID(0)
	, AttachedSocketName(TEXT(""))
	, InspectionPropClass(nullptr)
{
}

FInspectionData::~FInspectionData()
{
}
