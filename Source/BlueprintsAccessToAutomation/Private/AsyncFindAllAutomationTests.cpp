// Copyright 2021 - Trifolium Digital Limited

#include "AsyncFindAllAutomationTests.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "IAutomationControllerModule.h"
#include "Misc/FilterCollection.h"
#include "IAutomationReport.h"

#include "Engine.h"
#endif // WITH_DEV_AUTOMATION_TESTS


DEFINE_LOG_CATEGORY_STATIC(LogAsyncFindAllAutomationTests, NoLogging, All);


UAsyncFindAllAutomationTests* UAsyncFindAllAutomationTests::AsyncFindAllAutomationTests(UObject* WorldContextObject)
{
	UE_LOG(LogAsyncFindAllAutomationTests, Verbose, TEXT("UAsyncFindAllAutomationTests::AsyncFindAllAutomationTests"));
	UAsyncFindAllAutomationTests* Action = NewObject<UAsyncFindAllAutomationTests>();
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}


void UAsyncFindAllAutomationTests::Activate()
{
	UE_LOG(LogAsyncFindAllAutomationTests, Verbose, TEXT("UAsyncFindAllAutomationTests::Activate"));
#if WITH_DEV_AUTOMATION_TESTS
	IAutomationControllerModule& AutomationControllerModule = FModuleManager::LoadModuleChecked<IAutomationControllerModule>(TEXT("AutomationController"));
	IAutomationControllerManagerRef AutomationControllerManager = AutomationControllerModule.GetAutomationController();
	{
		AutomationControllerManager->OnTestsRefreshed().AddUObject(this, &UAsyncFindAllAutomationTests::OnTestsRefreshed);
		GEngine->Exec(nullptr, TEXT("Automation List"));
	}
#endif // WITH_DEV_AUTOMATION_TESTS
}


void UAsyncFindAllAutomationTests::OnTestsRefreshed() const
{
	UE_LOG(LogAsyncFindAllAutomationTests, Verbose, TEXT("UAsyncFindAllAutomationTests::OnTestsRefreshed"));
	FAsyncFindAllAutomationTestsResult FindAllAutomationTestsResult;
	
#if WITH_DEV_AUTOMATION_TESTS
	IAutomationControllerModule& AutomationControllerModule = FModuleManager::LoadModuleChecked<IAutomationControllerModule>(TEXT("AutomationController"));
	IAutomationControllerManagerRef AutomationControllerManager = AutomationControllerModule.GetAutomationController();
	{
		// Create a filter to add to the automation controller, otherwise we don't get any reports
		AutomationControllerManager->SetFilter(MakeShareable(new AutomationFilterCollection()));
		AutomationControllerManager->SetVisibleTestsEnabled(true);

		// Read results from automation controller
		AutomationControllerManager->GetEnabledTestNames(FindAllAutomationTestsResult.AllTestNames);
		
		UE_LOG(LogAsyncFindAllAutomationTests, Log, TEXT("Found %d tests."), FindAllAutomationTestsResult.AllTestNames.Num());
		for (const FString& TestName : FindAllAutomationTestsResult.AllTestNames)
		{
			UE_LOG(LogAsyncFindAllAutomationTests, Verbose, TEXT(" ... %s"), *TestName);
		}
	}	
	AutomationControllerManager->OnTestsRefreshed().RemoveAll(this);
#endif // WITH_DEV_AUTOMATION_TESTS
	
	OnTestNamesReady.Broadcast(FindAllAutomationTestsResult);
}
