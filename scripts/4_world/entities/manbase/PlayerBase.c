modded class PlayerBase
{
	void SetActions(out TInputActionMap InputActionMap)
	{
		super.SetActions(InputActionMap);
		AddAction(ActionWashHandsRain, InputActionMap);
	}
};