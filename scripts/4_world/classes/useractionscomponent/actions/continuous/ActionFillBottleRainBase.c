class ActionFillBottleRainBaseCB : ActionContinuousBaseCB
{
	// Create action component with appropriate fill speed
	override void CreateActionComponent()
	{
		float time = ZenRainWaterConstants.FILL_RAIN_SPEED;

		// Fill cooking pots faster than bottles.
		if (m_ActionData && m_ActionData.m_MainItem && m_ActionData.m_MainItem.IsKindOf("Pot"))
			time = ZenRainWaterConstants.FILL_RAIN_SPEED * ZenRainWaterConstants.FILL_MULTI_POT;

		// Keep in mind CAContinuousFillRain adjusts fill speed based on player movement (run, walk, idle etc)
		m_ActionData.m_ActionComponent = new CAContinuousFillRain(time);
	}
};

class ActionFillBottleRainBase : ActionContinuousBase
{
	// Set up action parameters.
	void ActionFillBottleRainBase()
	{
		m_CallbackClass		= ActionFillBottleRainBaseCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONMOD_CATCHRAIN;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONMOD_CATCHRAIN;
		m_FullBody			= false; // Allow movement by default. Set to false to disable, also set HasProneException() to false and change m_CommandUID to CMD_ACTIONFB_FILLBOTTLEPOND
		m_SpecialtyWeight	= UASoftSkillsWeight.PRECISE_LOW;
		m_Text				= "#STR_ZenRain_Txt";
		m_StanceMask		= DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}

	// Only condition is a non-ruined item (bottle_base)
	override void CreateConditionComponents()
	{
		m_ConditionItem	  = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}

	// For some reason CMD_ACTIONMOD_CATCHRAIN only works if prone exception is turned on...
	// But if player is actually prone when catching rain, his character turns into a flying superman pose. So I disable ActionCondition while prone.
	override bool HasProneException()
	{
		return true;
	}

	// Can player catch rain?
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item.IsFullQuantity())
			return false;
		if (GetGame().IsServer() && GetGame().IsMultiplayer())
			return true;

		// Client-side checks
		if (!player.IsAlive() || player.IsPlacingLocal())
			return false;
		if (!Liquid.CanFillContainer(item, LIQUID_WATER))
			return false;
		if (player.m_MovementState.m_iStanceIdx == DayZPlayerConstants.STANCEIDX_PRONE)
			return false;

		// Client-side action condition check - are we outside, is it raining and are we looking up?
		return ZenClientFunctions.IsRaining() && ZenClientFunctions.GetCameraPitch() >= ZenRainWaterConstants.CAMERA_PITCH_TO_CATCH_RAIN && !MiscGameplayFunctions.IsUnderRoof(player);
	}

	// Can player continue to catch rain? Same conditions as start conditions except player no longer needs to look up to sky.
	override bool ActionConditionContinue(ActionData action_data)
	{
		return action_data.m_MainItem.GetQuantity() < action_data.m_MainItem.GetQuantityMax() && ZenClientFunctions.IsRaining() && !MiscGameplayFunctions.IsUnderRoof(action_data.m_Player) && action_data.m_Player.m_MovementState.m_iStanceIdx != DayZPlayerConstants.STANCEIDX_PRONE;
	}

	// Used to sync relevant data between server/client
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);

		if (HasTarget())
		{
			ctx.Write(action_data.m_Target.GetCursorHitPos());
		}
	}

	// Used to sync relevant data between server/client
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
		super.ReadFromContext(ctx, action_recive_data);

		if (HasTarget())
		{
			vector cursor_position;

			if (!ctx.Read(cursor_position))
				return false;

			action_recive_data.m_Target.SetCursorHitPos(cursor_position);
		}

		return true;
	}
};