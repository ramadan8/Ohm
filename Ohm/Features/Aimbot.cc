#include "./Aimbot.h"

#include "../Config.h"

#include "../Interfaces/Dependencies.h"
#include "../Interfaces/Interfaces.h"

#include "../SDK/Class/CGlobalVars.h"
#include "../SDK/Class/CUserCmd.h"

#include "../SDK/Entities/CBasePlayer.h"

#include "../Utility/Utilities.h"

Vector CalculateRelativeAngle(const Vector& src, const Vector& dst, const Vector& viewAngles) {
	return ((dst - src).toAngle() - viewAngles).normalize();
}

void Aimbot::Run(CUserCmd* cmd) {
	if (!config->aim.isEnabled)
		return;

	CBasePlayer* localPlayer = Utilities::getLocalPlayer();

	if (!localPlayer || !localPlayer->isAlive())
		return;

	CBaseCombatWeapon* currentWeapon = localPlayer->getCurrentWeapon();

	if (!currentWeapon || currentWeapon->getClip() < 1)
		return;

	if (currentWeapon->isGrenade())
		return;

	// Player cannot use their weapon/item yet (e.g., switching weapons)
	if (localPlayer->getNextAttack() > memory->GlobalVars->ServerTime(cmd))
		return;

	if (localPlayer->isDefusing())
		return;

	if (!config->aim.ignoreFlash && localPlayer->isFlashed())
		return;

	int ourTeam = localPlayer->getTeam();

	float bestFov = config->aim.fieldOfView;
	Vector bestAngle{ };
	
	const Vector lpEyePos = localPlayer->getEyePosition();
	const Vector lpAimPunch = localPlayer->getAimPunch();

	for (int idx = 0; idx < interfaces->Engine->GetMaxClients(); idx++) {
		CBasePlayer* thisPlayer = reinterpret_cast<CBasePlayer*>(interfaces->ClientEntityList->GetClientEntity(idx));

		if (!thisPlayer || thisPlayer == localPlayer)
			continue;

		if (thisPlayer->GetDormant())
			continue;

		if (!config->aim.friendlyFire && thisPlayer->getTeam() == ourTeam)
			continue;

		if (!thisPlayer->isAlive())
			continue;

		if (thisPlayer->hasGunGameImmunity())
			continue;

		for (unsigned int bone : { 8, 4, 3, 7, 6, 5 }) {
			const Vector bonePos = thisPlayer->getBonePosition(bone);
			const Vector boneAng = CalculateRelativeAngle(lpEyePos, bonePos, cmd->viewAngles + lpAimPunch);

			const float fov = std::hypot(boneAng.x, boneAng.y);

			if (fov > bestFov)
				continue;

			if (!config->aim.ignoreSmoke && memory->GoesThroughSmoke(lpEyePos, bonePos, 1))
				continue;

			if (!thisPlayer->isVisible(localPlayer, bonePos))
				continue;

			if (fov < bestFov) {
				bestFov = fov;
				bestAngle = bonePos;
			}
		}
	}

	if (bestAngle.notNull()) {
		static Vector lastAngles{ cmd->viewAngles };
		static int lastCmd{};

		if (lastCmd == cmd->commandNumber - 1 && lastAngles.notNull() && config->aim.silentAim)
			cmd->viewAngles = lastAngles;

		Vector aimAngle = CalculateRelativeAngle(lpEyePos, bestAngle, cmd->viewAngles + lpAimPunch);
		aimAngle /= config->aim.smoothAmount;

		cmd->viewAngles += aimAngle;

		if (!config->aim.silentAim)
			interfaces->Engine->SetViewAngles(cmd->viewAngles);

		const bool canAttack = currentWeapon->nextPrimaryAttack() <= memory->GlobalVars->ServerTime();

		// TODO: Fix issue where user may double-click into scope unnaturally.
		if (config->aim.autoScope && canAttack && currentWeapon->isSniper() && !localPlayer->isScoped())
			cmd->buttons |= CUserCmd::IN_ATTACK2;

		if (config->aim.autoShoot && canAttack)
			cmd->buttons |= CUserCmd::IN_ATTACK;

		lastAngles = config->aim.smoothAmount > 1.f ? cmd->viewAngles : Vector{};
		lastCmd = cmd->commandNumber;
	}
}

void Aimbot::FixMovement(CUserCmd* cmd, Vector oldAngles, float oldForward, float oldSide) {
	float deltaView, oldYaw, newYaw;
	Vector newAngles = cmd->viewAngles;

	// X, Y, Z => Pitch, Yaw, Roll
	oldYaw = oldAngles.y < 0.f ? 360.f + oldAngles.y : oldAngles.y;
	newYaw = newAngles.y < 0.f ? 360.f + newAngles.y : newAngles.y;

	deltaView = 360.f - (newYaw < oldYaw ? std::abs(newYaw - oldYaw) : 360.f - std::abs(newYaw - oldYaw));

	cmd->forwardMove = std::cos(deg2rad(deltaView)) * oldForward + std::cos(deg2rad(deltaView + 90)) * oldSide;
	cmd->sideMove = std::sin(deg2rad(deltaView)) * oldForward + std::sin(deg2rad(deltaView + 90)) * oldSide;
}