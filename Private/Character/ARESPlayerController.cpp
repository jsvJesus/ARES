#include "Character/ARESPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

AARESPlayerController::AARESPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
	
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC(
			TEXT("/Game/Main/Input/IMC_ARES.IMC_ARES")
		);
		if (IMC.Succeeded()) MappingContext = IMC.Object;
		else UE_LOG(LogTemplateCharacter, Error, TEXT("IMC_ARES not found: /Game/Main/Input/IMC_ARES.IMC_ARES"));
	}
	
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> IA_Move(
			TEXT("/Game/Main/Input/Actions/IA_Move.IA_Move")
		);
		if (IA_Move.Succeeded()) MoveAction = IA_Move.Object;
		else UE_LOG(LogTemplateCharacter, Error, TEXT("IA_Move not found: /Game/Main/Input/Actions/IA_Move.IA_Move"));
	}
	
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> IA_Fire(
			TEXT("/Game/Main/Input/Actions/IA_Fire.IA_Fire")
		);
		if (IA_Fire.Succeeded()) FireAction = IA_Fire.Object;
		else UE_LOG(LogTemplateCharacter, Error, TEXT("IA_Fire not found: /Game/Main/Input/Actions/IA_Fire.IA_Fire"));
	}
}

void AARESPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (MappingContext)
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
	
	FInputModeGameOnly Mode;
	SetInputMode(Mode);
}

void AARESPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("EnhancedInputComponent missing."));
		return;
	}

	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AARESPlayerController::Input_Move);
	}

	if (FireAction)
	{
		EIC->BindAction(FireAction, ETriggerEvent::Started,   this, &AARESPlayerController::Input_FireStarted);
		EIC->BindAction(FireAction, ETriggerEvent::Completed, this, &AARESPlayerController::Input_FireCompleted);
		EIC->BindAction(FireAction, ETriggerEvent::Canceled,  this, &AARESPlayerController::Input_FireCompleted);
	}
}

void AARESPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateAimToMouse();
}

void AARESPlayerController::Input_Move(const struct FInputActionValue& Value)
{
	APawn* P = GetPawn();
	if (!P) return;

	const FVector2D Axis = Value.Get<FVector2D>();

	// движение по осям экрана/мира, НЕ относительно поворота персонажа
	if (!Axis.IsNearlyZero())
	{
		P->AddMovementInput(FVector::ForwardVector, Axis.Y);
		P->AddMovementInput(FVector::RightVector,   Axis.X);
	}
}

void AARESPlayerController::Input_FireStarted(const struct FInputActionValue& Value)
{
	// Пока заглушка
	// UE_LOG(LogTemplateCharacter, Log, TEXT("Fire Started"));
}

void AARESPlayerController::Input_FireCompleted(const struct FInputActionValue& Value)
{
	// Пока заглушка
	// UE_LOG(LogTemplateCharacter, Log, TEXT("Fire Stopped"));
}

void AARESPlayerController::UpdateAimToMouse()
{
	APawn* P = GetPawn();
	if (!P) return;

	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(AimTraceChannel, true, Hit);
	if (!bHit) return;

	const FVector PawnLoc = P->GetActorLocation();
	const FVector AimLoc  = Hit.ImpactPoint;

	FVector Dir = (AimLoc - PawnLoc);
	Dir.Z = 0.f; // только по плоскости
	if (Dir.IsNearlyZero()) return;

	FRotator NewRot = Dir.Rotation();
	NewRot.Pitch = 0.f;
	NewRot.Roll  = 0.f;

	P->SetActorRotation(NewRot);
}
