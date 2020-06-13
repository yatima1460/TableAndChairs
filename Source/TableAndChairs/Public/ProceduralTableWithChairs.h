#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Materials/Material.h"

#include "ProceduralChairWithBackrest.h"
#include "ProceduralBoxComponent.h"

#include "ProceduralTableWithChairs.generated.h"

UCLASS(Blueprintable)
class TABLEANDCHAIRS_API ATableActor : public AActor
{
	GENERATED_BODY()


public:

	//UPROPERTY still does not support constexpr

	static constexpr bool SHOULD_HAVE_COLLISION = false;
	static constexpr float TABLE_TOP_THICKNESS = 10.0f;
	static constexpr float LEG_LENGTH = 65;
	static constexpr float LEG_SIDE_SIZE = 10;
	static const FVector2D DEFAULT_SIZE;
	static constexpr float ANCHOR_SIZE = 25;
	static constexpr float ANCHOR_HOVER_DISTANCE = 2;

	/**
	Table min size is 1x1 meters
	*/
	static constexpr float TABLE_MIN_SIZE = 50.0f;

	/**
	Table max size is 50x50 meters
	*/
	static constexpr float TABLE_MAX_SIZE = 5000.0f;


private:

	/**
	The four table corners in clockwise order from the positive coordinates relative sector
	*/
	TArray<UProceduralBoxComponent*> Corners;

	/**
	The procedural mesh of the table countertop
	*/
	UProceduralBoxComponent* CounterTop;

	/**
	The four procedural legs
	*/
	TArray<UProceduralBoxComponent*> Legs;
	
	/**
	Cached TableSize
	The real tablesize is calculated as the distance between the corners
	*/
	FVector2D TableSize = DEFAULT_SIZE;

protected:

	virtual void OnConstruction(const FTransform& Transform) override;


public:

	ATableActor();

	UFUNCTION(BlueprintCallable, Category = "Archviz")
	bool SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewWorldLocation);

	/**
	Returns the opposite corner given a corner

	@Param CurrentCorner the input corner
	@Return opposite corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	UProceduralMeshComponent * GetOppositeCorner(const UProceduralMeshComponent * CurrentCorner) const;

	/**
	The size of the table

	@Return table size as FVector2D
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	FVector2D GetTableSize() const;

	/**
	Returns the total table height: legs + countertop
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	float GetTableHeight() const;

	/**
	Returns the four anchor corners of the table
	In clockwise order from the positive sector

	X+ Y+, X- Y+, X- Y-, X+ Y-
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	TArray<UProceduralBoxComponent*> GetCorners() const;

	/**
	Returns the clockwise corner given a corner

	@Param CurrentCorner the input corner
	@Return clockwise corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	UProceduralMeshComponent* GetClockwiseCorner(const UProceduralMeshComponent * const CurrentCorner) const;

	/**
	Returns the counterclockwise corner given a corner

	@Param CurrentCorner the input corner
	@Return counterclockwise corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "Archviz")
	UProceduralMeshComponent* GetCounterClockwiseCorner(const UProceduralMeshComponent* const CurrentCorner) const;

	/**
	Call this after editing one of the 4 corners locations to refresh vertexes locations
	*/
	UFUNCTION(BlueprintCallable, Category = "Archviz")
	void RefreshLocations();

	/**
	The material applied to the entire table, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Archviz")
	UMaterial* TableMaterial;
};
