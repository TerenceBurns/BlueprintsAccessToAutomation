// Copyright 2021 - Trifolium Digital Limited
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncRunAutomationTest.generated.h"


/**
 * Test status information from the UAsyncRunAutomationTest results.
 */
USTRUCT(BlueprintType)
struct  FAsyncRunAutomationTestResult
{
	GENERATED_BODY()

public:
	/** Identifier for the test. This is the full name. */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Results")
	FString TestName;

	/** Did the test complete successfully? */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Results")
	bool bSucceeded = false;

	/** Did the test track any log messages? If so, this is what it tracked... */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Results")
	TArray<FString> LogStrings;

	/** Did the test track any warning messages? If so, this is what it tracked... */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Results")
	TArray<FString> WarningStrings;

	/** Did the test track any Error messages? If so, this is what it tracked... */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Results")
	TArray<FString> ErrorStrings;

};


/** Delegate used to relay the test completion. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsyncRunAutomationTestComplete, const FAsyncRunAutomationTestResult&, TestResultsObject);


/** 
 * Async action to run a test that is part of the Automation Framework 
 */
UCLASS()
class UAsyncRunAutomationTest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	/** Blueprint node specific constructor for our test node. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "Automation", WorldContext = "WorldContextObject"))
	static UAsyncRunAutomationTest* AsyncRunAutomationTest(UObject* WorldContextObject, const FString& TestName);
	
	/** Delegate called when the Automation test succeeds. */
	UPROPERTY(BlueprintAssignable)
	FOnAsyncRunAutomationTestComplete OnSuccess;

	/** Delegate called when the Automation test fails. */
	UPROPERTY(BlueprintAssignable)
	FOnAsyncRunAutomationTestComplete OnFailure;

	/** Execute the actual operation */
	virtual void Activate() override;

protected:
	// Keep track of the test we are running.
	FString ExecutingTestName;

	// Callback used to hook into the automation framework to relay test completion.
	void OnTestComplete() const;
};
