#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.generated.h"

UCLASS(Blueprintable)
class TABLEANDCHAIRS_API UProceduralBoxComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	UProceduralBoxComponent();

	UFUNCTION(BlueprintCallable, Category="Archviz")
	void Build(const FVector Size, bool CollisionEnabled);
	
	void Build(const FVector Size, bool CollisionEnabled, const TArray<FLinearColor> LinearColors, const TArray<FProcMeshTangent> MeshTangents);

	/**
	Sets the specified material to all box faces
	*/
	UFUNCTION(BlueprintCallable, Category = "Archviz")
	void SetBoxMaterial(const UMaterialInterface* const NewMaterial);
};
