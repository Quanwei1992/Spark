namespace Spark
{
	public class Input
	{
		public static bool IsKeyPressed(KeyCode keycode)
		{
			return InternalCalls.Input_IsKeyPressed(keycode);
		}

		public static bool IsMouseButtonPressed(MouseCode button)
		{
			return InternalCalls.Input_IsMouseButtonPressed(button);
		}

		public static Vector2 MousePosition
		{
			get
			{
				InternalCalls.Input_GetMousePosition(out float x,out float y);
				return new Vector2(x,y);
			}
		}
	}
}
