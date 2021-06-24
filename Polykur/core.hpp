#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <vector>

#include "structure.hpp"

namespace polykur
{
	namespace Core
	{
		extern uint64_t GWorld;	// Pointer of UWorld 
		extern uint64_t UWorld;
		extern uint64_t ULevel;
		extern uint64_t OwningGameInstance;
		extern uint64_t LocalPlayers;
		extern uint64_t LocalPlayer;
		extern uint64_t PlayerController;
		extern uint64_t PlayerCameraManager;
		extern uint64_t AcknowledgedPawn;
		extern uint64_t Actors;
		extern uint32_t ActorCounts;

		extern uint64_t CameraCache;
		extern uint64_t PlayerState;
		extern uint64_t PlayerName;
		extern uint64_t Mesh;
		extern uint64_t StaticMesh;
		extern uint64_t ComponentToWorld;
		extern uint64_t BoneArray;
		extern uint64_t Team;

		extern uint64_t Pawn222;

		extern uint8_t LocalTeam;

		void Init();
		void Update();

		FTransform GetBoneIndex(uint64_t mesh, int index);
		FVector GetBoneWithRotation(uint64_t mesh, int id);	
		FVector WorldToScreen(FVector world, FCameraCacheEntry camera);
		bool CheckPlayerAlive(uint64_t actoraddr);
		uintptr_t GetPtrAddress(uintptr_t ptr, const std::vector<std::ptrdiff_t> offsets);
	}
}