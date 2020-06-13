#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralChairWithBackrest.generated.h"

UCLASS(Blueprintable)
class TABLEANDCHAIRS_API AProceduralChair : public AActor
{
	GENERATED_BODY()

	
public:	

	AProceduralChair();

	static constexpr float CHAIR_SQUARE_SIZE = 30;
	static constexpr float CHAIR_SQUARE_THICKNESS = 2;

	static constexpr float CHAIR_LEG_HEIGHT = 45;
	static constexpr float CHAIR_LEG_SIZE = 4;

	static constexpr float CHAIR_BACKREST_HEIGHT = 60;
	static constexpr float CHAIR_BACKREST_THICKNESS = 2;


public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Archviz")
	UProceduralMeshComponent* ChairMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Archviz")
	UMaterial* Material;
};
