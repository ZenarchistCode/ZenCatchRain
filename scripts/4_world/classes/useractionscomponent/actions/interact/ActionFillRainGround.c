class ActionFillRainGround : ActionInteractBase
{
	protected static ref Timer m_RainWaterTimer; // Used for processing rain bottles
	protected static ref array<Bottle_Base> m_BottlesToProcess; // Used for storing the bottles to process

	// Create & set up action
	void ActionFillRainGround()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text		 = "#STR_ZenRain_Txt";
	}

	// Check action conditions for starting the rain catching process
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (!ZenClientFunctions.IsRaining())
			return false;

		EntityAI target_entity = EntityAI.Cast(target.GetObject());

		// If there is no bottle or it's ruined, stop.
		if (!target_entity || target_entity.IsRuined())
			return false;

		InventoryLocation loc = new InventoryLocation;
		target_entity.GetInventory().GetCurrentInventoryLocation(loc);

		// If the bottle is not on the ground, stop.
		if (loc.GetType() != InventoryLocationType.GROUND)
			return false;

		Bottle_Base bottle = Bottle_Base.Cast(target_entity);

		// If the object is not a bottle_base, stop.
		if (!bottle)
			return false;

		// If the bottle cannot receive water (eg. it's filled with petrol), stop.
		if (!Liquid.CanFillContainer(ItemBase.Cast(target_entity), LIQUID_WATER))
			return false;

		// Check the final conditions.
		return player.IsAlive() && !bottle.IsCatchingRain() && !MiscGameplayFunctions.IsUnderRoof(target_entity);
	}

	// Server-side - start filling water bottle if it's raining
	override void OnExecuteServer(ActionData action_data)
	{
		EntityAI target_entity	= EntityAI.Cast(action_data.m_Target.GetObject());
		Bottle_Base bottle		= Bottle_Base.Cast(target_entity);

		if (bottle)
		{
			// If the bottles list doesn't exist, create it
			if (!m_BottlesToProcess)
			{
				m_BottlesToProcess = new array<Bottle_Base>;
			}

			bottle.SetCatchingRain(true); // Inform client
			m_BottlesToProcess.Insert(bottle); // Add bottle to queue

			// If timer doesn't exist, create it
			if (!m_RainWaterTimer)
			{
				m_RainWaterTimer = new Timer();
			}

			// If timer is not running, start it
			if (m_RainWaterTimer && !m_RainWaterTimer.IsRunning())
			{
				m_RainWaterTimer.Run(1, this, "FillRainWater", NULL, true);
			}
		}
	}

	// Process bottles and fill them with water (instead of having dozens of individual timers per each bottle)
	// TODO: Perform some tests to see if this is more or less efficient with lots of bottles collecting rain
	void FillRainWater()
	{
		// If it's no longer raining, stop the rain timer and inform all bottles
		if (!ZenClientFunctions.IsRaining())
		{
			StopRainTimer(true);
			return;
		}

		// If there are no more bottles to process, stop & delete timer to save server resources
		if (!m_BottlesToProcess || m_BottlesToProcess.Count() == 0)
		{
			StopRainTimer(false);
			return;
		}

		// Process bottles in queue
		for (int i = 0; i < m_BottlesToProcess.Count(); i++)
		{
			Bottle_Base bottle = Bottle_Base.Cast(m_BottlesToProcess.Get(i));

			// Check basic bottle status - does it exist, is it not ruined, is it not filled, is it still raining?
			if (!bottle || bottle.IsRuined() || bottle.GetQuantity() >= bottle.GetQuantityMax())
			{
				bottle.SetCatchingRain(false);
				m_BottlesToProcess.Remove(i);
				continue;
			}

			// Get inventory location
			InventoryLocation loc = new InventoryLocation;
			bottle.GetInventory().GetCurrentInventoryLocation(loc);

			// Check location status - has a player picked it up from ground? Or did someone put gasoline in it?
			if (loc.GetType() != InventoryLocationType.GROUND || !Liquid.CanFillContainer(bottle, LIQUID_WATER))
			{
				bottle.SetCatchingRain(false);
				m_BottlesToProcess.Remove(i);
				continue;
			}

			// Calculate fill speed (with a slight amount of randomness)
			float fill_speed = ZenRainWaterConstants.FILL_RAIN_SPEED * ZenRainWaterConstants.FILL_GROUND_SPEED;
			if (bottle.IsKindOf("Pot"))
				fill_speed = fill_speed * ZenRainWaterConstants.FILL_MULTI_POT;

			// Fill container. False = no inject cholera/salmonella/whatever agents
			Liquid.FillContainerEnviro(bottle, LIQUID_WATER, fill_speed * Math.RandomFloatInclusive(0.9, 1.1), false);

			// If bottle is full, stop timer.
			if (bottle.GetQuantity() >= bottle.GetQuantityMax())
			{
				bottle.SetCatchingRain(false);
				m_BottlesToProcess.Remove(i);
				continue;
			}
		}
	}

	// Stop timer and destroy object references to save server performance.
	void StopRainTimer(bool noRain)
	{
		if (noRain)
		{
			if (m_BottlesToProcess && m_BottlesToProcess.Count() > 0)
			{
				foreach(Bottle_Base bottle : m_BottlesToProcess)
					bottle.SetCatchingRain(false); // Inform client
			}
		}

		m_RainWaterTimer.Stop();
		m_RainWaterTimer = NULL;
		m_BottlesToProcess.Clear();
		m_BottlesToProcess = NULL;
	}
};