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


/** Delegate used to relay the test completion. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAsyncFindAllAutomationTests, const FAsyncFindAllAutomationTestsResult&, FindAllTestsResultObject);


/** 
 * Async action to run a test that is part of the Automation Framework 
 */
UCLASS()
class UAsyncFindAllAutomationTests : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	/** Blueprint node specific constructor for our test node. */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", Category = "Automation|Trifolium Digital", WorldContext = "WorldContextObject"))
	static UAsyncFindAllAutomationTests* AsyncFindAllAutomationTests(UObject* WorldContextObject);
	
	/** Delegate called when the Automation test succeeds. */
	UPROPERTY(BlueprintAssignable)
	FOnAsyncFindAllAutomationTests OnTestNamesReady;

	/** Execute the actual operation */
	virtual void Activate() override;

protected:

	// Callback used to hook into the automation framework to relay test information bacl..
	void OnTestsRefreshed() const;
};
