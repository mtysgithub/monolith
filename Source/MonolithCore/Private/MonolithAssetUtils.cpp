#include "MonolithAssetUtils.h"
#include "MonolithJsonUtils.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/UObjectHash.h"
#include "UObject/Package.h"

FString FMonolithAssetUtils::ResolveAssetPath(const FString& InPath)
{
	FString Path = InPath;
	Path.TrimStartAndEndInline();

	// Normalize backslashes
	Path.ReplaceInline(TEXT("\\"), TEXT("/"));

	// Handle /Content/ → /Game/
	if (Path.StartsWith(TEXT("/Content/")))
	{
		Path = TEXT("/Game/") + Path.Mid(9);
	}
	else if (!Path.StartsWith(TEXT("/")))
	{
		// Relative path — assume /Game/
		Path = TEXT("/Game/") + Path;
	}

	// Strip extension if present
	if (Path.EndsWith(TEXT(".uasset")) || Path.EndsWith(TEXT(".umap")))
	{
		Path = FPaths::GetBaseFilename(Path, false);
	}

	return Path;
}

UPackage* FMonolithAssetUtils::LoadPackageByPath(const FString& AssetPath)
{
	FString Resolved = ResolveAssetPath(AssetPath);
	UPackage* Package = LoadPackage(nullptr, *Resolved, LOAD_None);
	if (!Package)
	{
		UE_LOG(LogMonolith, Warning, TEXT("Failed to load package: %s"), *Resolved);
	}
	return Package;
}

UObject* FMonolithAssetUtils::LoadAssetByPath(const FString& AssetPath)
{
	FString Resolved = ResolveAssetPath(AssetPath);

	// Try StaticLoadObject first (handles ObjectPath format)
	UObject* Asset = StaticLoadObject(UObject::StaticClass(), nullptr, *Resolved);
	if (Asset)
	{
		return Asset;
	}

	// Build PackageName.ObjectName format — required for NiagaraSystem and other asset types
	FString PackageName = FPackageName::ObjectPathToPackageName(Resolved);
	FString ObjectName = FPackageName::ObjectPathToObjectName(Resolved);
	if (ObjectName.IsEmpty())
	{
		ObjectName = FPackageName::GetShortName(PackageName);
	}

	// Try PackageName.ObjectName format (how UE internally resolves many asset types)
	FString FullObjectPath = PackageName + TEXT(".") + ObjectName;
	Asset = StaticLoadObject(UObject::StaticClass(), nullptr, *FullObjectPath);
	if (Asset)
	{
		return Asset;
	}

	// Load the package and search within it
	UPackage* Package = LoadPackage(nullptr, *PackageName, LOAD_None);
	if (Package)
	{
		Asset = FindObject<UObject>(Package, *ObjectName);
		if (!Asset)
		{
			// Try with _C suffix for Blueprint generated classes
			Asset = FindObject<UObject>(Package, *(ObjectName + TEXT("_C")));
		}
		if (!Asset)
		{
			// Last resort: iterate package for first non-transient object
			// Handles assets whose internal name differs from package name
			ForEachObjectWithPackage(Package, [&Asset](UObject* Obj)
			{
				if (!Obj->IsA<UPackage>() && !Obj->HasAnyFlags(RF_Transient))
				{
					Asset = Obj;
					return false; // stop
				}
				return true; // continue
			}, false);
		}
	}

	if (!Asset)
	{
		UE_LOG(LogMonolith, Warning, TEXT("Failed to load asset: %s (tried: %s, %s)"), *AssetPath, *Resolved, *FullObjectPath);
	}
	return Asset;
}

bool FMonolithAssetUtils::AssetExists(const FString& AssetPath)
{
	FString Resolved = ResolveAssetPath(AssetPath);
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(FSoftObjectPath(Resolved));
	return AssetData.IsValid();
}

TArray<FAssetData> FMonolithAssetUtils::GetAssetsByClass(const FTopLevelAssetPath& ClassPath, const FString& PackagePath)
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();

	FARFilter Filter;
	Filter.ClassPaths.Add(ClassPath);
	if (!PackagePath.IsEmpty())
	{
		Filter.PackagePaths.Add(FName(*PackagePath));
		Filter.bRecursivePaths = true;
	}

	TArray<FAssetData> Results;
	AssetRegistry.GetAssets(Filter, Results);
	return Results;
}

FString FMonolithAssetUtils::GetAssetName(const FString& AssetPath)
{
	return FPackageName::GetShortName(AssetPath);
}
