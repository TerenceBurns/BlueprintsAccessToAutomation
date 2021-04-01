// Copyright 2021 - Trifolium Digital Limited
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintAsyncActionBase.h"

#include "AsyncFindAllAutomationTests.generated.h"


USTRUCT(BlueprintType)
struct  FAsyncFindAllAutomationTestsResult
{
	GENERATED_BODY()

public:
	/** Identifier for the test. This is the full name. */
	UPROPERTY(BlueprintReadOnly, Category = "AutomationTest|Available Tests")
	TArray<FString> AllTestNames;
};


/** Delegate used to notify that tests have been found. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsyncFindAllAutomationTests, const FAsyncFindAllAutomationTestsResult&, FindAllTestsResultObject);


/** 
 * Async action to find available tests that are part of the Automation Framework 
 */
UCLASS()
class UAsyncFindAllAutomationTests : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	/** Blueprint node specific constructor for our test node. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "Automation|Trifolium Digital", WorldContext = "WorldContextObject"))
	static UAsyncFindAllAutomationTests* AsyncFindAllAutomationTests(UObject* WorldContextObject);
	
	/** Delegate called when the Automation tests are found. */
	UPROPERTY(BlueprintAssignable)
	FOnAsyncFindAllAutomationTests OnTestNamesReady;

	/** Execute the actual operation */
	virtual void Activate() override;

protected:

	// Callback used to hook into the automation framework to relay test information back..
	void OnTestsRefreshed() const;
};
