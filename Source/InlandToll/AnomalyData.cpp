// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyData.h"

UAnomalyData::UAnomalyData()
	: InspectionName(FText::GetEmpty())
	, InspectionDescription(FText::GetEmpty())
	, InspectionID(0)
	, AttachedSocketName(TEXT(""))
	, InspectionPropClass(nullptr)
{
}

UAnomalyData::~UAnomalyData()
{
}
