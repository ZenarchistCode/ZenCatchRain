modded class Bottle_Base extends Edible_Base
{
	protected ref RainProcurementComponentBottle m_RainProcComponent;
	bool m_IsCatchingRain = false;

	void Bottle_Base()
	{
		RegisterNetSyncVariableBool("m_IsCatchingRain");
		m_RainProcComponent = new RainProcurementComponentBottle(this);
	}

	void SetCatchingRain(bool b)
	{
		m_IsCatchingRain = b;
		SetSynchDirty();

		if (m_IsCatchingRain)
		{
			m_RainProcComponent.StartRainProcurement();
		}
		else
		{
			m_RainProcComponent.StopRainProcurement();
		}
	}

	override void OnItemLocationChanged(EntityAI old_owner, EntityAI new_owner)
	{
		super.OnItemLocationChanged(old_owner, new_owner);

		SetCatchingRain(false);
	}

	bool IsCatchingRain()
	{
		return m_IsCatchingRain;
	}

	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionFillBottleRainBase);
		AddAction(ActionFillRainGround);
	}
}

class RainProcurementComponentBottle : RainProcurementComponentBase 
{
	override float GetBaseLiquidAmount()
	{
		return ZenRainWaterConstants.FILL_GROUND_SPEED; // Fill twice as fast as barrels
	}
};
