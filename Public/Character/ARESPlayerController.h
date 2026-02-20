#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ARESPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS()
class AARESPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AARESPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

protected:
	// --- Enhanced Input assets (грузим из Content по пути) ---
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> FireAction;

	// --- Aim ---
	UPROPERTY(EditDefaultsOnly, Category="Aim")
	TEnumAsByte<ECollisionChannel> AimTraceChannel = ECC_Visibility;

	// --- Handlers ---
	void Input_Move(const struct FInputActionValue& Value);
	void Input_FireStarted(const struct FInputActionValue& Value);
	void Input_FireCompleted(const struct FInputActionValue& Value);

	void UpdateAimToMouse();
};