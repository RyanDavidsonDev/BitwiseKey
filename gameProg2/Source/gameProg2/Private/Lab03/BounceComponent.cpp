// Fill out your copyright notice in the Description page of Project Settings.


#include "Lab03/BounceComponent.h"

// Sets default values for this component's properties
UBounceComponent::UBounceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBounceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBounceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector newPos = GetOwner()->GetActorLocation();
	newPos.Y += FMath::Sin(GetWorld()->GetTimeSeconds() * bounceSpeed) * 30;
	GetOwner()->SetActorLocation(newPos);
	// ...
}

