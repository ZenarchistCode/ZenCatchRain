modded class Bottle_Base extends Edible_Base
{
	bool m_IsCatchingRain = false;

	// Register m_IsCatchingRain for client-side to stop showing catch rain action
	void Bottle_Base()
	{
		RegisterNetSyncVariableBool("m_IsCatchingRain");
	}

	// (Server-side) Sets whether or not this bottle_base is currently catching rain
	void SetCatchingRain(bool b)
	{
		m_IsCatchingRain = b;
		SetSynchDirty();
	}

	// (Client & server) Returns whether or not this bottle_base is currently catching rain
	bool IsCatchingRain()
	{
		return m_IsCatchingRain;
	}

	// Set new actions
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionFillBottleRainBase);
		AddAction(ActionFillRainGround);
	}
}