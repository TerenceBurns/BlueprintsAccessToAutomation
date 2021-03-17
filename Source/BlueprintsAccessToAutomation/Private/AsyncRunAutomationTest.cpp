// Copyright 2021 - Trifolium Digital Limited

#include "AsyncRunAutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "IAutomationControllerModule.h"
#endif // WITH_DEV_AUTOMATION_TESTS

DEFINE_LOG_CATEGORY_STATIC(LogAsyncRunAutomationTest, NoLogging, All);

#pragma optimize("", off)


UAsyncRunAutomationTest* UAsyncRunAutomationTest::AsyncRunAutomationTest(UObject* WorldContextObject, const FString& TestName)
{
	UAsyncRunAutomationTest* Action = NewObject<UAsyncRunAutomationTest>();
	Action->ExecutingTestName = TestName;
	Action->RegisterWithGameInstance(WorldContextObject);
	return Action;
}


void UAsyncRunAutomationTest::Activate()
{
#if WITH_DEV_AUTOMATION_TESTS
	IAutomationControllerModule& AutomationControllerModule = FModuleManager::LoadModuleChecked<IAutomationControllerModule>(TEXT("AutomationController"));
	IAutomationControllerManagerRef AutomationControllerManager = AutomationControllerModule.GetAutomationController();
	{
		AutomationControllerManager->SetNumPasses(1);
		TArray<FString> FilteredTestNames = { ExecutingTestName };
		AutomationControllerManager->SetEnabledTests(FilteredTestNames);
	
		AutomationControllerManager->OnTestsComplete().AddUObject(this, &UAsyncRunAutomationTest::OnTestComplete);

		GEngine->Exec(nullptr, *FString::Printf(TEXT("Automation RunTests %s"), *ExecutingTestName));
	}
#endif // WITH_DEV_AUTOMATION_TESTS
}


#if WITH_DEV_AUTOMATION_TESTS
void GetEnabledReports(TSharedPtr<IAutomationReport> InReport, TArray<TSharedPtr<IAutomationReport>>& OutReports)
{
	TArray<TSharedPtr<IAutomationReport>>& ChildReports = InReport->GetChildReports();

	if (ChildReports.Num() > 0)
	{
		for (int32 Index = 0; Index < ChildReports.Num(); Index++)
		{
			GetEnabledReports(ChildReports[Index], OutReports);
		}
	}
	else if (InReport->IsEnabled())
	{
		OutReports.Add(InReport);
	}
}
#endif // WITH_DEV_AUTOMATION_TESTS


void UAsyncRunAutomationTest::OnTestComplete() const
{
#if WITH_DEV_AUTOMATION_TESTS
	UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("UAsyncRunAutomationTest::OnTestComplete"));
	IAutomationControllerModule& AutomationControllerModule = FModuleManager::LoadModuleChecked<IAutomationControllerModule>(TEXT("AutomationController"));
	IAutomationControllerManagerRef AutomationControllerManager = AutomationControllerModule.GetAutomationController();
	{
		// Collect results
		FAsyncRunAutomationTestResult TestResultObj;
		TestResultObj.TestName = ExecutingTestName;
		
		TArray<TSharedPtr<IAutomationReport>> AutomationReports = AutomationControllerManager->GetReports();
		for (int32 ClusterIndex = 0; ClusterIndex < AutomationControllerManager->GetNumDeviceClusters(); ++ClusterIndex)
		{
			for (TSharedPtr<IAutomationReport> NextReport : AutomationReports)
			{
				TArray<TSharedPtr<IAutomationReport>> EnabledReports;
				GetEnabledReports(NextReport, EnabledReports);
				for (TSharedPtr<IAutomationReport> EnabledReport : EnabledReports)
				{
					const int32 NumOfPasses = EnabledReport->GetNumResults(ClusterIndex);
					for (int32 PassIndex = 0; PassIndex < NumOfPasses; ++PassIndex)
					{
						FAutomationTestResults TestResults = EnabledReport->GetResults(ClusterIndex, PassIndex);

						TestResultObj.bSucceeded = TestResults.State == EAutomationState::Success;
						
						for (const FAutomationExecutionEntry& Entry : TestResults.GetEntries())
						{
							switch (Entry.Event.Type)
							{
							case EAutomationEventType::Info:
								UE_LOG(LogAsyncRunAutomationTest, Display, TEXT("%s"), *Entry.Event.Message);
								TestResultObj.LogStrings.Add(Entry.Event.Message);
								break;
							case EAutomationEventType::Warning:
								UE_LOG(LogAsyncRunAutomationTest, Warning, TEXT("%s"), *Entry.Event.Message);
								TestResultObj.WarningStrings.Add(Entry.Event.Message);
								break;
							case EAutomationEventType::Error:
								UE_LOG(LogAsyncRunAutomationTest, Error, TEXT("%s"), *Entry.Event.Message);
								TestResultObj.ErrorStrings.Add(Entry.Event.Message);
								break;
							}
						}
						UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("Test Report = %s"), *EnabledReport->GetFullTestPath());
						UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("Succeeded? = %s"), TestResultObj.bSucceeded ? TEXT("Yes") : TEXT("No"));
						UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("Num Errors = %d"), TestResults.GetErrorTotal());
						UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("Num Warnings = %d"), TestResults.GetWarningTotal());
						UE_LOG(LogAsyncRunAutomationTest, Log, TEXT("Num Log Entries = %d"), TestResults.GetLogTotal());
					}
				}
			}
		}

		if (TestResultObj.bSucceeded)
		{
			OnSuccess.Broadcast(TestResultObj);
		}
		else
		{
			OnFailure.Broadcast(TestResultObj);
		}
	}
	AutomationControllerManager->OnTestsComplete().RemoveAll(this);
#else
	// No support for dev automation in this build.
	FAsyncRunAutomationTestResult NullResult;
	NullResult.TestName = ExecutingTestName;
	NullResult.bSucceeded = false;

	const FString MissingSupportError(TEXT("Project build config does not support automation framework."));
	UE_LOG(LogAsyncRunAutomationTest, Error, TEXT("%s"), *MissingSupportError);
	NullResult.ErrorStrings.Add(MissingSupportError);

	OnFailure.Broadcast(NullResult);
#endif // WITH_DEV_AUTOMATION_TESTS
}

#pragma optimize("", on)