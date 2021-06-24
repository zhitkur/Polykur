#include "core.hpp"
#include "memory.hpp"
#include "render.hpp"
#include "var.hpp"

#pragma warning (disable : 4244)

namespace polykur
{
	uint64_t Core::GWorld{};
	uint64_t Core::UWorld{};
	uint64_t Core::ULevel{};				//Engine.World.PersistentLevel
	uint64_t Core::OwningGameInstance{};	//Engine.World.OwningGameInstance
	uint64_t Core::LocalPlayers{};			//Engine.GameInstance.LocalPlayers
	uint64_t Core::LocalPlayer{};
	uint64_t Core::PlayerController{};
	uint64_t Core::PlayerCameraManager{};
	uint64_t Core::AcknowledgedPawn{};
	uint64_t Core::Actors{};
	uint32_t Core::ActorCounts{};
	uint64_t Core::Pawn222{};
	uint8_t Core::LocalTeam{};

	uint64_t Core::CameraCache = 0x1AA0;	//Engine.PlayerCameraManager.CameraCachePrivate
	uint64_t Core::PlayerState = 0x240;		//Engine.Pawn.PlayerState
	uint64_t Core::PlayerName = 0x300;		//Engine.PlayerState.PlayerNamePrivate
	uint64_t Core::Mesh = 0x280;			//Engine.Character.Mesh
	uint64_t Core::StaticMesh = 0x478;		//Engine.StaticMeshComponent.StaticMesh	(Get Bone Index -> But Not this game maybe? doesn't work to me)
	uint64_t Core::ComponentToWorld = 0x1C0;		
	uint64_t Core::BoneArray = 0x4A0;		// Get Bone Index
	uint64_t Core::Team = 0x36C;

	void Core::Init()
	{
		// Init GWorld
		Core::GWorld = mem::dwGameBase + 0x57EE6F0;
	}

	void Core::Update()
	{
		Core::UWorld = mem::RPM<uint64_t>(Core::GWorld);
		Core::ULevel = mem::RPM<uint64_t>(Core::UWorld + 0x30);						//Engine.World.PersistentLevel
		Core::OwningGameInstance = mem::RPM<uint64_t>(Core::UWorld + 0x180);		//Engine.World.OwningGameInstance
		Core::LocalPlayers = mem::RPM<uint64_t>(OwningGameInstance + 0x38);			//Engine.GameInstance.LocalPlayers
		Core::LocalPlayer = mem::RPM<uint64_t>(LocalPlayers);
		Core::PlayerController = mem::RPM<uint64_t>(LocalPlayer + 0x30);			//Engine.Player.PlayerController
		Core::PlayerCameraManager = mem::RPM<uint64_t>(PlayerController + 0x2b8);	//Engine.PlayerController.PlayerCameraManager
		Core::AcknowledgedPawn = mem::RPM<uint64_t>(PlayerController + 0x2a0);		//Engine.PlayerController.AcknowledgedPawn
		Core::Actors = mem::RPM<uint64_t>(ULevel + 0xA8);
		Core::ActorCounts = mem::RPM<uint64_t>(ULevel + 0xB0);
		Core::Pawn222 = mem::RPM<uint64_t>(PlayerController + 0x258);

		Core::LocalTeam = mem::RPM<uint8_t>(mem::RPM<uint64_t>(AcknowledgedPawn + Core::PlayerState) + Core::Team);
	}

	FTransform Core::GetBoneIndex(uint64_t mesh, int index)
	{
		uint64_t BoneArray = mem::RPM<uint64_t>(mesh + Core::BoneArray);

		return mem::RPM<FTransform>(BoneArray + (index * 0x30));
	}

	FVector Core::GetBoneWithRotation(uint64_t mesh, int id)
	{
		FTransform Bone = GetBoneIndex(mesh, id);
		FTransform ComponentToWorld = mem::RPM<FTransform>(mesh + Core::ComponentToWorld);

		FMatrix Matrix = Bone.ToMatrixWithScale() * ComponentToWorld.ToMatrixWithScale();
		return FVector{ Matrix.WPlane.X, Matrix.WPlane.Y, Matrix.WPlane.Z };
	}

	FVector Core::WorldToScreen(FVector WorldLocation, FCameraCacheEntry CameraCacheL)
	{
		FVector Screenlocation{};

		auto POV = CameraCacheL.POV;
		FVector Rotation = POV.Rotation;

		FMatrix tempMatrix = ToMatrix(Rotation);

		FVector vAxisX, vAxisY, vAxisZ;

		vAxisX = FVector{ tempMatrix.XPlane.X, tempMatrix.XPlane.Y, tempMatrix.XPlane.Z };
		vAxisY = FVector{ tempMatrix.YPlane.X, tempMatrix.YPlane.Y, tempMatrix.YPlane.Z };
		vAxisZ = FVector{ tempMatrix.ZPlane.X, tempMatrix.ZPlane.Y, tempMatrix.ZPlane.Z };

		FVector vDelta = WorldLocation - POV.Location;
		FVector vTransformed = FVector{ vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX) };

		if (vTransformed.Z < 1.f)
			vTransformed.Z = 1.f;
		
		Vars.visuals.distance = vTransformed.Z / 100.f;
		

		float FovAngle = POV.FOV;
		float ScreenCenterX = cRender::Width / 2;
		float ScreenCenterY = cRender::Height / 2;

		Screenlocation.X = ScreenCenterX + vTransformed.X * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;
		Screenlocation.Y = ScreenCenterY - vTransformed.Y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.Z;

		return Screenlocation;
	}

	bool Core::CheckPlayerAlive(uint64_t actoraddr)
	{
		int Relevant = mem::RPM<char>(actoraddr + 0x58);
		if (Relevant == -112)
		{
			return true;
		}
		return false;
	}

	uintptr_t Core::GetPtrAddress(uintptr_t ptr, const std::vector<std::ptrdiff_t> offsets)
	{
		for (const auto& offset : offsets)
		{
			if (ptr == 0) return 0;
			ptr = *(uintptr_t*)(ptr);
			ptr += offset;
		}
		return ptr;
	}
}