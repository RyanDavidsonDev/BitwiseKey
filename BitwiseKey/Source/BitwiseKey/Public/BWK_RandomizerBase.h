// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "./Interactables/RandomItemSpawner.h"
#include "BWK_RandomizerBase.generated.h"

class AActorComponent;
class RandomItemSpawner;



UCLASS()
class BITWISEKEY_API ABWK_RandomizerBase : public AActor
{
	GENERATED_BODY()
	
// -- FUNCTIONS

public:	
	// Sets default values for this actor's properties
	ABWK_RandomizerBase();

	virtual void RandomizeOrder();
	virtual void PlaceItems();
	virtual ~ABWK_RandomizerBase() noexcept = default;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

// -- VARIABLES-- 

protected:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Randomization")
	TArray<TSubclassOf<AActor>> ItemArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, category = "Randomization")
	TArray<ARandomItemSpawner*> SpawnerArray;

public:
	UPROPERTY(EditAnywhere, Category = "Randomization")
	TArray<TSubclassOf<AActor>> ItemArrayPlease;

};
