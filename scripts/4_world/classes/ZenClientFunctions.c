class ZenClientFunctions
{
	// Is it currently raining heavily and overcast?
	static bool IsRaining()
	{
		return GetGame().GetWeather().GetRain().GetActual() >= ZenRainWaterConstants.REQUIRED_RAIN && GetGame().GetWeather().GetOvercast().GetActual() >= ZenRainWaterConstants.REQUIRED_OVERCAST;
	}

	// Client-side only. Gets camera angle.
	static int GetCameraPitch()
	{
		if (!GetGame())
			return 0;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		float pitch = 0;

		if (player)
		{
			DayZPlayerCamera camera = player.GetCurrentCamera();

			if (camera)
				pitch = camera.GetCurrentPitch();
		}

		return pitch;
	}
};