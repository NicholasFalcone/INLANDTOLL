#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "AnomalyData.h"
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanel.h"
#include "Tools/TabletComponent.h"
#include "UI/TableUI.h"
#include "UI/UI_Master.h"
#include "UObject/UnrealType.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTabletComponentVisibilityTest, "InlandToll.Tablet.VisibilityAndSheet",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTabletComponentVisibilityTest::RunTest(const FString& Parameters)
{
    UTabletComponent* Tablet = NewObject<UTabletComponent>();
    UTableUI* Sheet = NewObject<UTableUI>();
    UUserWidget* DefaultPage = NewObject<UTableUI>();
    UCanvasPanel* Container = NewObject<UCanvasPanel>();
    UAnomalyData* Data = NewObject<UAnomalyData>();

    AddExpectedError(TEXT("Tablet sheet widget must define an OnSheetUpdate dispatcher"), EAutomationExpectedErrorFlags::Contains, 1);
    Tablet->InitializeWidgets(Sheet, DefaultPage, Container);
    TestTrue(TEXT("Widgets are ready"), Tablet->AreWidgetsReady());
    TestEqual(TEXT("Tablet starts closed"), Container->GetVisibility(), ESlateVisibility::Collapsed);

    Tablet->SetOpen(true);
    TestEqual(TEXT("Default page is visible without a sheet"), DefaultPage->GetVisibility(), ESlateVisibility::SelfHitTestInvisible);
    TestEqual(TEXT("Sheet is hidden initially"), Sheet->GetVisibility(), ESlateVisibility::Collapsed);

    TestTrue(TEXT("Sheet can be loaded"), Tablet->UpdateAnomaly(Data));
    TestTrue(TEXT("Tablet has a sheet"), Tablet->HasSheet());
    TestEqual(TEXT("Loaded sheet is visible"), Sheet->GetVisibility(), ESlateVisibility::Visible);
    TestEqual(TEXT("Default page is hidden"), DefaultPage->GetVisibility(), ESlateVisibility::Collapsed);
    TestFalse(TEXT("Uncompiled sheet cannot be delivered"), Tablet->CanBeDelivered());
    Sheet->bSheetCompiled = true;
    TestTrue(TEXT("Compiled sheet can be delivered"), Tablet->CanBeDelivered());

    Tablet->SetOpen(false);
    TestEqual(TEXT("Closing hides the tablet"), Container->GetVisibility(), ESlateVisibility::Collapsed);
    TestTrue(TEXT("Closing preserves the sheet"), Tablet->HasSheet());
    Tablet->SetOpen(true);
    TestEqual(TEXT("Reopening shows the sheet"), Sheet->GetVisibility(), ESlateVisibility::Visible);

    Tablet->RemoveSheet();
    TestFalse(TEXT("Removing clears the sheet"), Tablet->HasSheet());
    TestEqual(TEXT("Removing restores the default page"), DefaultPage->GetVisibility(), ESlateVisibility::SelfHitTestInvisible);

    AddExpectedError(TEXT("Cannot update tablet"), EAutomationExpectedErrorFlags::Contains, 1);
    TestFalse(TEXT("Null data is rejected"), Tablet->UpdateAnomaly(nullptr));
    TestFalse(TEXT("Rejected data does not create a sheet"), Tablet->HasSheet());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTabletSheetDispatcherTest, "InlandToll.Tablet.SheetDispatcher",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTabletSheetDispatcherTest::RunTest(const FString& Parameters)
{
    UClass* SheetClass = LoadClass<UTableUI>(nullptr, TEXT("/Game/Blueprints/Widgets/WBP_TabletUI.WBP_TabletUI_C"));
    if (!TestNotNull(TEXT("Sheet Blueprint class"), SheetClass))
    {
        return false;
    }

    UTableUI* Sheet = NewObject<UTableUI>(GetTransientPackage(), SheetClass);
    FMulticastDelegateProperty* Dispatcher = FindFProperty<FMulticastDelegateProperty>(SheetClass, TEXT("OnSheetUpdate"));
    if (!TestNotNull(TEXT("Sheet update dispatcher"), Dispatcher))
    {
        return false;
    }

    UTabletComponent* Tablet = NewObject<UTabletComponent>();
    Tablet->InitializeWidgets(Sheet, NewObject<UTableUI>(), NewObject<UCanvasPanel>());
    const FMulticastScriptDelegate* Binding = Dispatcher->GetMulticastDelegate(Dispatcher->ContainerPtrToValuePtr<void>(Sheet));
    const FName Callback = GET_FUNCTION_NAME_CHECKED(UTabletComponent, NotifyTabletSheetUpdated);
    TestTrue(TEXT("Sheet dispatcher is connected to the component"), Binding && Binding->Contains(Tablet, Callback));

    Tablet->InitializeWidgets(nullptr, nullptr, nullptr);
    Binding = Dispatcher->GetMulticastDelegate(Dispatcher->ContainerPtrToValuePtr<void>(Sheet));
    TestFalse(TEXT("Sheet dispatcher is disconnected on teardown"), Binding && Binding->Contains(Tablet, Callback));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTabletMasterWidgetTest, "InlandToll.Tablet.MasterWidgetTree",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTabletMasterWidgetTest::RunTest(const FString& Parameters)
{
    UClass* MasterClass = LoadClass<UUI_Master>(nullptr, TEXT("/Game/Blueprints/Widgets/WBP_Master.WBP_Master_C"));
    if (!TestNotNull(TEXT("Master Blueprint class"), MasterClass))
    {
        return false;
    }

    UUI_Master* Master = NewObject<UUI_Master>(GetTransientPackage(), MasterClass);
    if (!TestTrue(TEXT("Master widget initializes"), Master->Initialize()))
    {
        return false;
    }
    TestNotNull(TEXT("Tablet container"), Master->GetWidgetFromName(TEXT("Tablet")));
    TestNotNull(TEXT("Tablet sheet"), Cast<UTableUI>(Master->GetWidgetFromName(TEXT("WBP_TabletUI"))));
    TestNotNull(TEXT("Tablet default page"), Cast<UUserWidget>(Master->GetWidgetFromName(TEXT("WBP_TabletDefaultPaage"))));
    return true;
}

#endif
