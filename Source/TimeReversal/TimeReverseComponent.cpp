#include "TimeReverseComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimeReversalCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "FramePackage.h"
#include "Engine/Engine.h"

UTimeReverseComponent::UTimeReverseComponent() :
	bReversingTime( false ),
	MaxRecordedTime( 5.0f ),
	SlowMotionMultiplier( 1.0f )
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTimeReverseComponent::BeginPlay()
{
	Super::BeginPlay();

	ATimeReversalCharacter* Character = Cast<ATimeReversalCharacter>( UGameplayStatics::GetPlayerPawn( GetWorld(), 0 ) );
	Character->TimeReverseDelegate.AddDynamic( this, &UTimeReverseComponent::SetTimeReverse );
}

// Called every frame
void UTimeReverseComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
	if( GEngine )
	{
		GEngine->AddOnScreenDebugMessage( 1, 1.f, FColor::Green, FString::Printf( TEXT( "Time Recorded: %.2lf" ), RecordedTime ) );
	}

	if( !bReversingTime )
	{
		Runningtime = 0.f;
		PreviousReverseTimeFrame = 0.f;
		NextReverseTimeFrame = 0.f;

		AActor* OwningActor = GetOwner();
		 
		 /** 4.24 and below
		   * TArray<UActorComponent*> Components = OwningActor->GetComponentsByClass( UStaticMeshComponent::StaticClass() );
		   */

		 /** 4.25 and above */
		TInlineComponentArray<UActorComponent*> Components;
		OwningActor->GetComponents( UStaticMeshComponent::StaticClass(), Components );

		if( Components.Num() > 0 )
		{
			UStaticMeshComponent* ActorMesh = Cast<UStaticMeshComponent>( Components[ 0 ] );
			if( ActorMesh )
			{
				FFramePackage Package( OwningActor->GetActorLocation(),OwningActor->GetActorRotation(), ActorMesh->GetPhysicsLinearVelocity(), ActorMesh->GetPhysicsAngularVelocityInDegrees(), DeltaTime );
				if( RecordedTime < MaxRecordedTime )
				{
					StoredFramesPackage.AddTail( Package );
					RecordedTime += Package.DeltaTime;
					bOutOfData = false;
				}
				else
				{
					while( RecordedTime >= MaxRecordedTime )
					{
						TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* FirstFramePackage = StoredFramesPackage.GetHead();
						float FirstPackageDeltaTime = FirstFramePackage->GetValue().DeltaTime;
						StoredFramesPackage.RemoveNode( FirstFramePackage );
						RecordedTime -= FirstPackageDeltaTime;
					}
					StoredFramesPackage.AddTail( Package );
					RecordedTime += Package.DeltaTime;
					bOutOfData = false;
				}
			}
		}
	}
	else if( !bOutOfData ) // Enter time reversal
	{
		Runningtime += DeltaTime * SlowMotionMultiplier;
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* PreviousFramePackage = StoredFramesPackage.GetTail();
		TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* NextFramePackage = PreviousFramePackage->GetPrevNode();
		NextReverseTimeFrame = PreviousReverseTimeFrame + PreviousFramePackage->GetValue().DeltaTime;

		while( Runningtime > NextReverseTimeFrame )
		{
			PreviousReverseTimeFrame += PreviousFramePackage->GetValue().DeltaTime;
			PreviousFramePackage = NextFramePackage;
			NextReverseTimeFrame += NextFramePackage->GetValue().DeltaTime;
			NextFramePackage = NextFramePackage->GetPrevNode();

			TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* FrameTail = StoredFramesPackage.GetTail();
			RecordedTime -= FrameTail->GetValue().DeltaTime;
			StoredFramesPackage.RemoveNode( FrameTail );
			if( NextFramePackage == StoredFramesPackage.GetHead() )
			{
				bOutOfData = true;
			}
		}
		if( Runningtime <= NextReverseTimeFrame && Runningtime >= PreviousReverseTimeFrame )
		{
			float DeltaBetweenCurrentAndNextRunningTime = Runningtime - PreviousReverseTimeFrame;
			float Interval = NextReverseTimeFrame - PreviousReverseTimeFrame;
			float Fraction = DeltaBetweenCurrentAndNextRunningTime / Interval;

			FVector LocationInterpolation = FMath::VInterpTo( PreviousFramePackage->GetValue().Location, NextFramePackage->GetValue().Location, Fraction, 1.0f );
			FRotator RotationInterpolcation = FMath::RInterpTo( PreviousFramePackage->GetValue().Rotation, NextFramePackage->GetValue().Rotation, Fraction, 1.0f );
			FVector LinearVelocityInterpolation = FMath::VInterpTo( PreviousFramePackage->GetValue().LinearVelocity, NextFramePackage->GetValue().LinearVelocity, Fraction, 1.0f );
			FVector AngularVelocityInterpolation = FMath::VInterpTo( PreviousFramePackage->GetValue().AngularVelocity, NextFramePackage->GetValue().AngularVelocity, Fraction, 1.0f );

			SetReversedActorInfo( LocationInterpolation, RotationInterpolcation, LinearVelocityInterpolation, AngularVelocityInterpolation );
		}
	}
}

void UTimeReverseComponent::SetTimeReverse( bool InReversingTime )
{
	bReversingTime = InReversingTime;	
}

void UTimeReverseComponent::SetReversedActorInfo( FVector ActorLocation, FRotator ActorRotation, FVector ActorLinearVelocity, FVector ActorAngularVelocity )
{
	AActor* OwningActor = GetOwner();
	OwningActor->SetActorLocation( ActorLocation );
	OwningActor->SetActorRotation( ActorRotation );

	TInlineComponentArray<UActorComponent*> ActorComponents;
	OwningActor->GetComponents( UStaticMeshComponent::StaticClass(), ActorComponents );

	if( ActorComponents.Num() > 0 )
	{
		UStaticMeshComponent* ActorMeshcomponent = Cast<UStaticMeshComponent>( ActorComponents[ 0 ] );
		if( ActorMeshcomponent )
		{
			ActorMeshcomponent->SetPhysicsLinearVelocity( ActorLinearVelocity );
			ActorMeshcomponent->SetPhysicsAngularVelocityInDegrees( ActorAngularVelocity );
		}
	}
}
