#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FramePackage.h"
#include "TimeReverseComponent.generated.h"

/** Blueprintable decorator is added to allow creating blueprint instances of this component. */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class TIMEREVERSAL_API UTimeReverseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	//Constructor
	UTimeReverseComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** Reversing time, when true, collecting data when false. */
	bool bReversingTime;

	/** When we run out of saved reverse - time data, ( out of time, cannot keep reversing ). */
	bool bOutOfData;

	/** Actual time since we started reversing time. */
	float Runningtime;

	/** Running count of the FFramePackage Delta Times. */
	float PreviousReverseTimeFrame;
	float NextReverseTimeFrame;

	/** Total amount of time recorded in FramePackages. */
	float RecordedTime;

	/** The maximum amount to record time. */
	UPROPERTY( EditDefaultsOnly, Category = "Time Recording", meta = ( ClampMin = "0.0", ClampMax = "10.0", UIMin = "0.0", UIMax = "10.0" ))
	float MaxRecordedTime;
	
	/** A list of frame packages */
	TDoubleLinkedList<FFramePackage> StoredFramesPackage;

	/** The value to multiply the Delta Time by. 1 is normal reverse time and < 1 slower reverse time to create slow motion feel. */
	UPROPERTY( EditAnywhere, Category = "Slowmotion", meta = ( ClampMin = "0.01", ClampMax = "1.0", UIMin = "0.01", UIMax = "1.0" ) )
	float SlowMotionMultiplier;
	
	/** Any function that we bind to a delegate MUST be a UFUNCTION and MUST be called in BeginPlay */
	UFUNCTION()
	void SetTimeReverse( bool InReversingTime );
	void SetReversedActorInfo( FVector ActorLocation, FRotator ActorRotation, FVector ActorLinearVelocity, FVector ActorAngularVelocity );
};
