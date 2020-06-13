#include "ProceduralBoxComponent.h"
#include "ConstructorHelpers.h"


UProceduralBoxComponent::UProceduralBoxComponent()
{
	bUseAsyncCooking = true; // Use PhysX multithreaded cooking
}


void UProceduralBoxComponent::Build(const FVector Size, bool CollisionEnabled)
{
	Build(Size, CollisionEnabled, TArray<FLinearColor>(), TArray<FProcMeshTangent>());
}

void UProceduralBoxComponent::Build(const FVector Size, bool CollisionEnabled, const TArray<FLinearColor> LinearColors, const TArray<FProcMeshTangent> MeshTangents)
{
	ensureAlwaysMsgf(!Size.IsZero(), TEXT("Size can't be zero"));
	ensureAlwaysMsgf(Size.X > 0 && Size.Y > 0, TEXT("Size can't be negative"));

	TArray<FVector2D> UV0 = { { 0, 0 } , { 1, 0 }, { 0, 1 }, { 1, 1 } };

	// Top
	TArray<FVector> vertices = {
		{-Size.X / 2, -Size.Y / 2, Size.Z / 2 },
		{-Size.X / 2,  Size.Y / 2, Size.Z / 2 },
		{ Size.X / 2, -Size.Y / 2, Size.Z / 2 },
		{ Size.X / 2,  Size.Y / 2, Size.Z / 2 }
	};
	TArray<int32> triangles = { 2, 0, 1, 1, 3, 2 };
	TArray<FVector> normals = { FVector::UpVector , FVector::UpVector, FVector::UpVector , FVector::UpVector };

	CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Bottom
	vertices = {
	{-Size.X / 2, -Size.Y / 2, -Size.Z / 2 },
	{-Size.X / 2,  Size.Y / 2, -Size.Z / 2 },
	{ Size.X / 2, -Size.Y / 2, -Size.Z / 2 },
	{ Size.X / 2,  Size.Y / 2, -Size.Z / 2 }
	};
	triangles = { 2, 1, 0, 1, 2, 3 };
	normals = { -FVector::UpVector , -FVector::UpVector, -FVector::UpVector , -FVector::UpVector };
	CreateMeshSection_LinearColor(1, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Backward
	vertices = {
	{-Size.X / 2, -Size.Y / 2, Size.Z / 2  },
	{-Size.X / 2,  Size.Y / 2, Size.Z / 2  },
	{-Size.X / 2, -Size.Y / 2, -Size.Z / 2  },
	{-Size.X / 2,  Size.Y / 2, -Size.Z / 2  }
	};
	triangles = { 0, 2, 1, 1, 2, 3 };
	normals = { -FVector::ForwardVector , -FVector::ForwardVector, -FVector::ForwardVector , -FVector::ForwardVector };
	CreateMeshSection_LinearColor(2, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Forward
	vertices = {
	{Size.X / 2, -Size.Y / 2,  Size.Z / 2  },
	{Size.X / 2,  Size.Y / 2,  Size.Z / 2  },
	{Size.X / 2, -Size.Y / 2, -Size.Z / 2 },
	{Size.X / 2,  Size.Y / 2, -Size.Z / 2 }
	};
	triangles = { 2, 0, 1, 1, 3, 2 };
	normals = { FVector::ForwardVector , FVector::ForwardVector, FVector::ForwardVector , FVector::ForwardVector };
	CreateMeshSection_LinearColor(3, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Left
	vertices = {
	{Size.X / 2, -Size.Y / 2, Size.Z / 2  },
	{-Size.X / 2,  -Size.Y / 2, Size.Z / 2  },
	{ Size.X / 2, -Size.Y / 2, -Size.Z / 2  },
	{-Size.X / 2,  -Size.Y / 2, -Size.Z / 2 }
	};
	triangles = { 0, 2, 1, 1, 2, 3 };
	normals = { -FVector::RightVector, -FVector::RightVector, -FVector::RightVector , -FVector::RightVector };
	CreateMeshSection_LinearColor(4, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Right
	vertices = {
	{Size.X / 2, Size.Y / 2, Size.Z / 2  },
	{-Size.X / 2,  Size.Y / 2, Size.Z / 2  },
	{ Size.X / 2, Size.Y / 2, -Size.Z / 2  },
	{-Size.X / 2,  Size.Y / 2, -Size.Z / 2 }
	};
	triangles = { 0, 1, 2, 1, 3, 2 };
	normals = { FVector::RightVector, FVector::RightVector, FVector::RightVector , FVector::RightVector };
	CreateMeshSection_LinearColor(5, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	//Box->SetupAttachment(RootComponent);
	//return Box;
}

void UProceduralBoxComponent::SetBoxMaterial(const UMaterialInterface * const NewMaterial)
{
	for (size_t i = 0; i < GetNumSections(); i++)
		SetMaterial(i, (UMaterialInterface*)NewMaterial);
}
