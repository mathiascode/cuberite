
#include "Globals.h"  // NOTE: MSVC stupidness requires this to be the same across all modules

#include "EnderDragon.h"





cEnderDragon::cEnderDragon(void) :
	// TODO: Vanilla source says this, but is it right? Dragons fly, they don't stand
	super(mtEnderDragon, "entity.enderdragon.hurt", "entity.enderdragon.death", 16.0, 8.0)
{
	m_EMPersonality = AGGRESSIVE;
	m_BehaviorDoNothing.AttachToMonster(*this);
	GetMonsterConfig("EnderDragon");
}





void cEnderDragon::GetDrops(cItems & a_Drops, cEntity * a_Killer)
{
	return;
}




