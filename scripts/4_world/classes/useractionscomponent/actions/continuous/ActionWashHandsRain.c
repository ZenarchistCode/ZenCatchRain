class ActionWashHandsRainCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousRepeat(UATimeSpent.WASH_HANDS * 10);
	}
};

class ActionWashHandsRain : ActionContinuousBase
{
	void ActionWashHandsRain()
	{
		m_CallbackClass = ActionWashHandsRainCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_WASHHANDSPOND;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#wash_hands";
	}

	override void CreateConditionComponents()
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTNone;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (GetGame().IsClient())
		{
			// Weather must be raining, player must look up, player must be outside to start action
			if (!ZenClientFunctions.IsRaining() || ZenClientFunctions.GetCameraPitch() < ZenRainWaterConstants.CAMERA_PITCH_TO_CATCH_RAIN || MiscGameplayFunctions.IsUnderRoof(player))
				return false;
		}

		return (GetGame().IsMultiplayer() && GetGame().IsServer()) || (player.HasBloodyHands() && !player.GetItemInHands());
	}

	override bool ActionConditionContinue(ActionData action_data)
	{
		if (GetGame().IsClient())
		{
			// Weather must be raining, player must be outside to continue washing hands
			if (!ZenClientFunctions.IsRaining() || MiscGameplayFunctions.IsUnderRoof(action_data.m_Player))
				return false;
		}

		return (GetGame().IsMultiplayer() && GetGame().IsServer()) || (action_data.m_Player.HasBloodyHands() && !action_data.m_Player.GetItemInHands());
	}

	// Clean hands
	override void OnFinishProgressServer(ActionData action_data)
	{
		PluginLifespan module_lifespan = PluginLifespan.Cast(GetPlugin(PluginLifespan));
		module_lifespan.UpdateBloodyHandsVisibility(action_data.m_Player, false);
	}
};