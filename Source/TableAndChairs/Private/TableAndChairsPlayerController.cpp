#include "TableAndChairsPlayerController.h"
#include "LogArchviz.h"
#include "Camera/CameraActor.h"


ATableAndChairsPlayerController::ATableAndChairsPlayerController()
{
	this->bShowMouseCursor = true;

	// Updating the drag and drop every 50ms is more than enough
	SetActorTickInterval(0.05);
}

void ATableAndChairsPlayerController::ExitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ATableAndChairsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Bind the functions for clicking with the cursor to edit the table
	ensureMsgf(InputComponent != nullptr, TEXT("ATableAndChairsPlayerController can't bind mouse events because InputComponent is nullptr"));
	InputComponent->BindAction("MouseLeftClicked", IE_Pressed, this, &ATableAndChairsPlayerController::LeftClickPressed);
	InputComponent->BindAction("MouseLeftClicked", IE_Released, this, &ATableAndChairsPlayerController::LeftClickReleased);
	InputComponent->BindAction("Escape", IE_Pressed, this, &ATableAndChairsPlayerController::ExitGame);

	// High quality settings
	GetWorld()->Exec(GetWorld(), TEXT("SCALABILITY 4"));
}

void ATableAndChairsPlayerController::LeftClickPressed()
{
	// Get the location of the cursor in world space
	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	if (!flag) return;
	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);

	// Spawn a ray from the cursor to "infinity" to find an editable table
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams) &&
		OutHit.bBlockingHit &&
		OutHit.GetActor()->GetClass() == ATableActor::StaticClass())
	{
		TableBeingEdited = (ATableActor*)OutHit.GetActor();
		CurrentCornerDraggedComponent = static_cast<UProceduralMeshComponent*>(OutHit.GetComponent());
	}
}

void ATableAndChairsPlayerController::LeftClickReleased()
{
	TableBeingEdited = nullptr;
	CurrentCornerDraggedComponent = nullptr;
}


void ATableAndChairsPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TableBeingEdited == nullptr || CurrentCornerDraggedComponent == nullptr)
		return;

	// Intersect the mouse ray with the table plane to find the new corner
	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	FVector End = ((ForwardVector * 10000.f) + Start);
	FVector NewCornerWorldLocation = FMath::LinePlaneIntersection(Start, End, CurrentCornerDraggedComponent->GetComponentLocation(), FVector::UpVector);

	TableBeingEdited->SetCornerWorldLocation(CurrentCornerDraggedComponent, NewCornerWorldLocation);
}
