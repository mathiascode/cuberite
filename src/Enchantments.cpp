// Enchantments.cpp

// Implements the cEnchantments class representing a storage for item enchantments and stored-enchantments

#include "Globals.h"
#include "Enchantments.h"
#include "WorldStorage/FastNBT.h"
#include "FastRandom.h"





cEnchantments::cEnchantments(void)
{
	// Nothing needed yet, but the constructor needs to be declared and impemented in order to be usable
}





cEnchantments::cEnchantments(const AString & a_StringSpec)
{
	AddFromString(a_StringSpec);
}





void cEnchantments::AddFromString(const AString & a_StringSpec)
{
	// Add enchantments in the stringspec; if a specified enchantment already exists, overwrites it
	
	// Split the StringSpec into separate declarations, each in the form "id=lvl":
	AStringVector Decls = StringSplit(a_StringSpec, ";");
	for (AStringVector::const_iterator itr = Decls.begin(), end = Decls.end(); itr != end; ++itr)
	{
		// Split each declaration into the id and lvl part:
		if (itr->empty())
		{
			// The decl is empty (may happen if there's an extra semicolon at the end), ignore silently
			continue;
		}
		AStringVector Split = StringSplitAndTrim(*itr, "=");
		if (Split.size() != 2)
		{
			// Malformed decl
			LOG("%s: Malformed enchantment decl: \"%s\", skipping.", __FUNCTION__, itr->c_str());
			continue;
		}
		int id = StringToEnchantmentID(Split[0]);
		if (id < 0)
		{
			LOG("%s: Failed to parse enchantment \"%s\", skipping.", __FUNCTION__, Split[0].c_str());
			continue;
		}
		int lvl = atoi(Split[1].c_str());
		if ((lvl == 0) && (Split[1] != "0"))
		{
			// Level failed to parse
			LOG("%s: Failed to parse enchantment level \"%s\", skipping.", __FUNCTION__, Split[1].c_str());
			continue;
		}
		SetLevel(id, lvl);
	}  // for itr - Decls[]
}





AString cEnchantments::ToString(void) const
{
	// Serialize all the enchantments into a string
	AString res;
	for (cEnchantments::cMap::const_iterator itr = m_Enchantments.begin(), end = m_Enchantments.end(); itr != end; ++itr)
	{
		AppendPrintf(res, "%d=%d;", itr->first, itr->second);
	}  // for itr - m_Enchantments[]
	return res;
}





int cEnchantments::GetLevel(int a_EnchantmentID) const
{
	// Return the level for the specified enchantment; 0 if not stored
	cMap::const_iterator itr = m_Enchantments.find(a_EnchantmentID);
	if (itr != m_Enchantments.end())
	{
		return itr->second;
	}

	// Not stored, return zero
	return 0;
}





void cEnchantments::SetLevel(int a_EnchantmentID, int a_Level)
{
	// Sets the level for the specified enchantment, adding it if not stored before or removing it if level <= 0
	if (a_Level == 0)
	{
		// Delete enchantment, if present:
		cMap::iterator itr = m_Enchantments.find(a_EnchantmentID);
		if (itr != m_Enchantments.end())
		{
			m_Enchantments.erase(itr);
		}
	}
	else
	{
		// Add / overwrite enchantment
		m_Enchantments[a_EnchantmentID] = a_Level;
	}
}






void cEnchantments::Clear(void)
{
	m_Enchantments.clear();
}





bool cEnchantments::IsEmpty(void) const
{
	return m_Enchantments.empty();
}





int cEnchantments::StringToEnchantmentID(const AString & a_EnchantmentName)
{
	static const struct
	{
		int m_Value;
		const char * m_Name;
	} EnchantmentNames[] =
	{
		{ enchProtection,           "Protection"},
		{ enchFireProtection,       "FireProtection"},
		{ enchFeatherFalling,       "FeatherFalling"},
		{ enchBlastProtection,      "BlastProtection"},
		{ enchProjectileProtection, "ProjectileProtection"},
		{ enchRespiration,          "Respiration"},
		{ enchAquaAffinity,         "AquaAffinity"},
		{ enchThorns,               "Thorns"},
		{ enchSharpness,            "Sharpness"},
		{ enchSmite,                "Smite"},
		{ enchBaneOfArthropods,     "BaneOfArthropods"},
		{ enchKnockback,            "Knockback"},
		{ enchFireAspect,           "FireAspect"},
		{ enchLooting,              "Looting"},
		{ enchEfficiency,           "Efficiency"},
		{ enchSilkTouch,            "SilkTouch"},
		{ enchUnbreaking,           "Unbreaking"},
		{ enchFortune,              "Fortune"},
		{ enchPower,                "Power"},
		{ enchPunch,                "Punch"},
		{ enchFlame,                "Flame"},
		{ enchInfinity,             "Infinity"},
		{ enchLuckOfTheSea,         "LuckOfTheSea"},
		{ enchLure,                 "Lure"},
	} ;

	// First try to parse as a number:
	int id = atoi(a_EnchantmentName.c_str());
	if ((id != 0) || (a_EnchantmentName == "0"))
	{
		return id;
	}

	// It wasn't a number, do a lookup:
	for (size_t i = 0; i < ARRAYCOUNT(EnchantmentNames); i++)
	{
		if (NoCaseCompare(EnchantmentNames[i].m_Name, a_EnchantmentName) == 0)
		{
			return EnchantmentNames[i].m_Value;
		}
	}  // for i - EnchantmentNames[]
	return -1;
}





bool cEnchantments::operator ==(const cEnchantments & a_Other) const
{
	return m_Enchantments == a_Other.m_Enchantments;
}





bool cEnchantments::operator !=(const cEnchantments & a_Other) const
{
	return m_Enchantments != a_Other.m_Enchantments;
}





void cEnchantments::AddItemEnchantmentWeights(cWeightedEnchantments & a_Enchantments, short & a_ItemType, int a_EnchantmentLevel)
{
	if (ItemCategory::IsSword(a_ItemType))
	{
		// Sharpness
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 54)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=4"));
		}
		else if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 43)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=3"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 32)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 21)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=1"));
		}

		// Smite
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 49)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=1"));
		}

		// Bane of Arthropods
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 49)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=1"));
		}

		// Knockback
		if (a_EnchantmentLevel >= 25 && a_EnchantmentLevel <= 75)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Knockback=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 55)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Knockback=1"));
		}

		// Fire Aspect
		if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 80)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("FireAspect=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 60)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("FireAspect=1"));
		}

		// Looting
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=1"));
		}
	}

	else if (ItemCategory::IsTool(a_ItemType))
	{
		// Efficiency
		if (a_EnchantmentLevel >= 31 && a_EnchantmentLevel <= 81)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 71)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=3"));
		}
		else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 61)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 51)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=1"));
		}

		// Silk Touch
		if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("SilkTouch=1"));
		}

		// Fortune
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=1"));
		}
	}

	else if (ItemCategory::IsArmor(a_ItemType))
	{
		// Protection
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 54)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=4"));
		}
		else if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 43)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=3"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 32)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 21)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=1"));
		}

		// Fire Protection
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 46)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=4"));
		}
		else if (a_EnchantmentLevel >= 26 && a_EnchantmentLevel <= 38)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=3"));
		}
		else if (a_EnchantmentLevel >= 18 && a_EnchantmentLevel <= 30)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 22)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=1"));
		}

		// Blast Protection
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 17)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=1"));
		}

		// Projectile Protection
		if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 36)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=4"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 30)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=3"));
		}
		else if (a_EnchantmentLevel >= 9 && a_EnchantmentLevel <= 24)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=2"));
		}
		else if (a_EnchantmentLevel >= 3 && a_EnchantmentLevel <= 18)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=1"));
		}

		// Thorns
		if (a_EnchantmentLevel >= 50 && a_EnchantmentLevel <= 100)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=3"));
		}
		else if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 80)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 60)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=1"));
		}


		if (ItemCategory::IsHelmet(a_ItemType))
		{
			// Respiration
			if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 60)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=3"));
			}
			else if (a_EnchantmentLevel >= 20 && a_EnchantmentLevel <= 50)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=2"));
			}
			else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 40)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=1"));
			}

			// Aqua Affinity
			if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 41)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("AquaAffinity=1"));
			}
		}

		else if (ItemCategory::IsBoots(a_ItemType))
		{
			// Feather Fall
			if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 33)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=4"));
			}
			else if (a_EnchantmentLevel >= 17 && a_EnchantmentLevel <= 27)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=3"));
			}
			else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 21)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=2"));
			}
			else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 15)
			{
				AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=1"));
			}
		}
	}

	else if (a_ItemType == E_ITEM_BOW)
	{
		// Power
		if (a_EnchantmentLevel >= 31 && a_EnchantmentLevel <= 46)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 36)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=3"));
		}
		else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 26)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 16)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=1"));
		}

		// Punch
		if (a_EnchantmentLevel >= 32 && a_EnchantmentLevel <= 57)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Punch=2"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 37)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Punch=1"));
		}

		// Flame and Infinity
		if (a_EnchantmentLevel >= 20 && a_EnchantmentLevel <= 50)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Flame=1"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Infinity=1"));
		}
	}

	else if (a_ItemType == E_ITEM_FISHING_ROD)
	{
		// Luck of the Sea and Lure
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=3"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=2"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=1"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=1"));
		}
	}

	else if (a_ItemType == E_ITEM_BOOK)
	{
		// All &a_Enchantments

		// Sharpness
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 54)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=4"));
		}
		else if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 43)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=3"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 32)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 21)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Sharpness=1"));
		}

		// Smite
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 49)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Smite=1"));
		}

		// Bane of Arthropods
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 49)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("BaneOfArthropods=1"));
		}

		// Knockback
		if (a_EnchantmentLevel >= 25 && a_EnchantmentLevel <= 75)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Knockback=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 55)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Knockback=1"));
		}

		// Fire Aspect
		if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 80)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("FireAspect=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 60)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("FireAspect=1"));
		}

		// Looting
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Looting=1"));
		}

		// Efficiency
		if (a_EnchantmentLevel >= 31 && a_EnchantmentLevel <= 81)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 71)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=3"));
		}
		else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 61)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 51)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Efficiency=1"));
		}

		// Silk Touch
		if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("SilkTouch=1"));
		}

		// Fortune
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Fortune=1"));
		}

		// Protection
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 54)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=4"));
		}
		else if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 43)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=3"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 32)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 21)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Protection=1"));
		}

		// Fire Protection
		if (a_EnchantmentLevel >= 34 && a_EnchantmentLevel <= 46)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=4"));
		}
		else if (a_EnchantmentLevel >= 26 && a_EnchantmentLevel <= 38)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=3"));
		}
		else if (a_EnchantmentLevel >= 18 && a_EnchantmentLevel <= 30)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 22)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FireProtection=1"));
		}

		// Blast Protection
		if (a_EnchantmentLevel >= 29 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=3"));
		}
		else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 25)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 17)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("BlastProtection=1"));
		}

		// Projectile Protection
		if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 36)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=4"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 30)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=3"));
		}
		else if (a_EnchantmentLevel >= 9 && a_EnchantmentLevel <= 24)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=2"));
		}
		else if (a_EnchantmentLevel >= 3 && a_EnchantmentLevel <= 18)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("ProjectileProtection=1"));
		}

		// Thorns
		if (a_EnchantmentLevel >= 50 && a_EnchantmentLevel <= 100)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=3"));
		}
		else if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 80)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 60)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Thorns=1"));
		}

		// Respiration
		if (a_EnchantmentLevel >= 30 && a_EnchantmentLevel <= 60)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=3"));
		}
		else if (a_EnchantmentLevel >= 20 && a_EnchantmentLevel <= 50)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=2"));
		}
		else if (a_EnchantmentLevel >= 10 && a_EnchantmentLevel <= 40)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Respiration=1"));
		}

		// Aqua Affinity
		if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 41)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("AquaAffinity=1"));
		}

		// Feather Fall
		if (a_EnchantmentLevel >= 23 && a_EnchantmentLevel <= 33)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=4"));
		}
		else if (a_EnchantmentLevel >= 17 && a_EnchantmentLevel <= 27)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=3"));
		}
		else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 21)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=2"));
		}
		else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 15)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("FeatherFalling=1"));
		}

		// Power
		if (a_EnchantmentLevel >= 31 && a_EnchantmentLevel <= 46)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=4"));
		}
		else if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 36)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=3"));
		}
		else if (a_EnchantmentLevel >= 11 && a_EnchantmentLevel <= 26)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=2"));
		}
		else if (a_EnchantmentLevel >= 1 && a_EnchantmentLevel <= 16)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 10, cEnchantments("Power=1"));
		}

		// Punch
		if (a_EnchantmentLevel >= 32 && a_EnchantmentLevel <= 57)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Punch=2"));
		}
		else if (a_EnchantmentLevel >= 12 && a_EnchantmentLevel <= 37)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Punch=1"));
		}

		// Flame and Infinity
		if (a_EnchantmentLevel >= 20 && a_EnchantmentLevel <= 50)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 2, cEnchantments("Flame=1"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Infinity=1"));
		}

		// Luck of the Sea and Lure
		if (a_EnchantmentLevel >= 33 && a_EnchantmentLevel <= 83)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=3"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=3"));
		}
		else if (a_EnchantmentLevel >= 24 && a_EnchantmentLevel <= 74)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=2"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=2"));
		}
		else if (a_EnchantmentLevel >= 15 && a_EnchantmentLevel <= 65)
		{
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("LuckOfTheSea=1"));
			AddEnchantmentWeightToVector(&a_Enchantments, 1, cEnchantments("Lure=1"));
		}

		a_ItemType = E_ITEM_ENCHANTED_BOOK;
	}

	// Unbreaking
	if (a_EnchantmentLevel >= 21 && a_EnchantmentLevel <= 71)
	{
		AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Unbreaking=3"));
	}
	else if (a_EnchantmentLevel >= 13 && a_EnchantmentLevel <= 63)
	{
		AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Unbreaking=2"));
	}
	else if (a_EnchantmentLevel >= 5 && a_EnchantmentLevel <= 55)
	{
		AddEnchantmentWeightToVector(&a_Enchantments, 5, cEnchantments("Unbreaking=1"));
	}
}





void cEnchantments::AddEnchantmentWeightToVector(cWeightedEnchantments * a_Enchantments, int a_Weight, cEnchantments a_Enchantment)
{
	cWeightedEnchantment enchantment;
	enchantment.m_Weight = a_Weight;
	enchantment.m_Enchantments = a_Enchantment;
	a_Enchantments->push_back(enchantment);
}





void cEnchantments::RemoveEnchantmentWeightFromVector(cWeightedEnchantments * a_Enchantments, int a_EnchantmentID)
{
	for (cWeightedEnchantments::iterator it = a_Enchantments->begin(); it != a_Enchantments->end(); ++it)
	{
		int EnchantmentID = atoi(StringSplit((*it).m_Enchantments.ToString(), "=")[0].c_str());

		if (EnchantmentID == a_EnchantmentID)
		{
			a_Enchantments->erase(it);
			break;
		}
	}
}





void cEnchantments::RemoveEnchantmentWeightFromVector(cWeightedEnchantments * a_Enchantments, cEnchantments a_Enchantment)
{
	for (cWeightedEnchantments::iterator it = a_Enchantments->begin(); it != a_Enchantments->end(); ++it)
	{
		if ((*it).m_Enchantments == a_Enchantment)
		{
			a_Enchantments->erase(it);
			break;
		}
	}
}





void cEnchantments::CheckEnchantmentConflictsFromVector(cWeightedEnchantments & a_Enchantments, cEnchantments a_FirstEnchantment)
{
	int FirstEnchantmentID = atoi(StringSplit(a_FirstEnchantment.ToString(), "=")[0].c_str());

	if (FirstEnchantmentID == cEnchantments::enchProtection)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchFireProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchBlastProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProjectileProtection);
	}
	else if (FirstEnchantmentID == cEnchantments::enchFireProtection)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchBlastProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProjectileProtection);
	}
	else if (FirstEnchantmentID == cEnchantments::enchBlastProtection)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchFireProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProjectileProtection);
	}
	else if (FirstEnchantmentID == cEnchantments::enchProjectileProtection)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchFireProtection);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchBlastProtection);
	}

	else if (FirstEnchantmentID == cEnchantments::enchSharpness)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchSmite);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchBaneOfArthropods);
	}
	else if (FirstEnchantmentID == cEnchantments::enchSmite)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchSharpness);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchBaneOfArthropods);
	}
	else if (FirstEnchantmentID == cEnchantments::enchBaneOfArthropods)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchSharpness);
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchSmite);
	}
	else if (FirstEnchantmentID == cEnchantments::enchSilkTouch)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchFortune);
	}
	else if (FirstEnchantmentID == cEnchantments::enchFortune)
	{
		RemoveEnchantmentWeightFromVector(&a_Enchantments, cEnchantments::enchSilkTouch);
	}
}





cEnchantments cEnchantments::GetRandomEnchantmentFromVector(cWeightedEnchantments & a_Enchantments)
{
	cFastRandom Random;

	int AllWeights = 0;
	for (cWeightedEnchantments::iterator it = a_Enchantments.begin(); it != a_Enchantments.end(); ++it)
	{
		AllWeights += (*it).m_Weight;
	}
	int RandomNumber = Random.GenerateRandomInteger(0, AllWeights - 1);
	cEnchantments enchantment;
	for (cWeightedEnchantments::iterator it = a_Enchantments.begin(); it != a_Enchantments.end(); ++it)
	{
		RandomNumber -= (*it).m_Weight;
		if (RandomNumber < 0)
		{
			return (*it).m_Enchantments;
		}
	}

	return cEnchantments();
}











