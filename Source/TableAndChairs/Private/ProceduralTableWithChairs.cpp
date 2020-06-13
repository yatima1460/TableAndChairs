#include "ProceduralTableWithChairs.h"
#include "LogArchviz.h"

#ifdef UE_BUILD_DEVELOPMENT
#	include "Engine.h"
#endif

#include "ConstructorHelpers.h"


const FVector2D ATableActor::DEFAULT_SIZE = FVector2D(100, 200);

ATableActor::ATableActor()
{
	// Turn the table tick off, we don't need it
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Create resizable corners
	Corners = {
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XPositiveYPositive")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XNegativeYPositive")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XNegativeYNegative")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XPositiveYNegative"))
	};
	for (size_t i = 0; i < Corners.Num(); i++)
	{
		Corners[i]->SetupAttachment(RootComponent);
		Corners[i]->Build(FVector(ANCHOR_SIZE, ANCHOR_SIZE, 1), true);
		Corners[i]->ContainsPhysicsTriMeshData(true);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialTileAnchor(TEXT("Material'/Game/Materials/M_TileAnchor.M_TileAnchor'"));
	if (MaterialTileAnchor.Succeeded())
	{
		for (size_t i = 0; i < Corners.Num(); i++)
			Corners[i]->SetBoxMaterial(MaterialTileAnchor.Object);
		UE_LOG(LogArchviz, Log, TEXT("Resize anchor material loaded"));
	}
	else
	{
		UE_LOG(LogArchviz, Error, TEXT("Resize anchor material failed loading"));
	}


	Corners[0]->SetRelativeLocation(FVector( TableSize.X / 2,  TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[1]->SetRelativeLocation(FVector(-TableSize.X / 2,  TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[2]->SetRelativeLocation(FVector(-TableSize.X / 2, -TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[3]->SetRelativeLocation(FVector( TableSize.X / 2, -TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));


	// Load table material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/xoio_berlinflat/Materials/wood_chair.wood_chair'"));
	if (Material.Succeeded())
	{
		TableMaterial = Material.Object;
		UE_LOG(LogArchviz, Log, TEXT("Table material loaded"));
	}
	else
	{
		UE_LOG(LogArchviz, Error, TEXT("Table material failed loading"));
	}

	// Create countertop
	CounterTop = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("Countertop"));
	CounterTop->SetupAttachment(RootComponent);
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS), false);
	CounterTop->SetBoxMaterial(TableMaterial);

	// Create legs
	for (size_t i = 0; i < Corners.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(LEG_SIDE_SIZE, LEG_SIDE_SIZE, LEG_LENGTH), false);
		LegComp->SetupAttachment(RootComponent);
		LegComp->SetRelativeLocation(FVector::ZeroVector);
		LegComp->SetBoxMaterial(TableMaterial);
		Legs.Add(LegComp);
	}
}

bool ATableActor::SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewCornerWorldLocation)
{
	if (Corner == nullptr)
		return false;

	const FVector OldLocation = Corner->GetComponentLocation();
	const UProceduralMeshComponent* OppositeCorner = GetOppositeCorner(Corner);
	const FVector OldSign = (Corner->GetRelativeTransform().GetLocation() - OppositeCorner->GetRelativeTransform().GetLocation()).GetSignVector();


	// The current corner location is where the mouse ray hits the table plane
	Corner->SetWorldLocation(NewCornerWorldLocation);

	// The nearby corners location depends if they are the clockwise/counterclockwise corners and in which sector of the table they are
	UProceduralMeshComponent* ClockwiseCorner = GetClockwiseCorner(Corner);
	ensureMsgf(ClockwiseCorner != GetOppositeCorner(Corner), TEXT("A clockwise corner can't be the opposite corner"));
	UProceduralMeshComponent* CounterClockwiseCorner = GetCounterClockwiseCorner(Corner);
	ensureMsgf(CounterClockwiseCorner != GetOppositeCorner(Corner), TEXT("A counterclockwise corner can't be the opposite corner"));
	const FVector NewSign = (Corner->GetRelativeTransform().GetLocation() - OppositeCorner->GetRelativeTransform().GetLocation()).GetSignVector();

	// HACK: fix this ugly switch
	// If a corner is in a quadrant in which both coordinates have the same sign it should assign the Y on the clockwise corner and the X on the counterclockwise corner
	// If a corner is in a quadrant in which both coordinates have a different sign it should assign the X on the clockwise corner and the Y on the counterclockwise corner
	UProceduralMeshComponent* Corner1;
	UProceduralMeshComponent* Corner2;

	auto CornerIndex = Corners.IndexOfByKey(Corner);

	switch (CornerIndex)
	{
	case 0:
	case 2:
		Corner1 = ClockwiseCorner;
		Corner2 = CounterClockwiseCorner;
		break;
	case 1:
	case 3:
		Corner1 = CounterClockwiseCorner;
		Corner2 = ClockwiseCorner;
		break;
	default:
		Corner1 = nullptr;
		Corner2 = nullptr;
	}
	const auto Corner1OldLoc = Corner1->GetComponentLocation();
	const auto Corner2OldLoc = Corner2->GetComponentLocation();
	Corner1->SetWorldLocation(FVector(Corner1->GetComponentLocation().X, NewCornerWorldLocation.Y, Corner1->GetComponentLocation().Z));
	Corner2->SetWorldLocation(FVector(NewCornerWorldLocation.X, Corner2->GetComponentLocation().Y, Corner2->GetComponentLocation().Z));

	// Minimum table area check
	// if corner in other relative sector or distance with relative (0,0) too small abort the resize
	if (FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation()) < ATableActor::TABLE_MIN_SIZE ||
		FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation()) < ATableActor::TABLE_MIN_SIZE ||
		FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation()) > ATableActor::TABLE_MAX_SIZE ||
		FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation()) > ATableActor::TABLE_MAX_SIZE ||
		OldSign != NewSign
		)
	{
		Corner->SetWorldLocation(OldLocation);
		Corner1->SetWorldLocation(Corner1OldLoc);
		Corner2->SetWorldLocation(Corner2OldLoc);
		return false;
	}
	// TODO: support table rotations

	// Update the table vertexes
	RefreshLocations();

	return true;
}


FVector2D ATableActor::GetTableSize() const
{
	return TableSize;
}


void ATableActor::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	RefreshLocations();
}


float ATableActor::GetTableHeight() const
{
	return LEG_LENGTH + TABLE_TOP_THICKNESS;
}


TArray<UProceduralBoxComponent*> ATableActor::GetCorners() const
{
	return Corners;
}


UProceduralMeshComponent * ATableActor::GetOppositeCorner(const UProceduralMeshComponent * CurrentCorner) const
{
	// HACK: fix this with a lookup table maybe
	int index = Corners.IndexOfByKey(CurrentCorner);

	switch (index)
	{
		case 0:  return Corners[2];
		case 1:  return Corners[3];
		case 2:  return Corners[0];
		case 3:  return Corners[1];
		default: return nullptr;
	}
}

UProceduralMeshComponent* ATableActor::GetClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	// HACK: fix this with a lookup table maybe
	int index = Corners.IndexOfByKey(CurrentCorner);
	//return Corners[(index+1) % 4];
	switch (index)
	{
		case 0:  return Corners[1];
		case 1:  return Corners[2];
		case 2:  return Corners[3];
		case 3:  return Corners[0];
		default: return nullptr;
	}
}

UProceduralMeshComponent* ATableActor::GetCounterClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	// HACK: fix this with a lookup table maybe
	int index = Corners.IndexOfByKey(CurrentCorner);
	//return Corners[(index+1) % 4];
	switch (index)
	{
		case 0:  return Corners[3];
		case 1:  return Corners[0];
		case 2:  return Corners[1];
		case 3:  return Corners[2];
		default: return nullptr;
	}
}

void ATableActor::RefreshLocations()
{
	// Cache table size
	TableSize = FVector2D(
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[1]->GetComponentLocation()),
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[3]->GetComponentLocation())
	);

	

	// HACK: fix root location
	FVector NewRelativeRoot = FVector(
		Corners[0]->GetRelativeTransform().GetLocation().X - TableSize.X / 2,
		Corners[0]->GetRelativeTransform().GetLocation().Y - TableSize.Y / 2,
		0
	);
	FVector NewWorldRoot = this->GetTransform().TransformPosition(NewRelativeRoot);

	// Refresh countertop geometry
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS), false);
	CounterTop->SetWorldLocation(NewWorldRoot+FVector(0,0, LEG_LENGTH  + TABLE_TOP_THICKNESS/2));

	// Refresh legs location
	const TArray<FVector> LegsOffsets = {
	{-LEG_SIDE_SIZE / 2, -LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{ LEG_SIDE_SIZE / 2, -LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{LEG_SIDE_SIZE / 2, LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{-LEG_SIDE_SIZE / 2, LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS} };
	for (size_t i = 0; i < 4; i++)
		Legs[i]->SetRelativeLocation(Corners[i]->GetRelativeTransform().GetLocation() + LegsOffsets[i]);

	// Remove old chairs
	TArray<AActor*> SpawnedChairs;
	this->GetAllChildActors(SpawnedChairs);
	for (size_t i = 0; i < SpawnedChairs.Num(); i++)
		SpawnedChairs[i]->Destroy();

	// Spawn necessary chairs
	constexpr float CHAIRS_DISTANCE_FROM_TABLE = 25;
	constexpr float DISTANCE_BETWEEN_CHAIRS = 50;
	constexpr float CHAIRS_INTERVAL = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS);

	

	int ChairsToSpawnOnYSide = TableSize.Y / CHAIRS_INTERVAL;
	int ChairsToSpawnOnXSide = TableSize.X / CHAIRS_INTERVAL;

	// The offset to add so the chairs line is centered
	// Remaining space / 2
	const float CHAIRS_YLINE_LENGTH = ChairsToSpawnOnYSide * AProceduralChair::CHAIR_SQUARE_SIZE + (ChairsToSpawnOnYSide-1)*DISTANCE_BETWEEN_CHAIRS;
	const float YChairsSpawnOffset = (TableSize.Y - CHAIRS_YLINE_LENGTH ) / 2;
	

	// Backward side
	FVector ChairsForwardSpawnPoint = Corners[2]->GetComponentLocation();
	ChairsForwardSpawnPoint.X -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsForwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE/2;
	ChairsForwardSpawnPoint.Z = this->GetActorLocation().Z;
	
	for (int y = 0; y < ChairsToSpawnOnYSide; y++)
	{	
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(ChairsForwardSpawnPoint.X, YChairsSpawnOffset + ChairsForwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
	}

	// Forward side
	FVector ChairsBackwardSpawnPoint = Corners[3]->GetComponentLocation();
	ChairsBackwardSpawnPoint.X += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsBackwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE/2;
	ChairsBackwardSpawnPoint.Z = this->GetActorLocation().Z;
	for (int y = 0; y < ChairsToSpawnOnYSide; y++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(ChairsBackwardSpawnPoint.X, YChairsSpawnOffset + ChairsBackwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, -180, 0));
	}

	const float CHAIRS_XLINE_LENGTH = ChairsToSpawnOnXSide * AProceduralChair::CHAIR_SQUARE_SIZE + (ChairsToSpawnOnXSide - 1)*DISTANCE_BETWEEN_CHAIRS;
	const float XChairsSpawnOffset = (TableSize.X - CHAIRS_XLINE_LENGTH) / 2;

	// Left side
	FVector ChairsLeftSpawnPoint = Corners[2]->GetComponentLocation();
	ChairsLeftSpawnPoint.Y -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsLeftSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;
	ChairsBackwardSpawnPoint.Z = this->GetActorLocation().Z;
	for (int x = 0; x < ChairsToSpawnOnXSide; x++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(XChairsSpawnOffset + ChairsLeftSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsLeftSpawnPoint.Y, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, 90, 0));
	}

	// Right side
	FVector ChairsRightSpawnPoint = Corners[1]->GetComponentLocation();
	ChairsRightSpawnPoint.Y += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsRightSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;
	ChairsRightSpawnPoint.Z = this->GetActorLocation().Z;
	for (int x = 0; x < ChairsToSpawnOnXSide; x++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(XChairsSpawnOffset + ChairsRightSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsRightSpawnPoint.Y, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, -90, 0));
	}

	// Register the chairs
	RegisterAllComponents();
}
