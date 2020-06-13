#include "TableAndChairsGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

ATableAndChairsGameModeBase::ATableAndChairsGameModeBase() {
	PlayerControllerClass = ATableAndChairsPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
}

void ATableAndChairsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Targets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATargetPoint::StaticClass(), Targets);

	ATableActor* Table = GetWorld()->SpawnActor<ATableActor>();
	Table->SetActorLocation(Targets[0]->GetActorLocation());
}