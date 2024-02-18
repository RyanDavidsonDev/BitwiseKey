// Fill out your copyright notice in the Description page of Project Settings.

#include "BitwiseGameMode.h"
#include "Interactables/ASpawnPowerup.h"
#include "BitwiseGameState.h"
#include "CollectionInteractable.h"
#include "Interactables/XRayVision.h"  
#include "Interactables/InvisibilityPowerup.h"
#include "PlayerCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "BitwiseKey/BitwiseKey.h"
#include "Components/AudioComponent.h" 
#include "Kismet/GameplayStatics.h" 
#include "TimerManager.h"
#include "Kismet/KismetArrayLibrary.h"
#include "UObject/ConstructorHelpers.h" 
#include "BitwiseGameState.h"


ABitwiseGameMode::ABitwiseGameMode()
{

    /** initialize some of the defaults, blueprinting this will override it,
        notice the use of StaticClass to get the UClass class type properly */
    DefaultPawnClass = APlayerCharacter::StaticClass();
    GameStateClass = ABitwiseGameState::StaticClass();


    //static ConstructorHelpers::FClassFinder<UStaticMesh> AssetFile(TEXT("/Game/Blueprints/XRayActor.XRayActor"));
    //if (AssetFile.Class != nullptr) {
    //    WARN("we're so fucking back")
    //    XRayCollectible = AssetFile.Class;

    //}
    //else {
    //    WARN("it's over")
    //}


    //auto xray = ConstructorHelpers::FObjectFinder<AActor>(TEXT("/Game/Blueprints/XRayActor.XRayActor"));
    //if(xray.Succeeded()){
    //    XRayCollectible->
}

void ABitwiseGameMode::BeginPlay()
{

    Super::BeginPlay();
    DispatchBeginPlay();

    playerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPowerup::StaticClass(), PowerupSpawnLocations);

    gs = GetWorld()->GetGameState<ABitwiseGameState>();

    //PlaceCollectibleArray();

    //should probably make a new helper function buuuuuuut

    GetWorldTimerManager().SetTimer(InvisibilityStruct.RechargeTimerHandle, this, &ABitwiseGameMode::UpdateInvisCharge, 1/InvisibilityStruct.Precision, true, 2.0f);


    D_OnReset.AddDynamic(this, &ABitwiseGameMode::ResetGameMode);
}



void ABitwiseGameMode::Tick(float DeltaSeconds)
{
    if (IsValid(playerCharacter)) {

        if (playerCharacter->bReceivedFirstPlayerInput) {
            //StartGameTimer();
            gs->gameTimer = GetWorld()->GetUnpausedTimeSeconds() - GameStartTime;
        }
    } else {
        UE_LOG(LogTemp, Error, TEXT("Player character reference in gamemode NOT valid"));
    }
}

void ABitwiseGameMode::StartGameTimer()
{
    GameStartTime = GetWorld()->GetUnpausedTimeSeconds();
    
}

void ABitwiseGameMode::ResetGameMode()
{

    gs->gameTimer = 0;



    //reset ability collection
        
    for (TPair<EPowerUpName, FPowerupStruct> pair : gs->PowerupMap) {
        pair.Value.bCollected = false;
        pair.Value.bIsActive = false;
    }

    InvisibilityStruct.currentCharge = 0;
    StaminaStruct.currentCharge = 0;

    //DEPRECATED: before I refactored powerups to structs I went through and reset each one individually (gross)
    //gs->PowerupMap.Find(EPowerUpName::PE_XRay)->bCollected = false;
    //gs->bHasSpeedBoost = false;
    //gs->bHasJumpBoost = false;
    //gs->bHasInvisibility = false;
    ////make sure that invisibility is reset beyond just collection
    //gs->bPlayerIsInvisible = false;
    //gs->CurrentInvisCharge = 0;

    //DEPRECEATED: before I refactored randomization to be based on individual objects, I'd originally tracked all 
    // the powerups within this game mode. the following comment destroyed their instances. this is now done
    // in RandomizerBase
    //destroy each spawned powerup and remove it from the TMap
    //for ( const auto& Pair : SpawnedCollectiblesMap) {
    //    LOG("PING")
    //    AActor* PowerUpActor = Pair.Value;
    //    PowerUpActor->Destroy();
    //    //SpawnedCollectiblesMap.Remove(Pair.Key);
    //}


    StartGameTimer();
}

void ABitwiseGameMode::CollectXRay()
{
    LOG("collect x ray method called from gamemode");
    gs->XRayStruct.bCollected = true;
    //used to notify all xray objects to update their state
    OnCollectedXray.Broadcast();
}

void ABitwiseGameMode::CollectInvisibility()
{
    //SpawnedCollectiblesMap.FindAndRemoveChecked(EPowerUp::PE_Invisibility);
    LOG("collect x ray method called from gamemode");
    gs->InvisibilityStruct.bCollected = true;

}

void ABitwiseGameMode::CollectSpeedBoost()
{
    gs->SpeedBoostStruct.bCollected = true;
}

void ABitwiseGameMode::CollectJumpBoost()
{
    gs->JumpBoostStruct.bCollected = true;

}



void ABitwiseGameMode::ToggleInvisibility()
{

    if (gs->InvisibilityStruct.bCollected) {

        if (gs->InvisibilityStruct.bIsActive) {

            gs->InvisibilityStruct.bIsActive = false;
            UGameplayStatics::PlaySound2D(GetWorld(), SW_InvisDeactivate);


        }
        else {
            gs->InvisibilityStruct.bCollected = true;
            UGameplayStatics::PlaySound2D(GetWorld(), SW_InvisActivate);

        }



    }
    else {
        WARN("You do not yet have that ability");
    }
}

void ABitwiseGameMode::UpdateInvisCharge()
{
    if (gs->InvisibilityStruct.bCollected) {

        //LOG("CurrentCharge: %f", gs->CurrentInvisCharge)
        if (gs->InvisibilityStruct.bIsActive) { //invisibility active, counting down

            InvisibilityStruct.currentCharge = FMath::Clamp(
                InvisibilityStruct.currentCharge - InvisibilityStruct.DischargeRate,
                0.0, InvisibilityStruct.MaxCharge);

            if (InvisibilityStruct.currentCharge == 0) {
                ToggleInvisibility();
                return;
            }

            //DEPRECATED: the (obviously very messy) way I'd been clamping the values originally
            //if (InvisibilityStruct.currentCharge == 0) {
            //    ToggleInvisibility();
            //    return;
            //}
            //else  if (InvisibilityStruct.currentCharge < 0) {
            //    InvisibilityStruct.currentCharge = 0;
            //    return;
            //}
            //else if (InvisibilityStruct.currentCharge > 0) {
            //    InvisibilityStruct.currentCharge -= InvisibilityStruct.DischargeRate / InvisibilityStruct.Precision;
            //    return;
            //}
        } else { //invisibility inactive, counting up

            InvisibilityStruct.currentCharge = FMath::Clamp(
                InvisibilityStruct.currentCharge + InvisibilityStruct.ChargeRate,
                0.0, InvisibilityStruct.MaxCharge);
                
            //DEPRECATED: the (obviously very messy) way I'd been clamping the values originally
            //if (InvisibilityStruct.currentCharge == InvisibilityStruct.MaxCharge) {
            //    return;
            //}
            //else  if (InvisibilityStruct.currentCharge > InvisibilityStruct.MaxCharge) {
            //    InvisibilityStruct.currentCharge = InvisibilityStruct.MaxCharge;
            //    return;
            //}
            //else if (gs->CurrentInvisCharge < InvisMaxCharge) {
            //    gs->CurrentInvisCharge += InvisChargeRate/ProgressBarPrecision;
            //    return;
            //}
        }
    }
    return ;
}

void ABitwiseGameMode::UpdateStamina()
{
    if (gs->bHasStaminaAbility) {
        if (gs->bPlayerIsUsingStamina) { //count down

            //decrease the value, setting it no lower than 0 and no higher than the max
            StaminaStruct.currentCharge = FMath::Clamp(
                StaminaStruct.currentCharge - StaminaStruct.DischargeRate,
                0.0, StaminaStruct.MaxCharge);

            if (StaminaStruct.currentCharge == 0) {
                playerCharacter->DeactivateStaminaEffects(); //tell the player to deactivate stamina
                return;
            }

            //DEPRECATED
            //if (gs->CurrentStamina == 0) {
            //    ToggleStamina();
            //    return;
            //}
            //else  if (gs->CurrentStamina < 0) {
            //    gs->CurrentStamina = 0;
            //    return;
            //}
            //else if (gs->CurrentStamina > 0) {

            //    gs->CurrentStamina -= StaminaDischargeRate / ProgressBarPrecision;
            //    return;
            //}
        }
        else { //invisibility inactive, counting up

            //increase stamina, setting it no lower than 0 and no higher than the max
            StaminaStruct.currentCharge = FMath::Clamp(
                StaminaStruct.currentCharge + StaminaStruct.ChargeRate,
                0.0, StaminaStruct.MaxCharge);


            //if (gs->CurrentStamina == InvisMaxCharge) {
            //    return;
            //}
            //else  if (gs->CurrentStamina > InvisMaxCharge) {
            //    gs->CurrentStamina = InvisMaxCharge;
            //    return;
            //}
            //else if (gs->CurrentStamina < InvisMaxCharge) {
            //    gs->CurrentStamina += InvisChargeRate / ProgressBarPrecision;
            //    return;
            //}
        }
    }
}



